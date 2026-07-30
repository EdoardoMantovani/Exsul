void kfree(void *addr){
	if(addr != NULL){
		untrack_kmem_alloc(addr);
		free(addr);
	}
}
