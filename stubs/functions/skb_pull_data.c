void *skb_pull_data(struct sk_buff *skb, size_t len){
	void *data = NULL;
	data = skb->data;

	if (skb->len < len){
		return NULL;
	}

	skb_pull(skb, len);
	return data;
}
