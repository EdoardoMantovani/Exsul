void hci_unregister_dev(struct hci_dev *hdev){
	int (*close)(struct hci_dev *hdev) = NULL;

	if (hdev == NULL){
		return;
	}
	clear_bit(HCI_UP, &hdev->flags);
	close = hdev->close;
	if (test_and_clear_bit(HCI_RUNNING, &hdev->flags) && close != NULL){
		close(hdev);
	}
	if (hdev->list.next != NULL && hdev->list.prev != NULL){
		list_del_init(&hdev->list);
	}
	hci_devcd_shutdown(hdev);
	if (hdev->req_workqueue != NULL){
		destroy_workqueue(hdev->req_workqueue);
		hdev->req_workqueue = NULL;
	}
	if (hdev->workqueue != NULL){
		destroy_workqueue(hdev->workqueue);
		hdev->workqueue = NULL;
	}
	skb_queue_purge(&hdev->rx_q);
	skb_queue_purge(&hdev->cmd_q);
	skb_queue_purge(&hdev->raw_q);
}
