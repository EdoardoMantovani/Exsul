int skb_linearize(struct sk_buff *skb){
	int ret = 0;
	if (skb->data_len == 0){
		return ret;
	}
	ret = __pskb_pull_tail(skb, skb->data_len) ? 0 : -ENOMEM;
	return ret;
}
