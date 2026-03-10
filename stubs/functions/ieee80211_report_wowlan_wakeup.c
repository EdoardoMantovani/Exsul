void ieee80211_report_wowlan_wakeup(void *vif, struct cfg80211_wowlan_wakeup *wakeup, int gfp){
	cfg80211_report_wowlan_wakeup(NULL, wakeup, gfp);
}
