void usb_unanchor_urb(struct urb *urb){
	struct usb_anchor *anchor = NULL;

	if (urb == NULL){
		return;
	}

	anchor = urb->anchor;
	if (anchor == NULL){
		return;
	}
	pthread_mutex_lock(&director.urb_anchor_mutex);
	/*
	 * At this point, we could be competing with another thread which
	 * has the same intention. To protect the urb from being unanchored
	 * twice, only the winner of the race gets the job.
	 */
	if (likely(anchor == urb->anchor)){
		/* inline '__usb_unanchor_urb' here */
		urb->anchor = NULL;
		list_del(&urb->anchor_list);
		usb_put_urb(urb);
		#if 0
		if (usb_anchor_check_wakeup(anchor)){
			wake_up(&anchor->wait);
		}
		#endif
	}

	pthread_mutex_unlock(&director.urb_anchor_mutex);
}
