struct net_device *alloc_netdev_mqs(int sizeof_priv, char *name, unsigned char name_assign_type, void (*setup)(struct net_device *), unsigned int txqs, unsigned int rxqs){
	int netdev_size = 0;
	unsigned int maxqs = 0;
	size_t napi_config_sz = 0;
	struct net_device *netdev = NULL;

	if (txqs < 1) {
		pr_err("alloc_netdev: Unable to allocate device with zero queues\n");
		return NULL;
	}

	if (rxqs < 1) {
		pr_err("alloc_netdev: Unable to allocate device with zero RX queues\n");
		return NULL;
	}

	maxqs = max(txqs, rxqs);

	if(director.struct_sizes.net_device == 0){
		netdev_size = NET_DEVICE_STRUCT_SIZE;
	}else{
		netdev_size = director.struct_sizes.net_device;
	}
	netdev = calloc(1, netdev_size);
	if (netdev == NULL){
		return NULL;
	}

	netdev->priv_len = sizeof_priv;

	refcount_set(&netdev->dev_refcnt, 1);

	if (dev_addr_init(netdev)){
		goto free_all;
	}

	dev_mc_init(netdev);
	dev_uc_init(netdev);

	INIT_LIST_HEAD(&netdev->unreg_list);
	INIT_LIST_HEAD(&netdev->close_list);
	INIT_LIST_HEAD(&netdev->ptype_all);
	INIT_LIST_HEAD(&netdev->ptype_specific);
	INIT_LIST_HEAD(&netdev->net_notifier_list);

	netdev->gso_max_size = GSO_MAX_SIZE;
	netdev->gso_max_segs = GSO_MAX_SEGS;

	netdev->priv_flags = IFF_XMIT_DST_RELEASE | IFF_XMIT_DST_RELEASE_PERM;
	setup(netdev);

	if (netdev->tx_queue_len == 0) {
		netdev->priv_flags |= IFF_NO_QUEUE;
		netdev->tx_queue_len = DEFAULT_TX_QUEUE_LEN;
	}

	netdev->num_tx_queues = txqs;
	netdev->real_num_tx_queues = txqs;
	#if 0
	if (netif_alloc_netdev_queues(netdev)){
		goto free_all;
	}

	netdev->num_rx_queues = rxqs;
	netdev->real_num_rx_queues = rxqs;

	if (netif_alloc_rx_queues(netdev)){
		goto free_all;
	}

	netdev->ethtool = malloc(sizeof(*dev->ethtool));
	if (netdev->ethtool == NULL){
		goto free_all;
	}

	netdev->cfg = malloc(sizeof(*dev->cfg));
	if (netdev->cfg == NULL){
		goto free_all;
	}
	netdev->cfg_pending = netdev->cfg;

	netdev->num_napi_configs = maxqs;
	#endif

	strncpy(netdev->name, name, IFNAMSIZ);
	netdev->name_assign_type = name_assign_type;
	netdev->group = INIT_NETDEV_GROUP;

	return netdev;

free_all:
	free(netdev);
	return NULL;
}
