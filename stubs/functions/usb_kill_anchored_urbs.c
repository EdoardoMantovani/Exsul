void usb_kill_anchored_urbs(struct usb_anchor *anchor){
	int surely_empty = 0;
	struct urb *victim = NULL;

	do {
		while (!list_empty(&anchor->urb_list)) {
			victim = list_entry(anchor->urb_list.prev, struct urb, anchor_list);
			/* make sure the URB isn't freed before we kill it */
			usb_get_urb(victim);
			/* this will unanchor the URB */
			usb_kill_urb(victim);
			usb_put_urb(victim);
		}
		surely_empty = usb_anchor_check_wakeup(anchor);
	}while (!surely_empty);
}
