struct sk_buff *skb_copy(const struct sk_buff *skb, int gfp_mask){
	int headerlen = 0;
	unsigned int size = 0;
	struct sk_buff *n = NULL;

	if (!skb_frags_readable(skb)){
		return NULL;
	}

	//if (skb_shinfo(skb)->gso_type & SKB_GSO_FRAGLIST)){
	//	return NULL;
	//}

	headerlen = skb_headroom(skb);
	size = skb_end_offset(skb) + ((char *)skb + director.runtime_generated_structs->structs[2].fields[8].field_offset);
	n = __alloc_skb(size, gfp_mask, skb_alloc_rx_flag(skb), NUMA_NO_NODE);
	if (!n){
		return NULL;
	}

	/* Set the data pointer */
	skb_reserve(n, headerlen);
	/* Set the tail pointer and length */
	skb_put(n, ((char *)skb + director.runtime_generated_structs->structs[2].fields[0].field_offset));

	//(skb_copy_bits(skb, -headerlen, n->head, headerlen + ((char *)skb + director.runtime_generated_structs->structs[2].fields[0].field_offset)));

	skb_copy_header(n, skb);
	return n;
}
