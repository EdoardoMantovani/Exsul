void *vzalloc_noprof(unsigned long size){
	return calloc(1, size);
}
