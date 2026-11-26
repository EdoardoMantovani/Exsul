int usb_submit_urb(struct urb *urb, int mem_flags){
	int ret = 0;
	int packets = 0;
	unsigned int ep = 0;
	unsigned int pipe = 0;
	unsigned int length = 0;
	unsigned char *control = NULL;
	struct usb_submit *submit = NULL;
	struct usb_ctrlrequest *setup = NULL;
	struct libusb_transfer *transfer = NULL;

	if(urb == NULL){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] NULL urb detected!!\n");
		#endif
		ret = -EINVAL;
		return ret;
	}
	if(atomic_load_explicit(&urb->reject, memory_order_acquire) != 0){
		ret = -EPERM;
		return ret;
	}
	if(director.usb_dev == NULL || atomic_load_explicit(&director.usb_device_lost, memory_order_acquire)){
		ret = -ENODEV;
		return ret;
	}
	if(urb->hcpriv != NULL){
		ret = -EBUSY;
		return ret;
	}
	pipe = urb->pipe;
	ep = convert_usb_pipe_to_ep(pipe);
	submit = calloc(1, sizeof(*submit));
	if(submit == NULL){
		ret = -ENOMEM;
		return ret;
	}
	submit->urb = urb;
	submit->pooled_transfer = !usb_pipeisoc(pipe);
	packets = urb->number_of_packets;
	transfer = usb_pipeisoc(pipe) ? usb_transfer_get_isoc(packets) : usb_transfer_get();
	if(transfer == NULL){
		free(submit);
		ret = -ENOMEM;
		return ret;
	}
	transfer->flags = 0;
	if(usb_pipebulk(pipe)){
		libusb_fill_bulk_transfer(transfer, director.usb_dev, ep, urb->transfer_buffer, urb->transfer_buffer_length, libusb_urb_callback, submit, 0);
		if(!usb_pipein(pipe) && (urb->transfer_flags & URB_ZERO_PACKET)){
			transfer->flags |= LIBUSB_TRANSFER_ADD_ZERO_PACKET;
		}
	}else if(usb_pipeint(pipe)){
		libusb_fill_interrupt_transfer(transfer, director.usb_dev, ep, urb->transfer_buffer, urb->transfer_buffer_length, libusb_urb_callback, submit, 0);
	}else if(usb_pipecontrol(pipe)){
		setup = (struct usb_ctrlrequest *)urb->setup_packet;
		if(setup == NULL){
			#ifdef ENABLE_DBG_PRINT
			printf("[!] usb_submit_urb: control urb has no setup packet, pipe=0x%08x\n", pipe);
			#endif
			ret = -EINVAL;
			goto fail;
		}
		length = LIBUSB_CONTROL_SETUP_SIZE + urb->transfer_buffer_length;
		control = calloc(1, length);
		if(control == NULL){
			ret = -ENOMEM;
			goto fail;
		}
		libusb_fill_control_setup(control, setup->bRequestType, setup->bRequest, le16_to_cpu(setup->wValue), le16_to_cpu(setup->wIndex), urb->transfer_buffer_length);
		if(!usb_pipein(pipe) && urb->transfer_buffer != NULL){
			memcpy(control + LIBUSB_CONTROL_SETUP_SIZE, urb->transfer_buffer, urb->transfer_buffer_length);
		}
		submit->owned_buffer = control;
		submit->control = true;
		libusb_fill_control_transfer(transfer, director.usb_dev, control, libusb_urb_callback, submit, 0);
	}else if(usb_pipeisoc(pipe)){
		if(packets <= 0){
			#ifdef ENABLE_DBG_PRINT
			printf("[!] usb_submit_urb: isochronous urb has no packets, pipe=0x%08x\n", pipe);
			#endif
			ret = -EINVAL;
			goto fail;
		}
		submit->isoc = true;
		libusb_fill_iso_transfer(transfer, director.usb_dev, ep, urb->transfer_buffer, urb->transfer_buffer_length, packets, libusb_urb_callback, submit, 0);
		for(int i = 0; i < packets; i++){
			transfer->iso_packet_desc[i].length = urb->iso_frame_desc[i].length;
		}
	}else{
		#ifdef ENABLE_DBG_PRINT
		printf("[!] usb_submit_urb: invalid pipe=0x%08x!\n", pipe);
		#endif
		ret = -EINVAL;
		goto fail;
	}
	urb->status = -EINPROGRESS;
	urb->actual_length = 0;
	urb->hcpriv = transfer;
	usb_get_urb(urb);
	atomic_fetch_add_explicit(&urb->use_count, 1, memory_order_relaxed);
	usb_transfer_note_submit();
	ret = libusb_submit_transfer(transfer);
	if(ret == LIBUSB_SUCCESS){
		ret = 0;
		return ret;
	}
	printf("[!] usb_submit_urb: libusb_submit_transfer failed: %s (%d), transfer=%p pipe=0x%08x\n",
	       libusb_error_name(ret), ret, transfer, pipe);
	usb_transfer_note_complete();
	urb->hcpriv = NULL;
	atomic_fetch_sub_explicit(&urb->use_count, 1, memory_order_relaxed);
	usb_free_urb(urb);
	if(ret == LIBUSB_ERROR_NO_DEVICE){
		atomic_store_explicit(&director.usb_device_lost, true, memory_order_release);
		ret = -ENODEV;
	}else{
		ret = -EIO;
	}
	fail:
	usb_release_submit(transfer, submit);
	return ret;
}
