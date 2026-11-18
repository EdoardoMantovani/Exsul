int mod_delayed_work_on(int cpu, struct workqueue_struct *wq, struct delayed_work *dwork, unsigned long delay){
	int ret = 0;
	long flags = 0;
	long desired = 0;
	long expected = 0;
	bool was_pending = 0;
	struct work_struct *work = NULL;

	if (wq == NULL || dwork == NULL){
		return ret;
	}

	work = &dwork->work;
	if (work->func == NULL || (uintptr_t)work->func <= 4096){
		return ret;
	}

	if(work_wq(work) == wq && wq_in_worker(wq) && (work_flags(work) & WORK_STRUCT_RUNNING)) {
		errno = EDEADLK;
		return ret;
	}

	pthread_mutex_lock(&wq->lock);

	flags = work_flags(work);
	was_pending = !!(flags & WORK_STRUCT_PENDING);

	if ((flags & WORK_STRUCT_PENDING) && work_is_linked(work)){
		list_del_init(&work->entry);
	}

	if (flags & WORK_STRUCT_RUNNING) {
		while (work_flags(work) & WORK_STRUCT_RUNNING){
			pthread_cond_wait(&wq->cond, &wq->lock);
		}
		flags = work_flags(work);
		was_pending = false;
	}

	if (!(flags & (WORK_STRUCT_PENDING | WORK_STRUCT_RUNNING))) {
		expected = 0;
		desired = work_encode(wq, WORK_STRUCT_PENDING | WORK_STRUCT_DELAYED);
		ret = atomic_compare_exchange_strong_explicit(&work->data, &expected, desired, memory_order_acq_rel, memory_order_relaxed);
		if(ret == 0) {
			pthread_mutex_unlock(&wq->lock);
			return ret;
		}
	}else{
		atomic_store_explicit(&work->data, work_encode(wq, WORK_STRUCT_PENDING | WORK_STRUCT_DELAYED), memory_order_relaxed);
	}

	dwork->expires = timespec_add_ms(wq_now(wq), delay);
	delayed_list_insert_sorted(wq, dwork);

	pthread_cond_signal(&wq->cond);
	pthread_mutex_unlock(&wq->lock);

	return was_pending;
}
