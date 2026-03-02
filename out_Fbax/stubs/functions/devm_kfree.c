void devm_kfree(void *dev, void *p){
	if(p != NULL){
		for(u8 j = 0; j < MAX_KMEM_ARRAY_SIZE; j++){
			if(director.kmem_array[j] == p){
				director.kmem_array[j] = NULL;
				free(p);
			}
		}
	}
}
