struct sk_buff *napi_build_skb(void *data, unsigned int frag_size){
	struct sk_buff *skb = NULL;

	skb = __napi_build_skb(data, frag_size);

	if (likely(skb) && frag_size) {
		skb->head_frag = 1;
	}

	return skb;
}
