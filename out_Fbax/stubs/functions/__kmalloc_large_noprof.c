void *__kmalloc_large_noprof(size_t size, int flags){
	return (void *)malloc(size);
}
