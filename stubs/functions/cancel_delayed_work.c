int cancel_delayed_work(struct delayed_work *dwork){
	int ret = 0;
	long flags = 0;
	bool cancel = false;
	struct work_struct *work = NULL;
	struct workqueue_struct *wq = NULL;

	if (dwork == NULL){
		return ret;
	}

	work = &dwork->work;
	wq = work_wq(work);

	if (wq == NULL){
		return ret;
	}

	pthread_mutex_lock(&wq->lock);
	flags = work_flags(work);

	if (flags & WORK_STRUCT_PENDING) {
		if (work_is_linked(work)){
			list_del_init(&work->entry);
		}
		atomic_store_explicit(&work->data, 0, memory_order_release);
		pthread_cond_broadcast(&wq->cond);
		cancel = true;
	}

	pthread_mutex_unlock(&wq->lock);
	return cancel;
}
