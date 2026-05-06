bool ieee80211_rx_for_interface(struct ieee80211_rx_data *rx, struct sk_buff *skb, bool consume){
	struct ieee80211_rx_status *status = NULL;
	struct ieee80211_hdr *hdr = NULL;
	struct link_sta_info *link_sta = NULL;
	struct sta_info *sta = NULL;
	int link_id = -1;

	if (rx == NULL || skb == NULL || rx->sdata == NULL){
		return false;
	}

	status = IEEE80211_SKB_RXCB(skb);
	hdr = (struct ieee80211_hdr *)skb->data;

	/*
	 * Match mac80211's ordering: try the per-link station table first so
	 * MLO frames can recover the RX link, then fall back to the legacy STA
	 * table keyed by transmitter address.
	 */
	link_sta = link_sta_info_get_bss(rx->sdata, hdr->addr2);
	if (link_sta != NULL) {
		sta = link_sta->sta;
		link_id = link_sta->link_id;
	}else{
		sta = sta_info_get_bss(rx->sdata, hdr->addr2);
		if (status != NULL && status->link_valid){
			link_id = status->link_id;
		}
	}

	if (!ieee80211_rx_data_set_sta(rx, sta, link_id)){
		return false;
	}

	return ieee80211_prepare_and_rx_handle(rx, skb, consume);
}
