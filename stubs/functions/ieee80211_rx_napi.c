void ieee80211_rx_napi(struct ieee80211_hw *hw, struct ieee80211_sta *pubsta, struct sk_buff *skb, struct napi_struct *napi){
	LIST_HEAD(list);

	/*
	 * key references and virtual interfaces are protected using RCU
	 * and this requires that we are in a read-side RCU section during
	 * receive processing
	 */
	ieee80211_rx_list(hw, pubsta, skb, &list);
	/* NAPI and non-NAPI delivery have identical userspace semantics. */
	netif_receive_skb_list(&list);
}
