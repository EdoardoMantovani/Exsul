void hci_unregister_dev(struct hci_dev *hdev){
	int ret = 0;
	int (*close)(struct hci_dev *hdev) = NULL;
	int (*shutdown)(struct hci_dev *hdev) = NULL;

	if(hdev == NULL){
		return;
	}
	if(test_and_set_bit(HCI_UNREGISTER, hdev->dev_flags)){
		return;
	}
	if(director.hci_state_initialized == true){
		pthread_mutex_lock(&director.hci_state_lock);
		director.hci_stopping = true;
		director.hci_setup_complete = true;
		director.hci_setup_status = -ENODEV;
		pthread_cond_broadcast(&director.hci_state_cond);
		pthread_mutex_unlock(&director.hci_state_lock);
	}
	if(director.hci_raw_initialized == true){
		pthread_mutex_lock(&director.hci_raw_lock);
		pthread_cond_broadcast(&director.hci_raw_cond);
		pthread_mutex_unlock(&director.hci_raw_lock);
	}
	hci_cmd_sync_cancel(hdev, ENODEV);
	cancel_delayed_work_sync(&hdev->cmd_timer);
	cancel_delayed_work_sync(&hdev->ncmd_timer);
	cancel_delayed_work_sync(&hdev->power_off);
	cancel_work_sync(&hdev->power_on);
	cancel_work_sync(&hdev->error_reset);
	cancel_work_sync(&hdev->cmd_work);
	cancel_work_sync(&hdev->tx_work);
	cancel_work_sync(&hdev->rx_work);

	shutdown = hdev->shutdown;
	close = hdev->close;
	if(test_bit(HCI_RUNNING, &hdev->flags) && shutdown != NULL){
		ret = shutdown(hdev);
	}
	clear_bit(HCI_UP, &hdev->flags);
	if(test_and_clear_bit(HCI_RUNNING, &hdev->flags) && close != NULL){
		ret = close(hdev);
	}
	clear_bit(HCI_INIT, &hdev->flags);
	if(hdev->list.next != NULL && hdev->list.prev != NULL && hdev->list.next != &hdev->list){
		list_del_init(&hdev->list);
	}
	hci_devcd_shutdown(hdev);
	skb_queue_purge(&hdev->rx_q);
	skb_queue_purge(&hdev->cmd_q);
	skb_queue_purge(&hdev->raw_q);
	kfree_skb(hdev->sent_cmd);
	hdev->sent_cmd = NULL;
	kfree_skb(hdev->req_skb);
	hdev->req_skb = NULL;
	kfree_skb(hdev->req_rsp);
	hdev->req_rsp = NULL;

	if(hdev->req_workqueue != NULL){
		destroy_workqueue(hdev->req_workqueue);
		hdev->req_workqueue = NULL;
	}
	if(hdev->workqueue != NULL){
		destroy_workqueue(hdev->workqueue);
		hdev->workqueue = NULL;
	}
	if(director.hci_state_initialized == true){
		pthread_mutex_lock(&director.hci_state_lock);
		if(director.hci_dev == hdev){
			director.hci_dev = NULL;
		}
		pthread_cond_broadcast(&director.hci_state_cond);
		pthread_mutex_unlock(&director.hci_state_lock);
	}
}
