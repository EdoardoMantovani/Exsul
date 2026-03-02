void kfree_const(const void *x){
	if(x != NULL){
		free(x);
	}
}
