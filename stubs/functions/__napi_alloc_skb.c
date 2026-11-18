struct sk_buff *__napi_alloc_skb(struct napi_struct *napi, unsigned int size, int gfp_mask){
	return __alloc_skb(size, gfp_mask, SKB_ALLOC_RX, NUMA_NO_NODE);
}
