int netif_receive_skb(struct sk_buff *skb){
	int ret = 0;

	ret = netif_receive_skb_internal(skb);
	return ret;
}
