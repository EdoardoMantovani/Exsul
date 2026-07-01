void usb_anchor_urb(struct urb *urb, struct usb_anchor *anchor){
	pthread_mutex_lock(&director.urb_anchor_mutex);
	usb_get_urb(urb);
	list_add_tail(&urb->anchor_list, &anchor->urb_list);
	urb->anchor = anchor;

	if (unlikely(anchor->poisoned)){
		#ifdef __clang__
		__c11_atomic_fetch_add(&urb->reject, 1, memory_order_release);
		#else
		atomic_fetch_add_explicit(&urb->reject, 1, memory_order_release);
		#endif
	}
	pthread_mutex_unlock(&director.urb_anchor_mutex);
}
