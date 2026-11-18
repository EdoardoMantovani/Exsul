int hci_cmd_sync_submit(struct hci_dev *hdev, int(*func)(struct hci_dev *hdev, void *data), void *data, void (*destroy)(struct hci_dev *hdev, void *data, int err)){
	int ret = 0;
	struct hci_cmd_sync_work_entry *entry = NULL;

	entry = calloc(1, sizeof(struct hci_cmd_sync_work_entry));
	if (entry == NULL) {
		ret = -ENOMEM;
		goto unlock;
	}
	entry->func = func;
	entry->data = data;
	entry->destroy = destroy;

	list_add_tail(&entry->list, &hdev->cmd_sync_work_list);
	queue_work(hdev->req_workqueue, &hdev->cmd_sync_work);
	unlock:
	return ret;
}
