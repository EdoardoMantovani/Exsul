int hci_devcd_append(struct hci_dev *hdev, struct sk_buff *skb){
	int ret = 0;
	struct hci_devcoredump_skb_cb *skb_cb = NULL;

	if (skb == NULL){
		ret = -ENOMEM;
		return ret;
	}
	if (hci_devcd_enabled(hdev) == false){
		kfree_skb(skb);
		ret = -EOPNOTSUPP;
		return ret;
	}
	skb_cb = (struct hci_devcoredump_skb_cb *)skb->cb;
	skb_cb->pkt_type = HCI_DEVCOREDUMP_PKT_SKB;
	ret = hci_devcd_queue(hdev, skb);
	return ret;
}
