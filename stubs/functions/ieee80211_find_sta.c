struct ieee80211_sta *ieee80211_find_sta(struct ieee80211_vif *vif, const u8 *addr){
	struct sta_info *sta = NULL;

	if (vif == NULL){
		return NULL;
	}

	sta = sta_info_get_bss(NULL, addr);
	if (sta == NULL){
		return NULL;
	}

	if (sta->uploaded == NULL){
		return NULL;
	}

	return sta->sta;
}
