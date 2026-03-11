struct net_device *alloc_netdev_dummy(int sizeof_priv){
	return alloc_netdev_mqs(sizeof_priv, "dummy#", 0, init_dummy_netdev, 1, 1);
}
