void usb_anchor_suspend_wakeups(struct usb_anchor *anchor){
	if (anchor != NULL){
		#if 0
		__c11_atomic_fetch_add(&anchor->suspend_wakeups, 1, memory_order_release);
		#endif
	}
}
