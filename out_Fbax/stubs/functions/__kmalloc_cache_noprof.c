void *__kmalloc_cache_noprof(void *s, int gfpflags, size_t size){
	return (void *)malloc(size);
}
