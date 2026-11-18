void ieee80211_ready_on_channel(struct ieee80211_hw *hw){
	int ret = 0;
	struct ieee80211_local *local = NULL;

	ret = peer_ready_on_channel(hw, true);
	if(ret){
		return;
	}
	local = director.ieee80211_local;
	wiphy_work_queue(local->wiphy, &local->hw_roc_start);
}
