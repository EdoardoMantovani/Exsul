int pskb_expand_head(struct sk_buff *skb, int nhead, int ntail, int gfp_mask){
	int ret = 0;
	long off = 0;
	unsigned int osize = 0;
	unsigned int size = 0;
	u8 *data = NULL;

	osize = skb_end_offset(skb);
	if(nhead < 0 || (int64_t)osize + nhead + ntail > UINT32_MAX || (int64_t)osize + nhead + ntail < skb_tail_pointer(skb) - skb->head + nhead){
		ret = -EINVAL;
		return ret;
	}
	size = osize + nhead + ntail;
	if(skb_shared(skb)){
		ret = -ENOMEM;
		return ret;
	}
	skb_zcopy_downgrade_managed(skb);
	data = kmalloc_reserve(&size, gfp_mask, NUMA_NO_NODE, NULL);
	if (!data){
		goto nodata;
	}
	size = SKB_WITH_OVERHEAD(size);

	/* Copy only real data... and, alas, header. This should be
	 * optimized for the cases when header is void.
	 */
	memcpy(data + nhead, skb->head, skb_tail_pointer(skb) - skb->head);
	memcpy((struct skb_shared_info *)(data + size), skb_shinfo(skb), offsetof(struct skb_shared_info, frags[skb_shinfo(skb)->nr_frags]));

	/*
	 * if shinfo is shared we must drop the old head gracefully, but if it
	 * is not we can just drop the old head and let the existing refcount
	 * be since all we did is relocate the values
	 */
	off = (data + nhead) - skb->head;
	if (skb_cloned(skb)) {
		if (skb_orphan_frags(skb, gfp_mask)){
			goto nofrags;
		}
		if (skb_zcopy(skb)){
			refcount_inc(&skb_uarg(skb)->refcnt);
		}
		for (int i = 0; i < skb_shinfo(skb)->nr_frags; i++){
			skb_frag_ref(skb, i);
		}

		if (skb_has_frag_list(skb)){
			skb_clone_fraglist(skb);
		}
		if(atomic_fetch_sub_explicit(&skb_shinfo(skb)->dataref, 1, memory_order_acq_rel) == 1){
			skb_free_head(skb);
		}
	} else {
		skb_free_head(skb);
	}

	skb->head     = data;
	skb->head_frag = 0;
	skb->data    += off;

	skb_set_end_offset(skb, size);
	#ifdef NET_SKBUFF_DATA_USES_OFFSET
	off           = nhead;
	#endif
	skb->tail	      += off;
	skb_headers_offset_update(skb, nhead);
	skb->cloned   = 0;
	skb->hdr_len  = 0;
	skb->nohdr    = 0;
	atomic_store(&skb_shinfo(skb)->dataref, 1);

	if (skb->sk == NULL || skb->destructor == sock_edemux){
		skb->truesize += size - osize;
	}
	return ret;
	nofrags:
	if(data != NULL){
		kfree(data);
	}
	nodata:
	ret = -ENOMEM;
	return ret;
}
