void ieee80211_disconnect(struct ieee80211_vif *vif, bool reconnect){
	struct ieee80211_local *local = director.ieee80211_local;
	struct ieee80211_sub_if_data *sdata = NULL;

	sdata = local->sdata;
	if (sdata->vif->type != NL80211_IFTYPE_STATION){
		return;
	}

	sdata->u.mgd.driver_disconnect = true;
	sdata->u.mgd.reconnect = reconnect;
	/* invoke 'ieee80211_beacon_connection_loss_work' */
	wiphy_work_queue(local->wiphy, &local->beacon_connection_loss_work);
}
