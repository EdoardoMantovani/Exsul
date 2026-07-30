int queue_work_on(int cpu, struct workqueue_struct *wq, struct work_struct *work){
	long state;
	pthread_attr_t attr;

	if (wq == NULL || work == NULL || work->func == NULL || (uintptr_t)work->func <= 4096){
		if (wq == NULL){
			printf("[%s] wq is null!\n", __FUNCTION__);
		}
		return false;
	}

	pthread_mutex_lock(&wq->lock);
	state = atomic_load_explicit(&work->data, memory_order_acquire);
	if (state & WORK_STRUCT_PENDING){
		pthread_mutex_unlock(&wq->lock);
		return false;
	}
	if (wq->stop){
		pthread_mutex_unlock(&wq->lock);
		return false;
	}

	if (wq->thread == 0) {
		pthread_attr_init(&attr);
		pthread_attr_setstacksize(&attr, 4 * 1024 * 1024);
		pthread_create(&wq->thread, &attr, worker_thread, wq);
		pthread_attr_destroy(&attr);
	}

	atomic_store_explicit(&work->data,
			      work_encode(wq, WORK_STRUCT_PENDING | (state & WORK_STRUCT_RUNNING)),
			      memory_order_release);
	list_add_tail(&work->entry, &wq->work_list);
	pthread_cond_signal(&wq->cond);
	pthread_mutex_unlock(&wq->lock);

	return true;

}
