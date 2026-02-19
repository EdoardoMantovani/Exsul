void vfree(void *addr){
	if(addr != NULL){
		free(addr);
	}
}
