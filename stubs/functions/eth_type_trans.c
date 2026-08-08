unsigned short eth_type_trans(void *skb_ptr, void *dev){
	int ret = 0;
	struct sk_buff *skb = skb_ptr;
	struct ethhdr *eth = NULL;

	if(skb == NULL || skb->data == NULL || skb->len < sizeof(struct ethhdr)){
		return ret;
	}
	skb->dev = dev;
	eth = (struct ethhdr *)skb->data;
	skb->protocol = eth->h_proto;
	return skb->protocol;
}

