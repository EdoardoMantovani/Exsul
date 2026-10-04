void ieee80211_ready_on_channel(struct ieee80211_hw *hw){
	struct ieee80211_local *local = NULL;

	local = director.ieee80211_local;
	wiphy_work_queue(local->wiphy, &local->hw_roc_start);
}
