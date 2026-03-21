struct hci_dev *hci_alloc_dev_priv(int sizeof_priv){
	struct hci_dev *hdev = NULL;
	unsigned int alloc_size = 0;

	alloc_size = get_struct_size("hci_dev");
	if (sizeof_priv) {
		/* Fixme: May need ALIGN-ment? */
		alloc_size += sizeof_priv;
	}

	hdev = malloc(alloc_size);
	if (hdev == NULL){
		return NULL;
	}

	hdev->pkt_type  = (HCI_DM1 | HCI_DH1 | HCI_HV1);
	hdev->esco_type = (ESCO_HV1);
	hdev->link_mode = (HCI_LM_ACCEPT);
	hdev->num_iac = 0x01;		/* One IAC support is mandatory */
	hdev->io_capability = 0x03;	/* No Input No Output */
	hdev->manufacturer = 0xffff;	/* Default to internal use */
	hdev->inq_tx_power = HCI_TX_POWER_INVALID;
	hdev->adv_tx_power = HCI_TX_POWER_INVALID;
	hdev->adv_instance_cnt = 0;
	hdev->cur_adv_instance = 0x00;
	hdev->adv_instance_timeout = 0;

	hdev->advmon_allowlist_duration = 300;
	hdev->advmon_no_filter_duration = 500;
	hdev->enable_advmon_interleave_scan = 0x00;	/* Default to disable */

	hdev->sniff_max_interval = 800;
	hdev->sniff_min_interval = 80;

	hdev->le_adv_channel_map = 0x07;
	hdev->le_adv_min_interval = 0x0800;
	hdev->le_adv_max_interval = 0x0800;
	hdev->le_scan_interval = DISCOV_LE_SCAN_INT_FAST;
	hdev->le_scan_window = DISCOV_LE_SCAN_WIN_FAST;
	hdev->le_scan_int_suspend = DISCOV_LE_SCAN_INT_SLOW1;
	hdev->le_scan_window_suspend = DISCOV_LE_SCAN_WIN_SLOW1;
	hdev->le_scan_int_discovery = DISCOV_LE_SCAN_INT;
	hdev->le_scan_window_discovery = DISCOV_LE_SCAN_WIN;
	hdev->le_scan_int_adv_monitor = DISCOV_LE_SCAN_INT_FAST;
	hdev->le_scan_window_adv_monitor = DISCOV_LE_SCAN_WIN_FAST;
	hdev->le_scan_int_connect = DISCOV_LE_SCAN_INT_CONN;
	hdev->le_scan_window_connect = DISCOV_LE_SCAN_WIN_CONN;
	hdev->le_conn_min_interval = 0x0018;
	hdev->le_conn_max_interval = 0x0028;
	hdev->le_conn_latency = 0x0000;
	hdev->le_supv_timeout = 0x002a;
	hdev->le_def_tx_len = 0x001b;
	hdev->le_def_tx_time = 0x0148;
	hdev->le_max_tx_len = 0x001b;
	hdev->le_max_tx_time = 0x0148;
	hdev->le_max_rx_len = 0x001b;
	hdev->le_max_rx_time = 0x0148;
	hdev->le_max_key_size = SMP_MAX_ENC_KEY_SIZE;
	hdev->le_min_key_size = SMP_MIN_ENC_KEY_SIZE;
	hdev->le_tx_def_phys = HCI_LE_SET_PHY_1M;
	hdev->le_rx_def_phys = HCI_LE_SET_PHY_1M;
	hdev->le_num_of_adv_sets = HCI_MAX_ADV_INSTANCES;
	hdev->def_multi_adv_rotation_duration = HCI_DEFAULT_ADV_DURATION;
	hdev->def_le_autoconnect_timeout = HCI_LE_CONN_TIMEOUT;
	hdev->min_le_tx_power = HCI_TX_POWER_INVALID;
	hdev->max_le_tx_power = HCI_TX_POWER_INVALID;

	hdev->rpa_timeout = HCI_DEFAULT_RPA_TIMEOUT;
	hdev->discov_interleaved_timeout = DISCOV_INTERLEAVED_TIMEOUT;
	hdev->conn_info_min_age = DEFAULT_CONN_INFO_MIN_AGE;
	hdev->conn_info_max_age = DEFAULT_CONN_INFO_MAX_AGE;
	hdev->auth_payload_timeout = DEFAULT_AUTH_PAYLOAD_TIMEOUT;
	hdev->min_enc_key_size = HCI_MIN_ENC_KEY_SIZE;

	/* default 1.28 sec page scan */
	hdev->def_page_scan_type = PAGE_SCAN_TYPE_STANDARD;
	hdev->def_page_scan_int = 0x0800;
	hdev->def_page_scan_window = 0x0012;

	mutex_init(&hdev->lock);
	mutex_init(&hdev->req_lock);
	mutex_init(&hdev->mgmt_pending_lock);

	INIT_LIST_HEAD(&hdev->mesh_pending);
	INIT_LIST_HEAD(&hdev->mgmt_pending);
	INIT_LIST_HEAD(&hdev->reject_list);
	INIT_LIST_HEAD(&hdev->accept_list);
	INIT_LIST_HEAD(&hdev->uuids);
	INIT_LIST_HEAD(&hdev->link_keys);
	INIT_LIST_HEAD(&hdev->long_term_keys);
	INIT_LIST_HEAD(&hdev->identity_resolving_keys);
	INIT_LIST_HEAD(&hdev->remote_oob_data);
	INIT_LIST_HEAD(&hdev->le_accept_list);
	INIT_LIST_HEAD(&hdev->le_resolv_list);
	INIT_LIST_HEAD(&hdev->le_conn_params);
	INIT_LIST_HEAD(&hdev->pend_le_conns);
	INIT_LIST_HEAD(&hdev->pend_le_reports);
	INIT_LIST_HEAD(&hdev->conn_hash.list);
	INIT_LIST_HEAD(&hdev->adv_instances);
	INIT_LIST_HEAD(&hdev->blocked_keys);
	INIT_LIST_HEAD(&hdev->monitored_devices);

	INIT_LIST_HEAD(&hdev->local_codecs);
	/* still TODO */
	#if 0
	INIT_WORK(&hdev->rx_work, hci_rx_work);
	INIT_WORK(&hdev->cmd_work, hci_cmd_work);
	INIT_WORK(&hdev->tx_work, hci_tx_work);
	INIT_WORK(&hdev->power_on, hci_power_on);
	INIT_WORK(&hdev->error_reset, hci_error_reset);
	#endif
	hci_cmd_sync_init(hdev);

	#if 0
	INIT_DELAYED_WORK(&hdev->power_off, hci_power_off);
	#endif

	skb_queue_head_init(&hdev->rx_q);
	skb_queue_head_init(&hdev->cmd_q);
	skb_queue_head_init(&hdev->raw_q);

	init_waitqueue_head(&hdev->req_wait_q);
	#if 0
	INIT_DELAYED_WORK(&hdev->cmd_timer, hci_cmd_timeout);
	INIT_DELAYED_WORK(&hdev->ncmd_timer, hci_ncmd_timeout);
	#endif
	discovery_init(hdev);

	return hdev;
}
