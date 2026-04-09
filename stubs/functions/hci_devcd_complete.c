int hci_devcd_complete(struct hci_dev *hdev){
	int ret = 0;
	struct sk_buff *skb = NULL;
	struct hci_devcoredump *dump = NULL;
	struct hci_devcoredump_skb_cb *skb_cb = NULL;

	skb = alloc_skb(0, GFP_ATOMIC);
	if (skb == NULL){
		ret = -ENOMEM;
		return ret;
	}

	dump = &hdev->dump;
	skb_cb = (struct hci_devcoredump_skb_cb *)skb->cb;
	skb_cb->pkt_type = HCI_DEVCOREDUMP_PKT_COMPLETE;

	skb_queue_tail(&dump->dump_q, skb);
	queue_work(hdev->workqueue, &dump->dump_rx);

	return ret;
}
