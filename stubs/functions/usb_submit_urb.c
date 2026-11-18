int usb_submit_urb(struct urb *urb, int mem_flags){
	int ret = 0;
	unsigned int ep = 0;
	unsigned int pipe = 0;
	struct libusb_transfer *transfer = NULL;

	if (urb == NULL){
		ret = -EINVAL;
		return ret;
	}

	if (atomic_load_explicit(&urb->reject, memory_order_acquire) != 0){
		ret = -EPERM;
		return ret;
	}
	if (director.usb_dev == NULL ||
	    atomic_load_explicit(&director.usb_device_lost, memory_order_acquire)){
		ret = -ENODEV;
		return ret;
	}
	if (urb->hcpriv != NULL){
		ret = -EBUSY;
		return ret;
	}

	pipe = urb->pipe;
	ep   = convert_usb_pipe_to_ep(pipe);
	transfer = usb_transfer_get();
	if (transfer == NULL){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] failed to alloc usb transfer\n");
		#endif
		ret = -ENOMEM;
		return ret;
	}

	transfer->flags = 0;

	if (usb_pipebulk(pipe)) {
		libusb_fill_bulk_transfer(transfer, director.usb_dev, ep, (u8 *)urb->transfer_buffer, urb->transfer_buffer_length, libusb_urb_callback, urb, 0);
		if (!usb_pipein(pipe) && (urb->transfer_flags & URB_ZERO_PACKET)){
			transfer->flags |= LIBUSB_TRANSFER_ADD_ZERO_PACKET;
		}
	}else if (usb_pipeint(pipe)) {
		libusb_fill_interrupt_transfer(transfer, director.usb_dev, ep, (u8 *)urb->transfer_buffer, urb->transfer_buffer_length, libusb_urb_callback, urb, 0);
	}else{
		#ifdef ENABLE_DBG_PRINT
		printf("[!] failed to find usb pipe\n");
		#endif
		usb_transfer_put(transfer);
		ret = -EINVAL;
		return ret;
	}

	urb->hcpriv = transfer;
	usb_get_urb(urb);
	/* increase the 'use_count' field by 1, assume that the transfer will be successful */
	atomic_fetch_add_explicit(&urb->use_count, 1, memory_order_relaxed);
	/* Completion may run before libusb_submit_transfer() returns. */
	usb_transfer_note_submit();
	ret = libusb_submit_transfer(transfer);
	if (ret != LIBUSB_SUCCESS) {
		#ifdef ENABLE_DBG_PRINT
		printf("[!] %s\n", libusb_strerror(ret));
		#endif
		urb->hcpriv = NULL;
		/* if the transfer fails, decrease the urb 'use_count' field by 1 */
		atomic_fetch_sub_explicit(&urb->use_count, 1, memory_order_relaxed);
		usb_free_urb(urb);
		usb_transfer_put(transfer);
		usb_transfer_note_complete();
		if (ret == LIBUSB_ERROR_NO_DEVICE){
			atomic_store_explicit(&director.usb_device_lost, true,
					      memory_order_release);
			ret = -ENODEV;
		}else{
			ret = -EIO;
		}
		return ret;
	}
	return ret;
}
