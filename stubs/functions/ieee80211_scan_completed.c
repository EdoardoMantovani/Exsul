void ieee80211_scan_completed(struct ieee80211_hw *hw, struct cfg80211_scan_info *info){
	if (director.ieee80211_local != NULL) {
		clear_bit(SCAN_HW_SCANNING, &director.ieee80211_local->scanning);
		set_bit(SCAN_COMPLETED, &director.ieee80211_local->scanning);
		if (info != NULL && info->aborted){
			set_bit(SCAN_ABORTED, &director.ieee80211_local->scanning);
		}else{
			clear_bit(SCAN_ABORTED, &director.ieee80211_local->scanning);
		}
		if (info != NULL){
			memcpy(&director.ieee80211_local->scan_info, info, sizeof(*info));
		}
	}
	scan_complete_defer();
}
