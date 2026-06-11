void wiphy_work_queue(struct wiphy *wiphy, struct wiphy_work *work){
	struct ieee80211_local *local = director.ieee80211_local;

	if(local == NULL || work == NULL || director.rdev == NULL){
		return;
	}

	pthread_mutex_lock(&local->wiphy_work_queue_mutex);
	if(work->entry.next == NULL || work->entry.prev == NULL){
		INIT_LIST_HEAD(&work->entry);
	}
	if(list_empty(&work->entry)){
		list_add_tail(&work->entry, &director.rdev->wiphy_work_list);
	}
	pthread_mutex_unlock(&local->wiphy_work_queue_mutex);
	queue_work(&director.system_dfl_wq, &director.rdev->wiphy_work);
}
