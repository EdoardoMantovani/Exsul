void ieee80211_iterate_stations_atomic(struct ieee80211_hw *hw, void (*iterator)(void *data, struct ieee80211_sta *sta), void *data){
	struct sta_info *sta = NULL;
	struct ieee80211_local *local = NULL;

	local = director.ieee80211_local;
	if (local == NULL){
		return;
	}
	list_for_each_entry(sta, &local->sta_list, list) {
		if (sta->sta != NULL){
			iterator(data, sta->sta);
		}
	}
}
