void *__kmalloc_large_noprof(size_t size, int flags){
	void *p = NULL;

	/* change strategy: use calloc instead of malloc, seems that it crashes when I invoke kmalloc in rtw89 */
	p = calloc(1, size);
	track_kmem_alloc(p);
	return p;
}
