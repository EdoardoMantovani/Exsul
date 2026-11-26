struct urb *usb_get_from_anchor(struct usb_anchor *anchor){
	int ret = 0;
	struct urb *urb = NULL;

	if(anchor == NULL){
		return NULL;
	}
	pthread_mutex_lock(&director.urb_anchor_mutex);
	if(!list_empty(&anchor->urb_list)){
		urb = list_first_entry(&anchor->urb_list, struct urb, anchor_list);
		usb_get_urb(urb);
		urb->anchor = NULL;
		list_del_init(&urb->anchor_list);
		usb_put_urb(urb);
	}
	pthread_mutex_unlock(&director.urb_anchor_mutex);
	return urb;
}
