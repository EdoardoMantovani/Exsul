struct sk_buff *__netdev_alloc_skb(struct net_device *netdev, unsigned int len, int gfp_mask){
	struct sk_buff *skb = NULL;

	len += NET_SKB_PAD;

	skb = __alloc_skb(len, gfp_mask, SKB_ALLOC_RX, 0);
	if (skb == NULL){
		goto skb_fail;
	}

skb_success:
	skb_reserve(skb, NET_SKB_PAD);
	skb->dev = netdev;

skb_fail:
	return skb;
}

