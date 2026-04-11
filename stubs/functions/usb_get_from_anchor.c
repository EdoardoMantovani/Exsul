struct urb *usb_get_from_anchor(struct usb_anchor *anchor){
	struct urb *victim = NULL;

	if (!list_empty(&anchor->urb_list)) {
		victim = list_entry(anchor->urb_list.next, struct urb, anchor_list);
		usb_get_urb(victim);
		/* inline '__usb_unanchor_urb' here */
                victim->anchor = NULL;
                list_del(&victim->anchor_list);
                usb_put_urb(victim);
                #if 0
                if (usb_anchor_check_wakeup(anchor)){
                        wake_up(&anchor->wait);
                }
                #endif
	}else{
		victim = NULL;
	}
	return victim;
}
