int hci_recv_diag(struct hci_dev *hdev, struct sk_buff *skb){
	int ret = 0;
	struct bt_skb_cb *bt_skb_cb = NULL;

	bt_skb_cb = (struct bt_skb_cb *)skb->cb;
	/* Mark as diagnostic packet */
	bt_skb_cb->pkt_type = HCI_DIAG_PKT;
	/* Time stamp */
	__net_timestamp(skb);
	skb_queue_tail(&hdev->rx_q, skb);
	queue_work(hdev->workqueue, &hdev->rx_work);
	return ret;
}
