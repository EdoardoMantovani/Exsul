void ieee80211_rx_irqsafe(struct ieee80211_hw *hw, struct sk_buff *skb){
	struct ieee80211_local *local = NULL;

	local = director.ieee80211_local;
	skb->pkt_type = IEEE80211_RX_MSG;
	skb_queue_tail(&local->skb_queue, skb);
	tasklet_schedule(&local->tasklet);
}
