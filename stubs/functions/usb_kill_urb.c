void usb_kill_urb(struct urb *urb){
	if (!(urb && urb->dev && urb->ep)){
		return;
	}
	#ifdef __clang__
	__c11_atomic_fetch_add(&urb->reject, 1, memory_order_release);
	__c11_atomic_fetch_sub(&urb->reject, 1, memory_order_release);
	#else
	atomic_fetch_add_explicit(&urb->reject, 1, memory_order_release);
	//wait_event(usb_kill_urb_queue, atomic_read(&urb->use_count) == 0);
	atomic_fetch_sub_explicit(&urb->reject, 1, memory_order_release);
	#endif
}
