void usb_kill_urb(struct urb *urb){
	int ret = 0;
	struct libusb_transfer *transfer = NULL;

	if(urb == NULL){
		return;
	}
	atomic_fetch_add_explicit(&urb->reject, 1, memory_order_release);
	if(director.usb_completion_initialized == true){
		pthread_mutex_lock(&director.usb_completion_lock);
	}
	transfer = (struct libusb_transfer *)urb->hcpriv;
	if(transfer != NULL){
		ret = libusb_cancel_transfer(transfer);
	}
	if(director.usb_completion_initialized == true && !pthread_equal(pthread_self(), director.libusb_event_thread)){
		while(atomic_load_explicit(&urb->use_count, memory_order_acquire) != 0){
			pthread_cond_wait(&director.usb_completion_cond, &director.usb_completion_lock);
		}
	}
	if(director.usb_completion_initialized == true){
		pthread_mutex_unlock(&director.usb_completion_lock);
	}
	atomic_fetch_sub_explicit(&urb->reject, 1, memory_order_release);
}
