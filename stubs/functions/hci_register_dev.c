int hci_register_dev(struct hci_dev *hdev){
	int ret = 0;
	struct device *dev = NULL;

	if (hdev == NULL){
		ret = -EINVAL;
		return ret;
	}
	if (hdev->open == NULL || hdev->close == NULL || hdev->send == NULL){
		ret = -EINVAL;
		return ret;
	}
	if(director.hci_state_initialized == false){
		ret = hci_runtime_init(&director);
		if(ret < 0){
			return ret;
		}
	}
	dev = &hdev->dev;
	snprintf(director.hci_name, sizeof(director.hci_name), "hci0");
	hdev->name = director.hci_name;
	hdev->workqueue = alloc_workqueue("%s", WQ_HIGHPRI, hdev->name);
	if (hdev->workqueue == NULL) {
		ret = -ENOMEM;
		goto err;
	}
	hdev->req_workqueue = alloc_workqueue("%s", WQ_HIGHPRI, hdev->name);
	if (hdev->req_workqueue == NULL) {
		destroy_workqueue(hdev->workqueue);
		hdev->workqueue = NULL;
		ret = -ENOMEM;
		goto err;
	}

	set_bit(HCI_SETUP, hdev->dev_flags);
	set_bit(HCI_AUTO_OFF, hdev->dev_flags);
	set_bit(HCI_BREDR_ENABLED, hdev->dev_flags);

	pthread_mutex_lock(&director.hci_state_lock);
	director.hci_dev = hdev;
	director.hci_stopping = false;
	director.hci_setup_complete = false;
	director.hci_setup_status = -EINPROGRESS;
	list_add_tail(&hdev->list, &director.hci_dev_list);
	pthread_mutex_unlock(&director.hci_state_lock);
	if (test_bit(HCI_QUIRK_RAW_DEVICE, hdev->quirk_flags)){
		set_bit(HCI_UNCONFIGURED, hdev->dev_flags);
	}

	if (hdev->wakeup){
		hdev->conn_flags |= HCI_CONN_FLAG_REMOTE_WAKEUP;
	}
	ret = queue_work(hdev->req_workqueue, &hdev->power_on) ? 0 : -EIO;
	err:
	if(ret < 0){
		hci_publish_state(hdev, ret);
	}
	return ret;
}
