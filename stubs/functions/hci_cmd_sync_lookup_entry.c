struct hci_cmd_sync_work_entry *hci_cmd_sync_lookup_entry(struct hci_dev *hdev, int (*func)(struct hci_dev *hdev, void *data), void *data, void (*destroy)(struct hci_dev *hdev, void *data, int err)){
	struct hci_cmd_sync_work_entry *entry = NULL;

	entry = _hci_cmd_sync_lookup_entry(hdev, func, data, destroy);
	return entry;
}
