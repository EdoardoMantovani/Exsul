struct workqueue_struct *alloc_workqueue(const char *name, unsigned int flags, int max_active){
	int ret = 0;
	struct workqueue_struct *wq = NULL;
	pthread_condattr_t cattr = {};
	printf("1\n");
	wq = (struct workqueue_struct *)calloc(1, sizeof(struct workqueue_struct));
	if (wq == NULL){
		return NULL;
	}

	printf("2\n");
	snprintf(wq->name, sizeof(wq->name), "%s", name ? name : "wq");
	printf("3\n");
	pthread_mutex_init(&wq->lock, NULL);

	pthread_condattr_init(&cattr);
	wq->clock_id = CLOCK_MONOTONIC;
	pthread_condattr_setclock(&cattr, wq->clock_id);
	pthread_cond_init(&wq->cond, &cattr);
	pthread_condattr_destroy(&cattr);

	INIT_LIST_HEAD(&wq->work_list);
	INIT_LIST_HEAD(&wq->delayed_list);

	ret = pthread_create(&wq->thread, NULL, worker_thread, wq);
	if(ret < 0){
		pthread_cond_destroy(&wq->cond);
		pthread_mutex_destroy(&wq->lock);
		free(wq);
		return NULL;
	}else{
		return wq;
	}
}
