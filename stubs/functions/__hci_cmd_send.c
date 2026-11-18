int __hci_cmd_send(struct hci_dev *hdev, u16 opcode, u32 plen, const void *param){
	int ret = 0;
	struct sk_buff *skb = NULL;

	if (hdev == NULL || (plen != 0 && param == NULL)){
		ret = -EINVAL;
		return ret;
	}
	if (hci_opcode_ogf(opcode) != 0x3f){
		ret = -EINVAL;
		return ret;
	}
	skb = hci_cmd_sync_alloc(hdev, opcode, plen, param, NULL);
	if (skb == NULL){
		ret = -ENOMEM;
		return ret;
	}
	ret = hci_send_frame(hdev, skb);
	return ret;
}
