void ieee80211_scan_completed(struct ieee80211_hw *hw, struct cfg80211_scan_info *info){
	set_bit(SCAN_COMPLETED, &director.ieee80211_local->scanning);
	if (info->aborted){
		set_bit(SCAN_ABORTED, &director.ieee80211_local->scanning);
	}

	memcpy(&director.ieee80211_local->scan_info, info, sizeof(*info));

//	wiphy_delayed_work_queue(local->hw.wiphy, &local->scan_work, 0);
}
