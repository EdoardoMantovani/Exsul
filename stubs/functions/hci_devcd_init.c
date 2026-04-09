int hci_devcd_init(struct hci_dev *hdev, u32 dump_size){
	int ret = 0;
	struct sk_buff *skb = NULL;
	struct hci_devcoredump *dump = NULL;
	struct hci_devcoredump_skb_cb *skb_cb = NULL;

	skb = alloc_skb(sizeof(dump_size), GFP_ATOMIC);
	if (skb == NULL){
		ret = -ENOMEM;
		return ret;
	}

	dump = &hdev->dump;
	skb_cb = (struct hci_devcoredump_skb_cb *)skb->cb;
	skb_cb->pkt_type = HCI_DEVCOREDUMP_PKT_INIT;
	put_unaligned_le32(dump_size, skb_put(skb, 4));

	skb_queue_tail(&dump->dump_q, skb);
	queue_work(hdev->workqueue, &dump->dump_rx);

	return ret;
}
