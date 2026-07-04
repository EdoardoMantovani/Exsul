int register_netdevice(struct net_device *dev){
	int ret = 0;

	director.net_device = dev;
	return ret;
}
