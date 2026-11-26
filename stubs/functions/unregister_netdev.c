void unregister_netdev(void *dev){
	struct net_device *netdev = NULL;
	struct net_device_ops *ops = NULL;
	void (*ndo_uninit)(struct net_device *dev) = NULL;

	netdev = dev;
	if(netdev == NULL){
		return;
	}
	call_netdevice_notifiers(NETDEV_GOING_DOWN, netdev);
	call_netdevice_notifiers(NETDEV_DOWN, netdev);
	netdev->reg_state = NETREG_UNREGISTERING;
	call_netdevice_notifiers(NETDEV_UNREGISTER, netdev);
	ops = netdev->netdev_ops;
	if(ops != NULL){
		ndo_uninit = ops->ndo_uninit;
	}
	if(ndo_uninit != NULL){
		ndo_uninit(netdev);
	}
	netdev->reg_state = NETREG_UNREGISTERED;
	if(director.net_device == dev){
		director.net_device = NULL;
	}
}
