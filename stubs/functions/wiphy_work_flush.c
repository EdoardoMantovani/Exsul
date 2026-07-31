void wiphy_work_flush(struct wiphy *wiphy, struct wiphy_work *work){
	bool run = false;
	pthread_mutex_t *lock = wiphy_work_queue_lock();

	if(lock == NULL || director.rdev == NULL){
		goto end;
	}

	pthread_mutex_lock(lock);
	if(work != NULL && (work->entry.next == NULL || work->entry.prev == NULL)){
		INIT_LIST_HEAD(&work->entry);
	}
	run = (work == NULL) || !list_empty(&work->entry);
	pthread_mutex_unlock(lock);

	if(run){
		cfg80211_process_wiphy_works(director.rdev, work);
	}
	end:
}
