int hci_devcd_append_pattern(struct hci_dev *hdev, u8 pattern, u32 len){
	int ret = 0;
	struct sk_buff *skb = NULL;
	struct hci_devcoredump *dump = NULL;
	struct hci_devcoredump_skb_pattern p = {};
	struct hci_devcoredump_skb_cb *skb_cb = NULL;

	skb = alloc_skb(sizeof(p), GFP_ATOMIC);
	if (skb == NULL){
		ret = -ENOMEM;
		return ret;
	}
	p.pattern = pattern;
	p.len = len;

	dump = &hdev->dump;
	skb_cb = (struct hci_devcoredump_skb_cb *)skb->cb;
	skb_cb->pkt_type = HCI_DEVCOREDUMP_PKT_PATTERN;
	skb_put_data(skb, &p, sizeof(p));

	skb_queue_tail(&dump->dump_q, skb);
	queue_work(hdev->workqueue, &dump->dump_rx);

	return 0;
}
