bool ieee80211_prepare_and_rx_handle(struct ieee80211_rx_data *rx, struct sk_buff *skb, bool consume){
	struct sk_buff *rx_skb = NULL;

	if (rx == NULL || skb == NULL){
		return false;
	}
	rx->skb = skb;
	if (!ieee80211_accept_frame(rx)){
		return false;
	}
	if (consume == false) {
		rx->skb = skb_copy(skb, GFP_ATOMIC);
		if (rx->skb == NULL){
			return true;
		}
	}

	rx_skb = rx->skb;
	if (rx->list != NULL){
		list_add_tail(&rx_skb->list, rx->list);
	}else{
		netif_receive_skb(rx_skb);
	}
	return true;
}
