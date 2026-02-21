void *devm_kmalloc(void *dev, size_t size, int gfp){
	void *p = NULL;
	p = malloc(size);

	if(p != NULL){
		for(u8 j = 0; j < MAX_KMEM_ARRAY_SIZE; j++){
			if(director.kmem_array[j] != NULL){
				director.kmem_array[j] = p;
			}
		}
	}
	return p;
}
