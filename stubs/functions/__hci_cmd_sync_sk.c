struct sk_buff *__hci_cmd_sync_sk(struct hci_dev *hdev, u16 opcode, u32 plen, const void *param, u8 event, u32 timeout, struct sock *sk){
	int err = 0;
	long remaining = timeout;
	struct hci_request req = {};
	struct sk_buff *skb = NULL;
	struct wait_queue_entry wait = {};

	if(hdev == NULL || (plen != 0 && param == NULL)){
		return ERR_PTR(-EINVAL);
	}
	mutex_lock(&hdev->req_lock);
	hci_request_init(&req, hdev);
	hci_cmd_sync_add(&req, opcode, plen, param, event, sk);
	hdev->req_rsp = NULL;
	hdev->req_result = 0;
	hdev->req_status = HCI_REQ_PEND;
	err = hci_req_sync_run(&req);
	if(err < 0){
		hdev->req_status = 0;
		mutex_unlock(&hdev->req_lock);
		return ERR_PTR(err);
	}
	init_wait_entry(&wait, 0);
	while(hdev->req_status == HCI_REQ_PEND){
		err = prepare_to_wait_event(&hdev->req_wait_q, &wait, TASK_INTERRUPTIBLE);
		if(err < 0 || hdev->req_status != HCI_REQ_PEND){
			break;
		}
		remaining = schedule_timeout(remaining);
		if(remaining == 0){
			err = -ETIMEDOUT;
			break;
		}
	}
	finish_wait(&hdev->req_wait_q, &wait);
	if(err == -ERESTARTSYS || err == -ERESTART){
		hci_cmd_sync_cancel(hdev, EINTR);
		err = -EINTR;
	}else if(err == -ETIMEDOUT){
		hci_cmd_sync_cancel(hdev, ETIMEDOUT);
	}else if(hdev->req_status == HCI_REQ_DONE){
		err = -bt_to_errno(hdev->req_result);
	}else if(hdev->req_status == HCI_REQ_CANCELED){
		err = -hdev->req_result;
	}else{
		err = -ETIMEDOUT;
	}
	hdev->req_status = 0;
	hdev->req_result = 0;
	skb = hdev->req_rsp;
	hdev->req_rsp = NULL;
	hci_drop_request(hdev);
	if(err < 0){
		kfree_skb(skb);
		mutex_unlock(&hdev->req_lock);
		return ERR_PTR(err);
	}
	if(skb == NULL){
		mutex_unlock(&hdev->req_lock);
		return ERR_PTR(-ENODATA);
	}
	mutex_unlock(&hdev->req_lock);
	return skb;
}
