int queue_work_on(int cpu, struct workqueue_struct *wq, struct work_struct *work){
	int ret = 0;
	long expected = 0;
	long desired = 0;
	pthread_attr_t attr;

	if (wq == NULL || work == NULL){
		if(wq == NULL){
			printf("[%s] wq is null!\n", __FUNCTION__);
		}
		return ret;
	}

	expected = atomic_load_explicit(&work->data, memory_order_acquire);
	if (expected & (WORK_STRUCT_PENDING | WORK_STRUCT_RUNNING)) {
		#ifdef ENABLE_DBG_PRINT
		printf("[%s] work already pending or running!\n", __FUNCTION__);
		#endif
		return ret;
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

	if (wq->thread == 0) {
		pthread_attr_init(&attr);
		pthread_attr_setstacksize(&attr, 4 * 1024 * 1024);
		pthread_create(&wq->thread, &attr, worker_thread, wq);
		pthread_attr_destroy(&attr);
	}

	list_add_tail(&work->entry, &wq->work_list);
	pthread_cond_signal(&wq->cond);
	pthread_mutex_unlock(&wq->lock);

	ret = true;
	return ret;
}
