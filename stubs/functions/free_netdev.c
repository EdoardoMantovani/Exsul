void free_netdev(struct net_device *dev){
	if(dev != NULL){
		if(director.net_device == dev){
			director.net_device = NULL;
		}
		free(dev);
	}
}
