void hci_cmd_sync_cancel(struct hci_dev *hdev, int err){
	printf("[%s] err 0x%2.2x", __FUNCTION__, err);

	if (hdev->req_status == HCI_REQ_PEND) {
		hdev->req_result = err;
		hdev->req_status = HCI_REQ_CANCELED;

		queue_work(hdev->workqueue, &hdev->cmd_sync_cancel_work);
	}
}
