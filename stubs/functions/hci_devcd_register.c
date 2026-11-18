int hci_devcd_register(struct hci_dev *hdev, void (*coredump)(struct hci_dev *hdev), void (*dmp_hdr)(struct hci_dev *hdev, struct sk_buff *skb), void (*notify_change)(struct hci_dev *hdev, int state)){
	int ret = 0;
	struct hci_devcoredump *dump = NULL;

	if (hdev == NULL || coredump == NULL || dmp_hdr == NULL){
		ret = -EINVAL;
		return ret;
	}
	dump = hci_devcd(hdev);
	dump->coredump = coredump;
	dump->dmp_hdr = dmp_hdr;
	dump->notify_change = notify_change;
	dump->timeout = DEVCOREDUMP_TIMEOUT;
	dump->supported = true;
	return ret;
}
