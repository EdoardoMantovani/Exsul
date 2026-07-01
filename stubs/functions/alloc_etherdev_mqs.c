struct net_device *alloc_etherdev_mqs(int sizeof_priv, unsigned int txqs, unsigned int rxqs){
	return alloc_netdev_mqs(sizeof_priv, "wlan%d", 0, NULL, txqs, rxqs);
}
