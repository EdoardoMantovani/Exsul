void rtnl_unlock(void){
	pthread_mutex_unlock(&director.rtnl_mutex);
}
