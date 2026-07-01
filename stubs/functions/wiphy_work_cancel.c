void wiphy_work_cancel(struct wiphy *wiphy, struct wiphy_work *work){
	struct ieee80211_local *local = NULL;

	local = director.ieee80211_local;

	if(local == NULL || work == NULL){
		goto end;
	}

	pthread_mutex_lock(&local->wiphy_work_queue_mutex);
	if(work->entry.next == NULL || work->entry.prev == NULL){
		INIT_LIST_HEAD(&work->entry);
	}else if(!list_empty(&work->entry)){
		list_del_init(&work->entry);
	}
	pthread_mutex_unlock(&local->wiphy_work_queue_mutex);
	end:
}
