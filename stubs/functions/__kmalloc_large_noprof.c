void *__kmalloc_large_noprof(size_t size, int flags){
	void *p = NULL;

	p = calloc(1, size);
	return p;
}
