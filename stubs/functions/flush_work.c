int flush_work(struct work_struct *work){
	long ret = 0;
	struct workqueue_struct *wq = NULL;

	if(work == NULL){
		return ret;
	}

	ret = atomic_load_explicit(&work->data, memory_order_acquire);
	if(!(ret & (WORK_STRUCT_PENDING | WORK_STRUCT_RUNNING))){
		return 0;
	}

	wq = (struct workqueue_struct *)(uintptr_t)(ret & ~(long)WORK_STRUCT_FLAG_MASK);

	if(wq == NULL){
		return 0;
	}

	ret = wq_in_worker(wq);

	if(ret){
		errno = EDEADLK;
		return ret;
	}

	pthread_mutex_lock(&wq->lock);

	while(atomic_load_explicit(&work->data, memory_order_acquire)){
		pthread_cond_wait(&wq->cond, &wq->lock);
	}

	pthread_mutex_unlock(&wq->lock);
	return ret;
}
