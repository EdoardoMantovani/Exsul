void devm_kfree(void *dev, void *p){
	if(p != NULL){
		free(p);
	}
}
