int hci_register_dev(struct hci_dev *hdev){
	int ret = 0;
	int error = 0;
	struct device *dev = NULL;

	if (hdev->open == NULL || hdev->close == NULL || hdev->send == NULL){
		ret = -EINVAL;
		return ret;
	}

	dev = &hdev->dev;
	/* inline here 'dev_name' for simplicity */

	if (dev->init_name != NULL){
		hdev->name = dev->init_name;
        }else{
		hdev->name = kobject_name(&dev->kobj);
	}

	hdev->workqueue = alloc_workqueue("%s", WQ_HIGHPRI, hdev->name);
	if (hdev->workqueue == NULL) {
		error = -ENOMEM;
		goto err;
	}

	hdev->req_workqueue = alloc_workqueue("%s", WQ_HIGHPRI, hdev->name);
	if (hdev->req_workqueue == NULL) {
		destroy_workqueue(hdev->workqueue);
		error = -ENOMEM;
		goto err;
	}

	set_bit(HCI_SETUP, hdev->dev_flags);
	set_bit(HCI_AUTO_OFF, hdev->dev_flags);
	set_bit(HCI_BREDR_ENABLED, hdev->dev_flags);

	list_add(&hdev->list, &director.hci_dev_list);

	if (test_bit(HCI_QUIRK_RAW_DEVICE, hdev->quirk_flags)){
		set_bit(HCI_UNCONFIGURED, hdev->dev_flags);
	}

	if (hdev->wakeup){
		hdev->conn_flags |= HCI_CONN_FLAG_REMOTE_WAKEUP;
	}

	queue_work(hdev->req_workqueue, &hdev->power_on);

	err:
	return ret;
}
