void netif_tx_stop_all_queues(struct net_device *netdev){
	struct netdev_queue *txq = NULL;

	for (int i = 0; i < netdev->num_tx_queues; i++) {
		txq = netdev_get_tx_queue(netdev, i);
		netif_tx_stop_queue(txq);
	}
}
