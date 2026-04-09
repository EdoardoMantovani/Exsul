int usb_submit_urb(struct urb *urb, int mem_flags){
	int ret = 0;
	unsigned int ep = 0;
	unsigned int pipe = 0;
	struct libusb_transfer *transfer = NULL;

	if (urb == NULL){
		ret = -EINVAL;
		return ret;
	}

	atomic_load_explicit(&urb->reject, memory_order_acquire);

	pipe = urb->pipe;
	ep   = convert_usb_pipe_to_ep(pipe);

	transfer = libusb_alloc_transfer(0);
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
		libusb_free_transfer(transfer);
		ret = -EINVAL;
		return ret;
	}

	urb->hcpriv = transfer;
	/* increase the 'use_count' field by 1, ipotize that the transfer will be succesfull */
	atomic_fetch_add_explicit(&urb->use_count, 1, memory_order_relaxed);

	ret = libusb_submit_transfer(transfer);
	if (ret != LIBUSB_SUCCESS) {
		printf("[!] %s\n", libusb_strerror(ret));
		urb->hcpriv = NULL;
		/* if the transfer fails, decrease the urb 'use_count' field by 1 */
		atomic_fetch_sub_explicit(&urb->use_count, 1, memory_order_relaxed);
		libusb_free_transfer(transfer);
		ret = -EIO;
		return ret;
	}
	return ret;
}
