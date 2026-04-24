void ieee80211_stop_queues(struct ieee80211_hw *hw){
	ieee80211_stop_queues_by_reason(hw, IEEE80211_MAX_QUEUE_MAP, IEEE80211_QUEUE_STOP_REASON_DRIVER, false);
}

