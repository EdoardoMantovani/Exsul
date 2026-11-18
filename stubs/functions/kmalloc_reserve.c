void *kmalloc_reserve(unsigned int *size, int flags, int node, bool *pfmemalloc){
	unsigned int capacity = 0;
	unsigned int overhead = 0;
	void *allocation = NULL;

	overhead = SKB_DATA_ALIGN(director.struct_sizes.skb_shared_info);
	if(size == NULL || *size > UINT32_MAX - (SMP_CACHE_BYTES - 1)){
		return NULL;
	}
	capacity = SKB_DATA_ALIGN(*size);
	if(capacity > UINT32_MAX - overhead){
		return NULL;
	}
	allocation = __kmalloc_cache_noprof(NULL, flags, capacity + overhead);
	if(allocation != NULL){
		*size = capacity + overhead;
		if(pfmemalloc != NULL){
			*pfmemalloc = false;
		}
	}
	return allocation;
}
