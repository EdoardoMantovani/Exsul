void ieee80211_stop_tx_ba_cb_irqsafe(struct ieee80211_vif *vif, u8 *ra, u16 tid){
	struct sta_info *sta = NULL;
	struct tid_ampdu_tx *tid_tx = NULL;
	struct ieee80211_sub_if_data *sdata = NULL;
	struct ieee80211_local *local = NULL;

	local = director.ieee80211_local;
	sdata = local->sdata;

	#if 0
	tid_tx = ieee80211_lookup_tid_tx(sdata, ra, tid, &sta);
	if (tid_tx == NULL){
		goto end;
	}

	set_bit(HT_AGG_STATE_STOP_CB, &tid_tx->state);
	wiphy_work_queue(local->wiphy, &local->ampdu_mlme.work);
	#endif
	end:
}
