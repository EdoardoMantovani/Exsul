void kfree(void *addr){
	if(addr != NULL){
		free(addr);
	}
}
