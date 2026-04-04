struct workqueue_struct *alloc_workqueue(const char *name, unsigned int flags, int max_active){
	struct workqueue_struct *wq = NULL;
	pthread_condattr_t cattr = {};

	wq = (struct workqueue_struct *)calloc(1, sizeof(struct workqueue_struct));
	if (wq == NULL){
		return NULL;
	}

	strncpy(wq->name, name, sizeof(wq->name));
	pthread_mutex_init(&wq->lock, NULL);

	pthread_condattr_init(&cattr);
	wq->clock_id = CLOCK_MONOTONIC;
	pthread_condattr_setclock(&cattr, wq->clock_id);
	pthread_cond_init(&wq->cond, &cattr);
	pthread_condattr_destroy(&cattr);

	INIT_LIST_HEAD(&wq->work_list);
	INIT_LIST_HEAD(&wq->delayed_list);

	wq->thread = 0;
	return wq;
}
