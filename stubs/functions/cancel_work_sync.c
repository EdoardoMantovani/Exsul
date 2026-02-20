int cancel_work_sync(struct work_struct *work){
	long ret = 0;
	int cancel = 0;
	struct workqueue_struct *wq = NULL;

	if (work == NULL){
		return ret;
	}

	ret = atomic_load_explicit(&work->data, memory_order_acquire);
	if (ret == 0){
		return ret;
	}

	wq = (struct workqueue_struct *)(uintptr_t)(ret & ~(long)WORK_STRUCT_FLAG_MASK);

	if (wq == NULL){
		return ret;
	}

	pthread_mutex_lock(&wq->lock);

	if (work_flags(work) & WORK_STRUCT_PENDING) {
		if (work_is_linked(work)){
			list_del_init(&work->entry);
		}
		atomic_store_explicit(&work->data, 0, memory_order_release);
		pthread_cond_broadcast(&wq->cond);
		cancel = true;
	}

	pthread_mutex_unlock(&wq->lock);
	flush_work(work);
	return cancel;
}
