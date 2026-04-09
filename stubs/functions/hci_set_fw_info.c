void hci_set_fw_info(struct hci_dev *hdev, const char *fmt, ...){
	va_list vargs;

	va_start(vargs, fmt);
	kfree_const(hdev->fw_info);
	vasprintf(hdev->fw_info, fmt, vargs);
	va_end(vargs);
}
