struct sk_buff *__alloc_skb(unsigned int size, int gfp_mask, int flags, int node){
	int skb_size = 0;
	int shared_size = 0;
	struct sk_buff *skb = NULL;

	skb_size = director.struct_sizes.sk_buff;
	shared_size = director.struct_sizes.skb_shared_info;
	if (skb_size == 0 || shared_size == 0 || size > UINT32_MAX - (SMP_CACHE_BYTES - 1)){
		return NULL;
	}
	size = SKB_DATA_ALIGN(size);
	if (size + skb_size + shared_size > UINT32_MAX){
		return NULL;
	}
	skb = calloc(1, skb_size);
	if (skb == NULL){
		return NULL;
	}
	skb->head = malloc(size + shared_size);
	if (skb->head == NULL){
		free(skb);
		return NULL;
	}
	skb->data = skb->head;
	skb_set_end_offset(skb, size);
	skb_set_tail_offset(skb, 0);
	skb->truesize = skb_size + size + shared_size;
	refcount_set(&skb->users, 1);
	memset(skb_shinfo(skb), 0, shared_size);
	atomic_init(&skb_shinfo(skb)->dataref, 1);
	return skb;
}
