struct sk_buff *skb_copy(const struct sk_buff *skb, int gfp_mask){
	unsigned int tail_off = 0;
	unsigned int headroom = 0;
	unsigned int data_capacity = 0;
	struct sk_buff *copied_skb = NULL;

	if (skb == NULL){
		return NULL;
	}

	if (!skb_frags_readable(skb)){
		return NULL;
	}

	if (skb->data_len != 0 || skb_is_nonlinear(skb)){
		return NULL;
	}

	headroom      = skb_headroom(skb);
	data_capacity = skb_end_offset(skb);
	tail_off      = skb_tail_offset(skb);

	/* always ignore the last argument of '__alloc_skb' */
	copied_skb = __alloc_skb(data_capacity, gfp_mask, skb_alloc_rx_flag(skb), 0);
	if (copied_skb == NULL){
		return NULL;
	}
	memcpy(copied_skb->head, skb->head, tail_off);
	copied_skb->data = copied_skb->head + headroom;
	skb_set_tail_offset(copied_skb, tail_off);
	copied_skb->len      = skb->len;
	copied_skb->data_len = 0;
	skb_copy_header(copied_skb, skb);
	copied_skb->cloned = 0;
	copied_skb->nohdr = 0;
	copied_skb->data = copied_skb->head + headroom;
	skb_set_tail_offset(copied_skb, tail_off);
	skb_set_end_offset(copied_skb, data_capacity);
	atomic_store(&skb_shinfo(copied_skb)->dataref, 1);

	return copied_skb;
}
