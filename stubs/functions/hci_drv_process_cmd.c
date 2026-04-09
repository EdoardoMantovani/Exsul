int hci_drv_process_cmd(struct hci_dev *hdev, struct sk_buff *skb){
	int ret = 0;
	u16 len = 0;
	u16 ogf = 0;
	u16 ocf = 0;
	u16 opcode = 0;
	struct hci_drv *hci_drv = NULL;
	struct hci_drv_cmd_hdr *hdr = NULL;
	const struct hci_drv_handler *handler = NULL;

	hci_drv = hdev->hci_drv;
	hdr = skb_pull_data(skb, sizeof(*hdr));
	if (hdr == NULL){
		ret = -EILSEQ;
		return ret;
	}
	opcode = __le16_to_cpu(hdr->opcode);
	len = __le16_to_cpu(hdr->len);
	if (len != skb->len){
		ret = -EILSEQ;
		return ret;
	}
	ogf = hci_opcode_ogf(opcode);
	ocf = hci_opcode_ocf(opcode);

	if (!hdev->hci_drv){
		ret = hci_drv_cmd_status(hdev, opcode, HCI_DRV_STATUS_UNKNOWN_COMMAND);
		return ret;
	}

	if (ogf != HCI_DRV_OGF_DRIVER_SPECIFIC) {
		if (opcode < hci_drv->common_handler_count){
			handler = &hci_drv->common_handlers[opcode];
		}
	} else {
		if (ocf < hci_drv->specific_handler_count){
			handler = &hci_drv->specific_handlers[ocf];
		}
	}

	if (handler == NULL || handler->func == NULL){
		ret = hci_drv_cmd_status(hdev, opcode, HCI_DRV_STATUS_UNKNOWN_COMMAND);
		return ret;
	}
	if (len != handler->data_len){
		ret = hci_drv_cmd_status(hdev, opcode, HCI_DRV_STATUS_INVALID_PARAMETERS);
		return ret;
	}
	ret = handler->func(hdev, skb->data, len);
	return ret;
}
