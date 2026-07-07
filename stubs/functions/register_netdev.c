int register_netdev(void *dev){
	int ret = 0;

	ret = register_netdevice(dev);
	return ret;
}
