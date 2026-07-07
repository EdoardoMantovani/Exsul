void netif_carrier_on(void *dev){
	struct net_device *netdev = NULL;

	netdev = (struct net_device *)dev;

	if(netdev == NULL){
		goto end;
	}
	clear_bit(__LINK_STATE_NOCARRIER, &netdev->state);
	call_netdevice_notifiers(NETDEV_CHANGE, netdev);
	end:
}
