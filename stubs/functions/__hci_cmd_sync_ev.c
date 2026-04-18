struct sk_buff *__hci_cmd_sync_ev(struct hci_dev *hdev, u16 opcode, u32 plen, const void *param, u8 event, u32 timeout){
	return __hci_cmd_sync_sk(hdev, opcode, plen, param, event, timeout, NULL);
}
