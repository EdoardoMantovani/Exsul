void cfg80211_gtk_rekey_notify(struct net_device *dev, const u8 *bssid, const u8 *replay_ctr, int gfp){
	nl80211_gtk_rekey_notify(dev, dev, bssid, replay_ctr, gfp);
}
