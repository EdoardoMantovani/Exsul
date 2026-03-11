void dev_mc_init(struct net_device *dev){
	__hw_addr_init(&dev->mc);
}
