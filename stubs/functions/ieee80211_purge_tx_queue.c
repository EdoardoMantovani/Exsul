void ieee80211_purge_tx_queue(struct ieee80211_hw *hw, struct sk_buff_head *queue){
	struct sk_buff *skb = NULL;

	if(queue == NULL){
		return;
	}
	while((skb = skb_dequeue(queue)) != NULL){
		ieee80211_free_txskb(hw, skb);
	}
}
