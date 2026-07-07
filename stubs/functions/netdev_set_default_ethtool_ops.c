void netdev_set_default_ethtool_ops(void *dev, const void *ops){
	struct net_device *netdev = NULL;

	netdev = (struct net_device *)dev;

	if(netdev == NULL){
		return;
	}
	netdev->ethtool_ops = ops;
}
