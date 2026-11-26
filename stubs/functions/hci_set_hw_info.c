void hci_set_hw_info(struct hci_dev *hdev, const char *fmt, ...){
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
	kfree_const(hdev->hw_info);
	hdev->hw_info = info;
}
