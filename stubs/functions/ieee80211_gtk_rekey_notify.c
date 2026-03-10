void ieee80211_gtk_rekey_notify(struct ieee80211_vif *vif, const u8 *bssid, const u8 *replay_ctr, int gfp){
	cfg80211_gtk_rekey_notify(NULL, bssid, replay_ctr, gfp);
}
