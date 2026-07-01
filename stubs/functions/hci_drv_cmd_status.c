int hci_drv_cmd_status(struct hci_dev *hdev, u16 cmd, u8 status){
	int ret = 0;
	struct sk_buff *skb = NULL;
	struct bt_skb_cb *bt_cb = NULL;
	struct hci_drv_ev_hdr *hdr = NULL;
	struct hci_drv_ev_cmd_status *ev = NULL;

	skb = bt_skb_alloc(sizeof(*hdr) + sizeof(*ev), GFP_KERNEL);
	if (skb == NULL){
		ret = -ENOMEM;
		return ret;
	}
	bt_cb = (struct bt_skb_cb *)skb->cb;
	hdr = skb_put(skb, sizeof(*hdr));
	hdr->opcode = __cpu_to_le16(HCI_DRV_EV_CMD_STATUS);
	hdr->len = __cpu_to_le16(sizeof(*ev));

	ev = skb_put(skb, sizeof(*ev));
	ev->opcode = __cpu_to_le16(cmd);
	ev->status = status;
	bt_cb->pkt_type = HCI_DRV_PKT;

	ret = hci_recv_frame(hdev, skb);
	return ret;
}

