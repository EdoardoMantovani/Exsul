bool ieee80211_rx_data_set_sta(struct ieee80211_rx_data *rx, struct sta_info *sta, int link_id){
	struct ieee80211_vif *ieee80211_vif = NULL;

	if (rx == NULL){
		return false;
	}

	rx->link_id = link_id;
	rx->sta = sta;
	if (sta != NULL) {
		if (sta->sdata.local != NULL){
			rx->local = sta->sdata.local;
		}
		if (rx->sdata == NULL){
			rx->sdata = &sta->sdata;
		}
		if (rx->sdata->vif == NULL && sta->vif != NULL){
			rx->sdata->vif = sta->vif;
		}
		rx->link_sta = NULL;
	}else{
		rx->link_sta = NULL;
	}

	if (rx->sdata == NULL){
		return false;
	}
	if (rx->local == NULL){
		rx->local = rx->sdata->local ? rx->sdata->local : director.ieee80211_local;
	}

	if (link_id >= 0) {
		ieee80211_rx_data_set_link(rx, link_id);
		return rx->link != NULL;
	}

	rx->link = &rx->sdata->deflink;
	if (rx->link->sdata == NULL){
		rx->link->sdata = rx->sdata;
	}
	rx->link->link_id = 0;
	ieee80211_vif = rx->sdata->vif;
	if (rx->link->conf == NULL && rx->sdata->vif != NULL){
		rx->link->conf = &ieee80211_vif->bss_conf;
	}
	if (rx->sdata->link[0] == NULL){
		rx->sdata->link[0] = rx->link;
	}

	return rx->link != NULL;
}
