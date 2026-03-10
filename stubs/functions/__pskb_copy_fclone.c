struct sk_buff *__pskb_copy_fclone(struct sk_buff *skb, int headroom, int gfp_mask, bool fclone){
	int i = 0;
	int flags = 0;
	unsigned int size = 0;
	struct sk_buff *n = NULL;

	size = skb_headlen(skb) + headroom;
	flags = skb_alloc_rx_flag(skb) | (fclone ? SKB_ALLOC_FCLONE : 0);
	n = __alloc_skb(size, gfp_mask, flags, 0);

	if (n == NULL){
		goto out;
	}

	/* Set the data pointer */
	skb_reserve(n, headroom);
	/* Set the tail pointer and length */
	skb_put(n, skb_headlen(skb));
	/* Copy the bytes */
	skb_copy_from_linear_data(skb, n->data, n->len);

	n->truesize += skb->data_len;
	n->data_len  = skb->data_len;
	n->len	     = skb->len;

	if (skb_shinfo(skb)->nr_frags) {
		if (skb_orphan_frags(skb, gfp_mask) || skb_zerocopy_clone(n, skb, gfp_mask)) {
			kfree_skb(n);
			n = NULL;
			goto out;
		}
		for (i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
			skb_shinfo(n)->frags[i] = skb_shinfo(skb)->frags[i];
			skb_frag_ref(skb, i);
		}
		skb_shinfo(n)->nr_frags = i;
	}

	if (skb_has_frag_list(skb)) {
		skb_shinfo(n)->frag_list = skb_shinfo(skb)->frag_list;
		skb_clone_fraglist(n);
	}

	skb_copy_header(n, skb);
out:
	return n;
}
