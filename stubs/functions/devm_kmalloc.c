void *devm_kmalloc(void *dev, size_t size, int gfp){
	void *p = NULL;

	p = calloc(1, size);
	track_kmem_alloc(p);
	return p;
}
