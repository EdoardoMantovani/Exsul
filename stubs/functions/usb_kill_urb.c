void usb_kill_urb(struct urb *urb) {
	struct libusb_transfer *transfer = NULL;
	if (!(urb != NULL && urb->dev != NULL && urb->ep != NULL)){
		goto end;
	}

	atomic_fetch_add_explicit(&urb->reject, 1, memory_order_release);
	/*
	 * this needs a careful analysis: the field hcpriv seems to be used only in the 'atmel' usb Wi-Fi driver,
	 * which isn't important at all imo, but what if in the future it will be used also by others ?
	 * probably I will have to add a field in the main context struct.
	 */
	transfer = (struct libusb_transfer *)urb->hcpriv;
	if (transfer){
		libusb_cancel_transfer(transfer);
	}

	#ifdef __linux__
	while (atomic_load_explicit(&urb->use_count, memory_order_acquire) > 0){
		sched_yield();
	}
	#endif
	atomic_fetch_sub_explicit(&urb->reject, 1, memory_order_release);
	end:
}

