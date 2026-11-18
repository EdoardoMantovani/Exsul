void *devm_kmalloc(void *dev, size_t size, int gfp){
	void *p = NULL;

	p = calloc(1, size);
	if(p != NULL && !track_kmem_alloc(p)){
		free(p);
		return NULL;
	}
	return p;
}
