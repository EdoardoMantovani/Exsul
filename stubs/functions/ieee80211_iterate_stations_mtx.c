void ieee80211_iterate_stations_mtx(struct ieee80211_hw *hw, void (*iterator)(void *data, struct ieee80211_sta *sta), void *data){
	ieee80211_iterate_stations_atomic(hw, iterator, data);
}
