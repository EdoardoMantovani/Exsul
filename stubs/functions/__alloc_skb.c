struct sk_buff *__alloc_skb(unsigned int size, int gfp_mask, int flags, int node){
	int ret = 0;
	unsigned char *head = NULL;
	struct sk_buff *allocated_skb = NULL;

	allocated_skb = (struct sk_buff *)malloc(SK_BUFF_STRUCT_SIZE);
	if(allocated_skb == NULL){
		ret = -ENOMEM;
		return ERR_PTR(ret);
	}

	memset(allocated_skb, 0, size);
	head = (unsigned char *)malloc(size + sizeof(struct skb_shared_info));
	if (head == NULL) {
		free(allocated_skb);
		ret = -ENOMEM;
		return ERR_PTR(ret);
	}

	allocated_skb->head = head;
	allocated_skb->data = head;
	skb_set_tail_offset(allocated_skb, 0);
	skb_set_end_offset(allocated_skb, size);

	allocated_skb->len = 0;
	allocated_skb->data_len = 0;
	allocated_skb->mac_len = 0;
	allocated_skb->hdr_len = 0;
	allocated_skb->cloned = 0;
	allocated_skb->nohdr = 0;
	allocated_skb->head_frag = 0;

	memset(skb_shinfo(allocated_skb), 0, sizeof(*skb_shinfo(allocated_skb)));
	atomic_store(&skb_shinfo(allocated_skb)->dataref, 1);
	refcount_set(&allocated_skb->users, 1);

	allocated_skb->truesize = (unsigned int)(SK_BUFF_STRUCT_SIZE + size + sizeof(struct skb_shared_info));

	return allocated_skb;
}

