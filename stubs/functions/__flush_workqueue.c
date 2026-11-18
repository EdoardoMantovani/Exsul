int __flush_workqueue(struct workqueue_struct *wq){
	int ret = 0;
	struct work_struct *w = NULL;
	struct delayed_work *dw = NULL;

	if (wq == NULL){
		return ret;
	}
	if (wq_in_worker(wq) == true) {
		return ret;
	}
	pthread_mutex_lock(&wq->lock);
	for (;;) {
		delayed_work_timer_fn(wq);

		if (list_empty(&wq->work_list) && list_empty(&wq->delayed_list) && wq->nr_active == 0){
			break;
		}
		pthread_cond_broadcast(&wq->cond);
		if (!list_empty(&wq->work_list) || wq->nr_active != 0) {
			pthread_cond_wait(&wq->cond, &wq->lock);
		}else{
			w = NULL;
			dw = NULL;
			w = list_first_entry(&wq->delayed_list, struct work_struct, entry);
			dw = container_of(w, struct delayed_work, work);
			pthread_cond_timedwait(&wq->cond, &wq->lock, &dw->expires);
		}
	}
	pthread_mutex_unlock(&wq->lock);
	ret = true;
	return ret;
}
