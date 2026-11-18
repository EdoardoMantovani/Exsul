int hci_devcd_complete(struct hci_dev *hdev){
	int ret = 0;
	struct sk_buff *skb = NULL;
	struct hci_devcoredump_skb_cb *skb_cb = NULL;

	if (hci_devcd_enabled(hdev) == false){
		ret = -EOPNOTSUPP;
		return ret;
	}
	skb = alloc_skb(0, GFP_ATOMIC);
	if (skb == NULL){
		ret = -ENOMEM;
		return ret;
	}
	skb_cb = (struct hci_devcoredump_skb_cb *)skb->cb;
	skb_cb->pkt_type = HCI_DEVCOREDUMP_PKT_COMPLETE;
	ret = hci_devcd_queue(hdev, skb);
	return ret;
}
