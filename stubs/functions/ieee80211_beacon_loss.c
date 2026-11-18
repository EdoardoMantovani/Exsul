void ieee80211_beacon_loss(struct ieee80211_vif *vif){
	struct ieee80211_local *local = NULL;

	local = director.ieee80211_local;
	if(vif == NULL || local == NULL || local->sdata == NULL){
		return;
	}
	wiphy_work_queue(&local->wiphy, &local->beacon_connection_loss_work);
}
