int __hci_cmd_sync_status(struct hci_dev *hdev, u16 opcode, u32 plen, const void *param, u32 timeout){
	int ret = 0;

	ret = __hci_cmd_sync_status_sk(hdev, opcode, plen, param, 0, timeout, NULL);
	return ret;
}
