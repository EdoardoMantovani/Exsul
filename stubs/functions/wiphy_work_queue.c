void wiphy_work_queue(struct wiphy *wiphy, struct wiphy_work *work){
	struct ieee80211_local *local = NULL;

	local = &director.ieee80211_local;

	pthread_mutex_lock(&local->wiphy_work_queue_mutex);
	if (list_empty(&work->entry)){
		list_add_tail(&work->entry, &local->wiphy_work_list);
	}
	pthread_mutex_unlock(&local->wiphy_work_queue_mutex);

	queue_work(&director.system_dfl_wq, work);
}
