int hci_cmd_sync_queue(struct hci_dev *hdev, int (*func)(struct hci_dev *hdev, void *data), void *data, void (*destroy)(struct hci_dev *hdev, void *data, int err)){
	int ret = 0;
	/* Only queue command if hdev is running which means it had been opened
	 * and is either on init phase or is already up.
	 */
	if (!test_bit(HCI_RUNNING, &hdev->flags)){
		ret = -ENETDOWN;
		return ret;
	}
	ret = hci_cmd_sync_submit(hdev, func, data, destroy);
	return ret;
}
