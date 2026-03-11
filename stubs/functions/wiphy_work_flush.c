void wiphy_work_flush(struct wiphy *wiphy, struct wiphy_work *work){
	bool run = false;
	struct ieee80211_local *local = NULL;

	local = &director.ieee80211_local;

	pthread_mutex_lock(&local->wiphy_work_queue_mutex);
	run = (work == NULL) || !list_empty(&work->entry);
	pthread_mutex_unlock(&local->wiphy_work_queue_mutex);

	if (run){
//		cfg80211_process_wiphy_works(NULL, work);
	}
}
