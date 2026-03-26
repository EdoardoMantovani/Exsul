void usb_init_urb(struct urb *urb){
	if (urb != NULL) {
		kref_init(&urb->kref);
		INIT_LIST_HEAD(&urb->urb_list);
		INIT_LIST_HEAD(&urb->anchor_list);
	}
}
