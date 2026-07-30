void ieee80211_free_hw(struct ieee80211_hw *hw){
	struct wiphy *wiphy = NULL;
	struct ieee80211_local *local = NULL;
	struct ieee80211_sub_if_data *sdata = NULL;

	local = director.ieee80211_local;
	if(local == NULL){
		goto end;
	}
	wiphy = local->wiphy;
	if(wiphy == NULL){
		goto free_local;
	}

	sta_info_stop(local);
	while (!list_empty(&local->interfaces)) {
		sdata = list_first_entry(&local->interfaces, struct ieee80211_sub_if_data, list);
		list_del_rcu(&sdata->list);
		skb_queue_purge(&sdata->skb_queue);
		skb_queue_purge(&sdata->status_queue);
		kfree(sdata);
	}
	for (int i = 0; i < IEEE80211_MAX_QUEUES; i++) {
		skb_queue_purge(&local->pending[i]);
	}
	skb_queue_purge(&local->skb_queue);
	skb_queue_purge(&local->skb_queue_unreliable);
	kfree(local->int_scan_req);
	local->int_scan_req = NULL;
	kfree(local->ieee80211_vif);
	local->ieee80211_vif = NULL;
	kfree(local->ieee80211_ap_vif);
	local->ieee80211_ap_vif = NULL;
	kfree(local->ieee80211_mon_vif);
	local->ieee80211_mon_vif = NULL;

	for (enum nl80211_band band = 0; band < NUM_NL80211_BANDS; band++) {
		if (!(local->sband_allocated & BIT(band))){
			continue;
		}
		kfree(wiphy->bands[band]);
		wiphy->bands[band] = NULL;
	}
	wiphy_free(wiphy);
	local->wiphy = NULL;
free_local:
	if(director.ieee80211_ops == local->ieee80211_ops){
		director.ieee80211_ops = NULL;
	}
	free(local);
	director.ieee80211_local = NULL;
end:
}
