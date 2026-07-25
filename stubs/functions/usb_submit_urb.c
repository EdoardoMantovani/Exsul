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
	if (director.usb_dev == NULL){
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
		printf("[!] failed to alloc usb transfer\n");
		ret = -ENOMEM;
		return ret;
	}

	if (usb_pipebulk(pipe)) {
		libusb_fill_bulk_transfer(transfer, director.usb_dev, ep, (u8 *)urb->transfer_buffer, urb->transfer_buffer_length, libusb_urb_callback, urb, 0);
	}else if (usb_pipeint(pipe)) {
		libusb_fill_interrupt_transfer(transfer, director.usb_dev, ep, (u8 *)urb->transfer_buffer, urb->transfer_buffer_length, libusb_urb_callback, urb, 0);
	}else{
		printf("[!] failed to find usb pipe\n");
		usb_transfer_put(transfer);
		ret = -EINVAL;
		return ret;
	}

	urb->hcpriv = transfer;
	/* The USB core owns an URB reference until the completion callback returns. */
	usb_get_urb(urb);
	/* increase the 'use_count' field by 1, assume that the transfer will be successful */
	atomic_fetch_add_explicit(&urb->use_count, 1, memory_order_relaxed);

	ret = libusb_submit_transfer(transfer);
	if (ret != LIBUSB_SUCCESS) {
		printf("[!] %s\n", libusb_strerror(ret));
		urb->hcpriv = NULL;
		/* if the transfer fails, decrease the urb 'use_count' field by 1 */
		atomic_fetch_sub_explicit(&urb->use_count, 1, memory_order_relaxed);
		usb_free_urb(urb);
		usb_transfer_put(transfer);
		ret = -EIO;
		return ret;
	}
	usb_transfer_note_submit();
	return ret;
}
