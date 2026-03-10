struct sk_buff *skb_clone(struct sk_buff *skb, int gfp_mask){
	struct sk_buff *n = NULL;

	if (skb_orphan_frags(skb, gfp_mask)){
		return NULL;
	}

	if (skb->fclone == SKB_FCLONE_ORIG) {
		n = __kmalloc_large_noprof(SK_BUFF_STRUCT_SIZE, gfp_mask);
		if (n == NULL){
			return NULL;
		}
		n->fclone = SKB_FCLONE_CLONE;
	} else {
		n = __kmalloc_large_noprof(SK_BUFF_STRUCT_SIZE, gfp_mask);
		if (n == NULL){
			return NULL;
		}
		n->fclone = SKB_FCLONE_UNAVAILABLE;
	}

	return __skb_clone(n, skb);
}
