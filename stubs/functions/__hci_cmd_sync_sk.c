struct sk_buff *__hci_cmd_sync_sk(struct hci_dev *hdev, u16 opcode, u32 plen, const void *param, u8 event, u32 timeout, struct sock *sk){
	int err = 0;
	struct hci_request req = {};
	struct sk_buff *skb = NULL;

	printf("[hdev] Opcode 0x%4.4x", opcode);

	hci_request_init(&req, hdev);

	hci_cmd_sync_add(&req, opcode, plen, param, event, sk);

	hdev->req_status = HCI_REQ_PEND;

	err = hci_req_sync_run(&req);
	if (err < 0){
		return ERR_PTR(err);
	}

	//err = wait_event_interruptible_timeout(hdev->req_wait_q, hdev->req_status != HCI_REQ_PEND, timeout);

	if (err == -ERESTARTSYS){
		return ERR_PTR(-EINTR);
	}

	switch (hdev->req_status) {
	case HCI_REQ_DONE:
		err = -bt_to_errno(hdev->req_result);
		break;

	case HCI_REQ_CANCELED:
		err = -hdev->req_result;
		break;

	default:
		err = -ETIMEDOUT;
		break;
	}

	hdev->req_status = 0;
	hdev->req_result = 0;
	skb = hdev->req_rsp;
	hdev->req_rsp = NULL;

	if (err < 0) {
		kfree_skb(skb);
		return ERR_PTR(err);
	}

	/* If command return a status event skb will be set to NULL as there are
	 * no parameters.
	 */
	if (skb == NULL){
		return ERR_PTR(-ENODATA);
	}
	return skb;
}
