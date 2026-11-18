struct sk_buff *hci_cmd_sync(struct hci_dev *hdev, u16 opcode, u32 plen, const void *param, u32 timeout){
	struct sk_buff *skb = NULL;

	if (!test_bit(HCI_UP, &hdev->flags)){
		return ERR_PTR(-ENETDOWN);
	}

	skb = __hci_cmd_sync(hdev, opcode, plen, param, timeout);
	return skb;
}
