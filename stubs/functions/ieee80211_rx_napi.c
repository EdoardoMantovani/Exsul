void ieee80211_rx_napi(struct ieee80211_hw *hw, struct ieee80211_sta *pubsta, struct sk_buff *skb, struct napi_struct *napi){
	struct sk_buff *tmp = NULL;
	LIST_HEAD(list);

	/*
	 * key references and virtual interfaces are protected using RCU
	 * and this requires that we are in a read-side RCU section during
	 * receive processing
	 */
	ieee80211_rx_list(hw, pubsta, skb, &list);

	if (napi == NULL) {
		netif_receive_skb_list(&list);
		return;
	}

	//list_for_each_entry_safe(skb, tmp, &list, list) {
	//	skb_list_del_init(skb);
	//	napi_gro_receive(napi, skb);
	//}
}
