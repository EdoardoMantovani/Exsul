void *__kmalloc_noprof(size_t size, int flags){
	void *p = NULL;

	p = calloc(1, size);
	return p;
}
