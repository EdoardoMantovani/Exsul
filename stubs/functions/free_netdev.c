void free_netdev(struct net_device *dev){
	if(dev != NULL){
		free(dev);
	}
}
