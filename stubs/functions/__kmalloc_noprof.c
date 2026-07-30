void *__kmalloc_noprof(size_t size, int flags){
	void *p = NULL;

	p = calloc(1, size);
	track_kmem_alloc(p);
	return p;
}
