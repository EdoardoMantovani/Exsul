int hci_cmd_sync_queue_once(struct hci_dev *hdev, int (*func)(struct hci_dev *hdev, void *data), void *data, void (*destroy)(struct hci_dev *hdev, void *data, int err)){
	int ret = 0;

	if (hci_cmd_sync_lookup_entry(hdev, func, data, destroy)){
		ret = -EEXIST;
		return ret;
	}
	ret = hci_cmd_sync_queue(hdev, func, data, destroy);
	return ret;
}
