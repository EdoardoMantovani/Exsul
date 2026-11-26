void hci_set_fw_info(struct hci_dev *hdev, const char *fmt, ...){
	int ret = 0;
	char *info = NULL;
	va_list vargs;

	if(hdev == NULL || fmt == NULL){
		return;
	}
	va_start(vargs, fmt);
	ret = vasprintf(&info, fmt, vargs);
	va_end(vargs);
	if(ret < 0){
		return;
	}
	kfree_const(hdev->fw_info);
	hdev->fw_info = info;
}
