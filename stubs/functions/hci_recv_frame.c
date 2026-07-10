int hci_recv_frame(struct hci_dev *hdev, struct sk_buff *skb){
	int ret = 0;
	u8 dev_pkt_type = 0;
	struct bt_skb_cb *bt_cb = NULL;

	bt_cb = (struct bt_skb_cb *)skb->cb;

	if (hdev == NULL || (!test_bit(HCI_UP, &hdev->flags) && !test_bit(HCI_INIT, &hdev->flags))) {
		kfree_skb(skb);
		ret = -ENXIO;
		return ret;
	}

	/* Check if the driver agree with packet type classification */
	dev_pkt_type = hci_dev_classify_pkt_type(hdev, skb);
	if (bt_cb->pkt_type != dev_pkt_type) {
		bt_cb->pkt_type = dev_pkt_type;
	}

	switch (bt_cb->pkt_type) {
	case HCI_EVENT_PKT:
		break;
	case HCI_ACLDATA_PKT:
		/* Detect if ISO packet has been sent as ACL */
		if (hci_conn_num(hdev, CIS_LINK) || hci_conn_num(hdev, BIS_LINK) || hci_conn_num(hdev, PA_LINK)) {
			__u16 handle = __le16_to_cpu(hci_acl_hdr(skb)->handle);
			__u8 type;

			type = hci_conn_lookup_type(hdev, hci_handle(handle));
			if (type == CIS_LINK || type == BIS_LINK || type == PA_LINK){
				bt_cb->pkt_type = HCI_ISODATA_PKT;
			}
		}
		break;
	case HCI_SCODATA_PKT:
		break;
	case HCI_ISODATA_PKT:
		break;
	case HCI_DRV_PKT:
		break;
	default:
		kfree_skb(skb);
		ret = -EINVAL;
		return ret;
	}

	/* Incoming skb */
	bt_cb->incoming = 1;

	/* Time stamp */
	__net_timestamp(skb);

	skb_queue_tail(&hdev->rx_q, skb);
	queue_work(hdev->workqueue, &hdev->rx_work);

	return ret;
}
