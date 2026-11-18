void hci_free_dev(struct hci_dev *hdev){
	if (hdev == NULL){
		return;
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
	free(hdev);
}
