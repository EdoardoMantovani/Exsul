int queue_work_on(int cpu, struct workqueue_struct *wq, struct work_struct *work){
	int ret = 0;
	long expected = 0;
	long desired = 0;

	if (wq == NULL || work == NULL){
		return 0;
	}

	desired = work_encode(wq, WORK_STRUCT_PENDING);

	ret = atomic_compare_exchange_strong_explicit(&work->data, &expected, desired, memory_order_acq_rel, memory_order_relaxed);
	if(ret == 0){
		return ret;
	}

	pthread_mutex_lock(&wq->lock);
	if (wq->stop == true) {
		pthread_mutex_unlock(&wq->lock);
		atomic_store_explicit(&work->data, 0, memory_order_release);
		return ret;
	}

	list_add_tail(&work->entry, &wq->work_list);
	pthread_cond_signal(&wq->cond);
	pthread_mutex_unlock(&wq->lock);

	ret = true;
	return ret;
}
