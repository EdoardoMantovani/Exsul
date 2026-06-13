void ieee80211_tx_status_irqsafe(struct ieee80211_hw *hw, struct sk_buff *skb){
	struct ieee80211_local *local = director.ieee80211_local;
	struct ieee80211_tx_info *info = NULL;

	if (skb == NULL || IS_ERR(skb)){
		return;
	}
	info = skb->cb;
	if (skb->data != NULL && skb->len >= IEEE80211_MIN_HDR_LEN &&
	    info != NULL && (info->flags & IEEE80211_TX_INTFL_MLME_CONN_TX)){
		struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)skb->data;
		const char *kind = ieee80211_is_auth(hdr->frame_control) ? "authentication" :
			ieee80211_is_assoc_req(hdr->frame_control) ? "association" : "management";

		printf("[*] %s TX status: %s\n", kind, (info->flags & IEEE80211_TX_STAT_ACK) ? "acknowledged" : "not acknowledged");
	}

	if (local != NULL){
		ieee80211_report_used_skb(local, skb, false, 0);
	}
	dev_kfree_skb_any_reason(skb, SKB_DROP_REASON_NOT_SPECIFIED);

}
