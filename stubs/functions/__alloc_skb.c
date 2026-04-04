struct sk_buff *__alloc_skb(unsigned int size, int gfp_mask, int flags, int node){
	int ret = 0;
	int sk_buff_size = 0;
	int sk_shared_info_size = 0;
	unsigned char *head = NULL;
	struct sk_buff *allocated_skb = NULL;

	sk_buff_size = director.struct_sizes.sk_buff;
	if(sk_buff_size <= 0){
                printf("[!] failed for sk_buff %d\n", sk_buff_size);
		ret = -EINVAL;
		return NULL;
	}
	sk_shared_info_size = director.struct_sizes.skb_shared_info;
	if(sk_shared_info_size <= 0){
		printf("[!] failed for skb_shared_info\n");
		ret = -EINVAL;
		return NULL;
	}
	allocated_skb = (struct sk_buff *)calloc(1, sk_buff_size);
	if(allocated_skb == NULL){
                printf("[!] failed to allocate allocated_skb\n");
		ret = -ENOMEM;
		return NULL;
	}

	head = calloc(1, size + sk_shared_info_size);
	if(head == NULL){
		free(allocated_skb);
		ret = -ENOMEM;
		return NULL;
	}

	allocated_skb->head = head;
	allocated_skb->data = head;

	skb_set_end_offset(allocated_skb, size);
	skb_set_tail_offset(allocated_skb, 0);

	allocated_skb->len = 0;
	allocated_skb->data_len = 0;
	allocated_skb->mac_len = 0;
	allocated_skb->hdr_len = 0;
	allocated_skb->cloned = 0;
	allocated_skb->nohdr = 0;
	allocated_skb->head_frag = 0;

	allocated_skb->truesize = (unsigned int)(sk_buff_size + size + sk_shared_info_size);
	refcount_set(&allocated_skb->users, 1);

	return allocated_skb;
}
