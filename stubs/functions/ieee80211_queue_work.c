void ieee80211_queue_work(struct ieee80211_hw *hw, struct work_struct *work){
	struct ieee80211_local *local = NULL;

	local = &director.ieee80211_local;

	if (!ieee80211_can_queue_work(local)){
		return;
	}

	queue_work(local->workqueue, work);
}
