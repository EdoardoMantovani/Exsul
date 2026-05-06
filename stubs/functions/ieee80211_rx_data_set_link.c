void ieee80211_rx_data_set_link(struct ieee80211_rx_data *rx, unsigned int link_id){
	struct ieee80211_link_data *link = NULL;
	struct ieee80211_vif *ieee80211_vif = NULL;

	if (rx == NULL || rx->sdata == NULL){
		return;
	}

	rx->link_id = link_id;
	if (link_id >= IEEE80211_MLD_MAX_NUM_LINKS){
		rx->link = NULL;
		return;
	}

	link = rx->sdata->link[link_id];
	if (link == NULL && link_id == 0){
		link = &rx->sdata->deflink;
		rx->sdata->link[0] = link;
	}

	rx->link = link;
	if (rx->link == NULL){
		return;
	}

	if (rx->link->sdata == NULL){
		rx->link->sdata = rx->sdata;
	}
	rx->link->link_id = link_id;
	ieee80211_vif = rx->sdata->vif;
	if (rx->link->conf == NULL && rx->sdata->vif != NULL){
		rx->link->conf = &ieee80211_vif->bss_conf;
	}
}
