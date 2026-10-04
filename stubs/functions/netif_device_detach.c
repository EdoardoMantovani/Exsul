void netif_device_detach(struct net_device *netdev){
	if (test_and_clear_bit(__LINK_STATE_PRESENT, &netdev->state) && netif_running(netdev)) {
		netif_tx_stop_all_queues(netdev);
	}
}
