struct sk_buff *alloc_skb_for_msg(struct sk_buff *first){
	struct sk_buff *n = NULL;

	n = alloc_skb(0, GFP_ATOMIC);
	if (n == NULL){
		return NULL;
	}

	n->len = first->len;
	n->data_len = first->len;
	n->truesize = first->truesize;

	skb_shinfo(n)->frag_list = first;

	__copy_skb_header(n, first);
	n->destructor = NULL;

	return n;
}
