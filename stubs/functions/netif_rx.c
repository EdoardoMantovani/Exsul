int netif_rx(struct sk_buff *skb){
	int ret = 0;

	consume_skb(skb);
	return ret;
}
