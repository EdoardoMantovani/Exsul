int __hci_cmd_sync_status_sk(struct hci_dev *hdev, u16 opcode, u32 plen, const void *param, u8 event, u32 timeout, struct sock *sk){
	int ret = 0;
	u8 status = 0;
	struct sk_buff *skb = NULL;

	skb = __hci_cmd_sync_sk(hdev, opcode, plen, param, event, timeout, sk);

	/* If command return a status event, skb will be set to -ENODATA */
	if (skb == ERR_PTR(-ENODATA)){
		return ret;
	}

	if (IS_ERR(skb)) {
		if (!event){
			printf("[hdev] Opcode 0x%4.4x failed: %ld", opcode, PTR_ERR(skb));
		}
		ret = PTR_ERR(skb);
		return ret;
	}

	status = skb->data[0];
	kfree_skb(skb);
	return status;
}
