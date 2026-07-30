void *__kmalloc_cache_noprof(void *s, unsigned int gfpflags, size_t size){
	void *p = NULL;

	p = calloc(1, size);
	track_kmem_alloc(p);
	return p;
}
