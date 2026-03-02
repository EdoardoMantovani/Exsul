void __mutex_init(pthread_mutex_t *lock, const char *name, void *key){
	pthread_mutex_init(lock, NULL);
}
