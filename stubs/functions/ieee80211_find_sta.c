struct ieee80211_sta *ieee80211_find_sta(struct ieee80211_vif *vif, const u8 *addr){
	struct sta_info *sta = NULL;
	struct connection_fsm *fsm = NULL;

	if (vif == NULL || addr == NULL){
		return NULL;
	}

	sta = sta_info_get_bss(NULL, (u8 *)addr);
	if (sta != NULL && sta->uploaded && sta->sta != NULL){
		return sta->sta;
	}

	fsm = connection_get();
	if (fsm != NULL && !fsm->stopping && fsm->sta != NULL && ether_addr_equal(addr, fsm->bssid)){
		return fsm->sta;
	}

	return NULL;
}
