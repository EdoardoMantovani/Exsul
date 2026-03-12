void ieee80211_remain_on_channel_expired(struct ieee80211_hw *hw){
	struct ieee80211_local *local = NULL;

	local = &director.ieee80211_local;
	/* invoke 'ieee80211_hw_roc_done' */
	wiphy_work_queue(local->wiphy, &local->hw_roc_done);
}
