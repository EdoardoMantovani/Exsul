void hci_free_dev(struct hci_dev *hdev){
	int ret = 0;

	if(hdev == NULL){
		return;
	}
	if(!test_bit(HCI_UNREGISTER, hdev->dev_flags)){
		hci_unregister_dev(hdev);
	}else if(hdev->workqueue != NULL || hdev->req_workqueue != NULL){
		hci_devcd_shutdown(hdev);
		skb_queue_purge(&hdev->rx_q);
		skb_queue_purge(&hdev->cmd_q);
		skb_queue_purge(&hdev->raw_q);
	}
	kfree_const(hdev->hw_info);
	hdev->hw_info = NULL;
	kfree_const(hdev->fw_info);
	hdev->fw_info = NULL;
	free(hdev);
}
