int queue_work_on(int cpu, struct workqueue_struct *wq, struct work_struct *work){
	int ret = 0;
	long expected = 0;
	long desired = 0;
	pthread_attr_t attr;
	printf("queue_work_on\n");
	if (wq == NULL || work == NULL){
		if(wq == NULL){
			printf("wq is null\n");
		}
		return 0;
	}

	printf("queue_work_on1\n");
	desired = work_encode(wq, WORK_STRUCT_PENDING);
	printf("queue_work_on2\n");
	ret = atomic_compare_exchange_strong_explicit(&work->data, &expected, desired, memory_order_acq_rel, memory_order_relaxed);
	if(ret == 0){
		printf("atomic_compare_exchange_strong_explicit!\n");
		return ret;
	}
	printf("queue_work_on3\n");
	pthread_mutex_lock(&wq->lock);
	if (wq->stop == true) {
		pthread_mutex_unlock(&wq->lock);
		atomic_store_explicit(&work->data, 0, memory_order_release);
		return ret;
	}
	printf("queue_work_on5\n");
	//
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
