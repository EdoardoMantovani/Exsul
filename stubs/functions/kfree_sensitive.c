void kfree_sensitive(const void *p){
	if(p != NULL){
		kfree(p);
	}
}
