void hci_cmd_sync_cancel(struct hci_dev *hdev, int err){
	int ret = 0;

	if(hdev != NULL && hdev->req_status == HCI_REQ_PEND){
		hdev->req_result = err;
		hdev->req_status = HCI_REQ_CANCELED;
		cancel_delayed_work(&hdev->cmd_timer);
		wake_up_interruptible(&hdev->req_wait_q);
	}
}
