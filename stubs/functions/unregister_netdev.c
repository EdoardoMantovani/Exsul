void unregister_netdev(void *dev){
	if(director.net_device == dev){
		director.net_device = NULL;
	}
}
