void wiphy_work_cancel(struct wiphy *wiphy, struct wiphy_work *work){
	struct ieee80211_local *local = NULL;

	local = director.ieee80211_local;

	pthread_mutex_lock(&local->wiphy_work_queue_mutex);

	if (!list_empty(&work->entry)){
		list_del_init(&work->entry);
	}
	pthread_mutex_unlock(&local->wiphy_work_queue_mutex);
}
