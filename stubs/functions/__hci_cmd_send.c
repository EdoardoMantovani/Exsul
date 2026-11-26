int __hci_cmd_send(struct hci_dev *hdev, u16 opcode, u32 plen, const void *param){
	int ret = 0;
	struct sk_buff *skb = NULL;

	if (hdev == NULL || (plen != 0 && param == NULL)){
		printf("[!] __hci_cmd_send: invalid hdev/parameter, opcode=0x%04x plen=%u\n", opcode, plen);
		ret = -EINVAL;
		return ret;
	}
	if (hci_opcode_ogf(opcode) != 0x3f){
		printf("[!] __hci_cmd_send: non-vendor opcode=0x%04x\n", opcode);
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
