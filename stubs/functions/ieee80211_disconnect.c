void ieee80211_disconnect(struct ieee80211_vif *ieee80211_vif, bool reconnect){
	struct ieee80211_sub_if_data *sdata = NULL;
	struct ieee80211_local *local = NULL;

	local = director.ieee80211_local;
	sdata = local->sdata;
	if (ieee80211_vif->type != NL80211_IFTYPE_STATION){
		return;
	}

	sdata->u.mgd.driver_disconnect = true;
	sdata->u.mgd.reconnect = reconnect;
	/* invoke 'ieee80211_beacon_connection_loss_work' */
	wiphy_work_queue(local->wiphy, &local->beacon_connection_loss_work);
}
