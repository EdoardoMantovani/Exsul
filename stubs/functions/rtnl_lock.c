void rtnl_lock(void){
	pthread_mutex_lock(&director.rtnl_mutex);
}
