void __mutex_init(struct mutex *lock, const char *name, void *key){
	if(lock != NULL){
		mutex_init_generic(lock);
	}
}
