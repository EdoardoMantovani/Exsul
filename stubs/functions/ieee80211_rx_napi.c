void ieee80211_rx_napi(struct ieee80211_hw *hw, struct ieee80211_sta *pubsta, struct sk_buff *skb, struct napi_struct *napi){
	struct sk_buff *entry = NULL;
	struct list_head *pos = NULL;
	struct list_head *next = NULL;
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

	pos = list.next;
	while (pos != &list) {
		next = pos->next;
		entry = (struct sk_buff *)pos;
		skb_list_del_init(entry);
		netif_receive_skb(entry);
		pos = next;
	}
}
