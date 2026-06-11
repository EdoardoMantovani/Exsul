/* this header will contain everything related to the workqueues and concurrency */

#ifndef ARCH_SET_GS
#define ARCH_SET_GS 0x1001
#endif

static __thread void *percpu_area = NULL;

static inline void setup_percpu(void){
	#ifdef __linux__
	if (percpu_area == NULL) {
		percpu_area = mmap(NULL, 1024 * 1024 * 8, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (percpu_area == MAP_FAILED){
			percpu_area = NULL;
		}
	}
	if (percpu_area != NULL){
		syscall(SYS_arch_prctl, ARCH_SET_GS, (unsigned long)percpu_area);
	}
	#else
	/* need to find a fix for Windows too, apparently it doesn't permit to manipulate the GS register safely */
	#endif
}

static __thread struct task_struct *tls_current;
static struct tasklet_head tasklet_global_queue;

static inline void tasklet_schedule(struct tasklet_struct *t){
	if (!test_and_set_bit(TASKLET_STATE_SCHED, &t->state)){
		__tasklet_schedule(t);
	}
}

/**
 * worker_thread - the worker thread function
 * @__worker: self
 *
 * The worker thread function.  All workers belong to a worker_pool -
 * either a per-cpu one or dynamic unbound one.  These workers process all
 * work items regardless of their specific target workqueue.  The only
 * exception is work items which belong to workqueues with a rescuer which
 * will be explained in rescuer_thread().
 *
 * Return: 0
 */
static void *worker_thread(void *arg){
	struct work_struct *work = NULL;
	struct workqueue_struct *wq = NULL;

	setup_percpu();
	wq = (struct workqueue_struct *)arg;
	pthread_mutex_lock(&wq->lock);
	for (;;) {
		if (wq->stop && list_empty(&wq->work_list) && list_empty(&wq->delayed_list) && wq->nr_active == 0){
			break;
		}
		delayed_work_timer_fn(wq);

		if (!list_empty(&wq->work_list)) {
			work = list_first_entry(&wq->work_list, struct work_struct, entry);
			list_del_init(&work->entry);
			atomic_store_explicit(&work->data, work_encode(wq, WORK_STRUCT_RUNNING), memory_order_relaxed);
			wq->nr_active++;

			pthread_mutex_unlock(&wq->lock);
			pthread_mutex_lock(&director.usb_io_lock);
			while (director.usb_io_depth > 0){
				pthread_cond_wait(&director.usb_io_idle, &director.usb_io_lock);
			}
			pthread_mutex_unlock(&director.usb_io_lock);
			work->func(work);
			pthread_mutex_lock(&wq->lock);

			wq->nr_active--;
			if (atomic_load_explicit(&work->data, memory_order_acquire) & WORK_STRUCT_PENDING){
				atomic_store_explicit(&work->data, work_encode(wq, WORK_STRUCT_PENDING), memory_order_release);
			}else{
				atomic_store_explicit(&work->data, 0, memory_order_release);
			}
			pthread_cond_broadcast(&wq->cond);
			continue;
		}

		if (wq->stop) {
			pthread_cond_wait(&wq->cond, &wq->lock);
			continue;
		}

		if (list_empty(&wq->delayed_list)) {
			pthread_cond_wait(&wq->cond, &wq->lock);
		}else{
			struct delayed_work *dw = NULL;
			struct work_struct *w = NULL;
			w = list_first_entry(&wq->delayed_list, struct work_struct, entry);
			dw = container_of(w, struct delayed_work, work);
			pthread_cond_timedwait(&wq->cond, &wq->lock, &dw->expires);
		}
	}
	pthread_mutex_unlock(&wq->lock);
	return NULL;
}

static struct task_struct *current_thread_info(void){
	struct task_struct *current_task = NULL;
	pthread_condattr_t current_condition_attr;

	if (tls_current != NULL){
		return tls_current;
	}

	current_task = (struct task_struct *)malloc(sizeof(struct task_struct));

	if(current_task == NULL){
		return NULL;
	}

	pthread_mutex_init(&current_task->lock, NULL);

	pthread_condattr_init(&current_condition_attr);
	pthread_condattr_setclock(&current_condition_attr, CLOCK_MONOTONIC);
	pthread_cond_init(&current_task->cond, &current_condition_attr);
	pthread_condattr_destroy(&current_condition_attr);

	atomic_store(&current_task->__state, TASK_RUNNING);
	atomic_store(&current_task->started, true);
	atomic_store(&current_task->should_stop, false);

	current_task->tid = pthread_self();
	snprintf(current_task->name, sizeof(current_task->name), "main");

	tls_current = current_task;
	return current_task;
}

static inline bool wq_entry_is_linked(const struct wait_queue_entry *wq_entry){
	return !(wq_entry->entry.next == &wq_entry->entry && wq_entry->entry.prev == &wq_entry->entry);
}

/* kthread_trampoline: main function used for invoking the kthread cb */
static void *kthread_trampoline(void *arg){
	int ret = 0;
	struct task_struct *current_thread = NULL;
	setup_percpu();
	current_thread = (struct task_struct *)arg;
	tls_current = current_thread;

	pthread_mutex_lock(&current_thread->lock);

	/* wait until the 'current_thread' has the 'started' flag set and the 'should_stop' flag isn't set */
	while (!atomic_load(&current_thread->started) && !atomic_load(&current_thread->should_stop)){
		pthread_cond_wait(&current_thread->cond, &current_thread->lock);
	}
	pthread_mutex_unlock(&current_thread->lock);

	if (!atomic_load(&current_thread->should_stop) && current_thread->threadfn){
		/* invoke the real thread callback */
		ret = current_thread->threadfn(current_thread->data);
	}
	/* set the exit code of the thread callback */
	current_thread->exit_code = ret;

	atomic_store(&current_thread->__state, TASK_RUNNING);
	pthread_mutex_lock(&current_thread->lock);
	pthread_cond_broadcast(&current_thread->cond);
	pthread_mutex_unlock(&current_thread->lock);

	return NULL;
}
