void *devm_kmemdup(void *dev, const void *src, size_t len, int gfp){
	void *p = NULL;

	p = devm_kmalloc(dev, len, gfp);
	if (p != NULL){
		memcpy(p, src, len);
	}
	return p;
}
