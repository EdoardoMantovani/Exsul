void *kmemdup_noprof(const void *src, size_t len, int gfp){
	void *p = NULL;
	p = malloc(len);

	if(p != NULL){
		memcpy(p, src, len);
	}
}
