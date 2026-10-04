struct ieee80211_sta *__ieee80211_iterate_stations(struct ieee80211_hw *hw, struct ieee80211_sta *prev){
	bool fsm_seen = false;
	bool return_next = prev == NULL;
	struct connection_fsm *fsm = NULL;
	struct ieee80211_local *local = NULL;
	struct sta_info *sta = NULL;

	(void)hw;
	local = director.ieee80211_local;
	fsm = connection_get();

	if(local == NULL || local->sta_list.next == NULL || local->sta_list.prev == NULL){
		goto fallback;
	}

	list_for_each_entry(sta, &local->sta_list, list) {
		if(sta->sta == NULL || !sta->uploaded){
			continue;
		}
		if(fsm != NULL && sta->sta == fsm->sta){
			fsm_seen = true;
		}
		if(return_next){
			return sta->sta;
		}
		if(sta->sta == prev){
			return_next = true;
		}
	}

fallback:
	if(!fsm_seen && fsm != NULL && fsm->sta != NULL){
		if(return_next){
			return fsm->sta;
		}
	}

	return NULL;
}
