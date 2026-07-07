void netif_carrier_off(void *dev){
	struct net_device *netdev = NULL;

	netdev = (struct net_device *)dev;
	if(netdev == NULL){
		return;
	}
	set_bit(__LINK_STATE_NOCARRIER, &netdev->state);
	call_netdevice_notifiers(NETDEV_CHANGE, netdev);
}
