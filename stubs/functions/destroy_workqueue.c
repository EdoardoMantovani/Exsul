void destroy_workqueue(struct workqueue_struct *wq){
	pthread_t thread;
	bool free_wq = true;
	struct list_head *n = NULL;
	struct list_head *pos = NULL;
	struct work_struct *work = NULL;

	if(wq == NULL){
		goto out;
	}
	if(wq == &director.system_wq || wq == &director.system_dfl_wq || wq == &director.system_freezable_wq){
		free_wq = false;
	}

	pthread_mutex_lock(&wq->lock);

	list_for_each_safe(pos, n, &wq->work_list) {
		work = container_of(pos, struct work_struct, entry);
		list_del_init(&work->entry);
		atomic_store_explicit(&work->data, 0, memory_order_release);
	}

	list_for_each_safe(pos, n, &wq->delayed_list) {
		work = container_of(pos, struct work_struct, entry);
		list_del_init(&work->entry);
		atomic_store_explicit(&work->data, 0, memory_order_release);
	}

	wq->stop = true;
	thread = wq->thread;
	pthread_cond_broadcast(&wq->cond);
	pthread_mutex_unlock(&wq->lock);

	if (thread != 0){
		pthread_join(thread, NULL);
	}

	pthread_cond_destroy(&wq->cond);
	pthread_mutex_destroy(&wq->lock);
	if(free_wq == true){
		free(wq);
	}

	out:
}
