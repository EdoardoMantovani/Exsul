/* contains the main code used for calculating/emulating jiffies */

/**
 * time_is_after_jiffies - return true if a is after jiffies
 * @a: time (unsigned long) to compare to jiffies
 *
 * Return: %true is time a is after jiffies, otherwise %false.
 */
#define time_is_after_jiffies(a) time_before(jiffies, a)

/*
 * HZ is equal to or smaller than 1000, and 1000 is a nice round
 * multiple of HZ, divide with the factor between them, but round
 * upwards:
 */
static inline unsigned long _msecs_to_jiffies(const unsigned int m){
	return (m + (MSEC_PER_SEC / HZ) - 1) / (MSEC_PER_SEC / HZ);
}

#if !(USEC_PER_SEC % HZ)
static inline unsigned long _usecs_to_jiffies(const unsigned int u){
	return (u + (USEC_PER_SEC / HZ) - 1) / (USEC_PER_SEC / HZ);
}
#else
static inline unsigned long _usecs_to_jiffies(const unsigned int u){
	return (USEC_TO_HZ_MUL32 * u + USEC_TO_HZ_ADJ32) >> USEC_TO_HZ_SHR32;
}
#endif

#define TIMER_NSEC_PER_JIFFY (1000000000ULL / HZ)

static unsigned long round_jiffies_common(unsigned long j, int cpu, bool force_up){
	int rem = 0;
	unsigned long original = j;

	/*
	 * We don't want all cpus firing their timers at once hitting the
	 * same lock or cachelines, so we skew each extra cpu with an extra
	 * 3 jiffies. This 3 jiffies came originally from the mm/ code which
	 * already did this.
	 * The skew is done by adding 3*cpunr, then round, then subtract this
	 * extra offset again.
	 */
	j += cpu * 3;

	rem = j % HZ;

	/*
	 * If the target jiffy is just after a whole second (which can happen
	 * due to delays of the timer irq, long irq off times etc etc) then
	 * we should round down to the whole second, not up. Use 1/4th second
	 * as cutoff for this rounding as an extreme upper bound for this.
	 * But never round down if @force_up is set.
	 */
	if (rem < HZ/4 && !force_up){ /* round down */
		j = j - rem;
	}else{ /* round up */
		j = j - rem + HZ;
	}
	/* now that we have rounded, subtract the extra skew again */
	j -= cpu * 3;

	/*
	 * Make sure j is still in the future. Otherwise return the
	 * unmodified value.
	 */
	if(((long)j - director.jiffies) < 0){
		return j;
	}else{
		return original;
	}
}

static inline unsigned long msecs_to_jiffies(const unsigned int m){
	if (__builtin_constant_p(m)) {
		if (m < 0){
			return MAX_JIFFY_OFFSET;
		}
		return _msecs_to_jiffies(m);
	}else{
		return __msecs_to_jiffies(m);
	}
}

/* timers related functions */
static void *timer_thread_fn(void *arg){
	long long fire_ns = 0;
	long long elapsed_ns = 0;

	struct timespec now = {};
	struct timespec ts = {};
        struct timer_list *first = NULL;
        void (*fn)(struct timer_list *) = NULL;
	struct hlist_node *next = NULL;
	struct hlist_node **pprev = NULL;

	/* timer callbacks run driver code with GS-relative per-CPU accesses */
	setup_percpu();
	pthread_mutex_lock(&director.timer_lock);
	for (;;) {
		if (director.timer_head.first == NULL) {
			pthread_cond_wait(&director.timer_cond, &director.timer_lock);
			continue;
		}
		first = container_of(director.timer_head.first, struct timer_list, entry);
		fire_ns = (long long)first->expires * TIMER_NSEC_PER_JIFFY;
		ts.tv_sec  = director.timer_base.tv_sec  + (time_t)(fire_ns / 1000000000LL);
		ts.tv_nsec = director.timer_base.tv_nsec + (long)(fire_ns % 1000000000LL);
		if (ts.tv_nsec >= 1000000000L) {
			ts.tv_sec++;
			ts.tv_nsec -= 1000000000L;
		}
		pthread_cond_timedwait(&director.timer_cond, &director.timer_lock, &ts);

		clock_gettime(CLOCK_REALTIME, &now);
		elapsed_ns = (now.tv_sec  - director.timer_base.tv_sec)  * 1000000000LL + (now.tv_nsec - director.timer_base.tv_nsec);
		/* update jiffies */
		director.jiffies = (u64)((unsigned long long)elapsed_ns / TIMER_NSEC_PER_JIFFY);

		while (director.timer_head.first != NULL) {
			first = container_of(director.timer_head.first, struct timer_list, entry);
			if ((long)(first->expires - (unsigned long)director.jiffies) > 0){
				break;
			}
			next  = first->entry.next;
			pprev = first->entry.pprev;
			*pprev = next;
			if (next != NULL){
				next->pprev = pprev;
			}
			first->entry.next  = NULL;
			first->entry.pprev = NULL;

			fn = first->function;
			director.timer_executing = first;
			pthread_mutex_unlock(&director.timer_lock);
			fn(first);
			pthread_mutex_lock(&director.timer_lock);
			director.timer_executing = NULL;
			pthread_cond_broadcast(&director.timer_cond);
		}
	}
	pthread_mutex_unlock(&director.timer_lock);
	return NULL;
}

/* the mechanics of timers are really similar to the workqueues, there is a global list
 * where timers are added and invoked at a specific time
 */
static void timer_thread_init(void){
	pthread_attr_t attr;

	clock_gettime(CLOCK_REALTIME, &director.timer_base);
	pthread_attr_init(&attr);
	pthread_create(&director.timer_thread, &attr, timer_thread_fn, NULL);
	pthread_attr_destroy(&attr);
}

/* delete expired timers and enqueue a (new) timer */
static void __enqueue_timer(struct timer_list *timer){
	struct timer_list *t = NULL;
	struct hlist_node **pprev = NULL;
	struct hlist_node  *curr  = NULL;

	pprev = &director.timer_head.first;
	curr  = director.timer_head.first;

	while (curr != NULL) {
		t = container_of(curr, struct timer_list, entry);
		if (timer->expires <= t->expires){
			break;
		}
		pprev = &curr->next;
		curr  = curr->next;
	}

	timer->entry.next  = curr;
	timer->entry.pprev = pprev;
	if (curr != NULL){
		curr->pprev = &timer->entry.next;
	}
	*pprev = &timer->entry;
}
