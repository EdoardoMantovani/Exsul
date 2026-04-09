void hci_set_hw_info(struct hci_dev *hdev, const char *fmt, ...){
	va_list vargs;

	va_start(vargs, fmt);
	kfree_const(hdev->hw_info);
	vasprintf(hdev->hw_info, fmt, vargs);
	va_end(vargs);
}
