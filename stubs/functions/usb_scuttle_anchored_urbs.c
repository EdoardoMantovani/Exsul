void usb_scuttle_anchored_urbs(struct usb_anchor *anchor){
	int ret = 0;
	struct urb *urb = NULL;

	if(anchor == NULL){
		return;
	}
	pthread_mutex_lock(&director.urb_anchor_mutex);
	while(!list_empty(&anchor->urb_list)){
		urb = list_last_entry(&anchor->urb_list, struct urb, anchor_list);
		urb->anchor = NULL;
		list_del_init(&urb->anchor_list);
		usb_put_urb(urb);
	}
	pthread_mutex_unlock(&director.urb_anchor_mutex);
}
