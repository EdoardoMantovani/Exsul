int ___pskb_trim(struct sk_buff *skb, unsigned int len){
	int ret = 0;
	int i   = 0;
	int frag_sz = 0;
	int frag_end = 0;
	unsigned int headlen = 0;
	struct skb_shared_info *shinfo = NULL;

	headlen = skb->len - skb->data_len;
	shinfo  = skb_shinfo(skb);

	if (len <= headlen) {
		for (i = 0; i < shinfo->nr_frags; i++) {
			skb_frag_unref(skb, i);
		}
		shinfo->nr_frags = 0;
		skb->data_len    = 0;
		skb->len         = len;
		skb->tail        = skb->data + len;
		return ret;
	}

	for (i = 0; i < shinfo->nr_frags; i++) {
		frag_sz  = skb_frag_size(&shinfo->frags[i]);
		frag_end = (int)headlen + frag_sz;
		if (len <= frag_end) {
			skb_frag_size_set(&shinfo->frags[i], len - headlen);
			i++;
			break;
		}
		headlen = frag_end;
	}
	for (; i < shinfo->nr_frags; i++) {
		skb_frag_unref(skb, i);
	}
	shinfo->nr_frags = i;
	skb->data_len   -= skb->len - len;
	skb->len         = len;
	return ret;
}
