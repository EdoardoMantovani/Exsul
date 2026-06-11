void wiphy_work_flush(struct wiphy *wiphy, struct wiphy_work *work){
	bool run = false;
	struct ieee80211_local *local = director.ieee80211_local;

	if(local == NULL || director.rdev == NULL){
		return;
	}

	pthread_mutex_lock(&local->wiphy_work_queue_mutex);
	if(work != NULL && (work->entry.next == NULL || work->entry.prev == NULL)){
		INIT_LIST_HEAD(&work->entry);
	}
	run = (work == NULL) || !list_empty(&work->entry);
	pthread_mutex_unlock(&local->wiphy_work_queue_mutex);

	if(run){
		cfg80211_process_wiphy_works(director.rdev, work);
	}
}
