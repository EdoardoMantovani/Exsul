void cfg80211_cqm_rssi_notify(struct net_device *netdev, enum nl80211_cqm_rssi_threshold_event rssi_event, s32 rssi_level, int gfp){
	struct ieee80211_local *local = NULL;
	struct cfg80211_cqm_config *cqm_config = NULL;

	if (rssi_event != NL80211_CQM_RSSI_THRESHOLD_EVENT_LOW && rssi_event != NL80211_CQM_RSSI_THRESHOLD_EVENT_HIGH){
		goto end;
	}
	local = director.ieee80211_local;
	cqm_config = &local->cqm_config;
	if (cqm_config != NULL) {
		cqm_config->last_rssi_event_value = rssi_level;
		cqm_config->last_rssi_event_type = rssi_event;
		wiphy_work_queue(local->wiphy, &local->cqm_rssi_work);
	}
	end:
}
