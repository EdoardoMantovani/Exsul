void usb_scuttle_anchored_urbs(struct usb_anchor *anchor){
	int surely_empty = 0;
	struct urb *victim = NULL;

	do {
		while (!list_empty(&anchor->urb_list)) {
			victim = list_entry(anchor->urb_list.prev,  struct urb, anchor_list);
			/* inline '__usb_unanchor_urb' here */
			victim->anchor = NULL;
			list_del(&victim->anchor_list);
			usb_put_urb(victim);
			#if 0
			if (usb_anchor_check_wakeup(anchor)){
				wake_up(&anchor->wait);
			}
			#endif
		}
		surely_empty = usb_anchor_check_wakeup(anchor);
	} while (!surely_empty);
}
