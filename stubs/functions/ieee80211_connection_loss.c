void ieee80211_connection_loss(struct ieee80211_vif *vif){
	struct ieee80211_sub_if_data *sdata = NULL;
	struct ieee80211_local *local = NULL;

	local = &director.ieee80211_local;
	sdata = local->sdata;

	sdata->u.mgd.connection_loss = true;
	//wiphy_work_queue(hw->wiphy, &sdata->u.mgd.beacon_connection_loss_work);
}
