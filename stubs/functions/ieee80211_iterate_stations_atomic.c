void ieee80211_iterate_stations_atomic(struct ieee80211_hw *hw, void (*iterator)(void *data, struct ieee80211_sta *sta), void *data){
	bool fsm_seen = false;
	void (*iter)(void *data, struct ieee80211_sta *sta) = iterator;
	struct sta_info *sta = NULL;
	struct connection_fsm *fsm = NULL;
	struct ieee80211_local *local = NULL;

	local = director.ieee80211_local;
	fsm = connection_get();
	if (local == NULL || iter == NULL || (uintptr_t)iter <= 4096){
		goto end;
	}
	list_for_each_entry(sta, &local->sta_list, list) {
		if (sta->sta != NULL){
			if (fsm != NULL && sta->sta == fsm->sta){
				fsm_seen = true;
			}
			iter(data, sta->sta);
		}
	}
	if (!fsm_seen && fsm != NULL && fsm->sta != NULL){
		iter(data, fsm->sta);
	}
	end:
}
