void usb_kill_anchored_urbs(struct usb_anchor *anchor){
	int ret = 0;
	struct urb *urb = NULL;

	if(anchor == NULL){
		return;
	}
	while(true){
		pthread_mutex_lock(&director.urb_anchor_mutex);
		if(list_empty(&anchor->urb_list)){
			pthread_mutex_unlock(&director.urb_anchor_mutex);
			break;
		}
		urb = list_last_entry(&anchor->urb_list, struct urb, anchor_list);
		usb_get_urb(urb);
		pthread_mutex_unlock(&director.urb_anchor_mutex);
		usb_kill_urb(urb);
		usb_put_urb(urb);
	}
}
