void __skb_trim(struct sk_buff *skb, unsigned int len){
	__skb_set_length(skb, len);
}

