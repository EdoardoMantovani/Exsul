int mod_timer(struct timer_list *timer, unsigned long expires){
	int ret = 0;
	int was_pending = 0;
	struct hlist_node *next = NULL;
	struct hlist_node **pprev = NULL;

	pthread_mutex_lock(&director.timer_lock);
	if(director.timer_stopping){
		pthread_mutex_unlock(&director.timer_lock);
		return ret;
	}
	pthread_once(&director.timer_once, timer_thread_init);

	if (timer->entry.pprev != NULL) {
		was_pending = 1;
		next  = timer->entry.next;
		pprev = timer->entry.pprev;
		*pprev = next;
                if (next != NULL){
			next->pprev = pprev;
		}
		timer->entry.next  = NULL;
		timer->entry.pprev = NULL;
	}

	timer->expires = expires;
	__enqueue_timer(timer);
	pthread_cond_signal(&director.timer_cond);
	pthread_mutex_unlock(&director.timer_lock);
	return was_pending;
}
