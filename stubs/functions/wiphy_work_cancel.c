void wiphy_work_cancel(struct wiphy *wiphy, struct wiphy_work *work){
	pthread_mutex_t *lock = NULL;

	lock = wiphy_work_queue_lock();
	if(lock == NULL || work == NULL){
		goto end;
	}
	pthread_mutex_lock(lock);
	if(work->entry.next == NULL || work->entry.prev == NULL){
		INIT_LIST_HEAD(&work->entry);
	}else if(!list_empty(&work->entry)){
		list_del_init(&work->entry);
	}
	pthread_mutex_unlock(lock);
	end:
}
