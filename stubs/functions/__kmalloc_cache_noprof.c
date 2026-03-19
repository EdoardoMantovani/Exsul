void *__kmalloc_cache_noprof(void *s, unsigned int gfpflags, size_t size){
	void *p = NULL;

	p = malloc(size);

	return p;
}
