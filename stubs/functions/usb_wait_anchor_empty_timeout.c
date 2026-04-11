int usb_wait_anchor_empty_timeout(struct usb_anchor *anchor, unsigned int timeout){
	struct timespec deadline;
	struct timespec now;
	struct timeval tv = {0, 500};
	int empty = 0;

	if (anchor == NULL){
		return 1;
	}

	clock_gettime(CLOCK_MONOTONIC, &deadline);
	deadline.tv_sec  += (long)(timeout / 1000);
	deadline.tv_nsec += (long)(timeout % 1000) * 1000000L;
	if (deadline.tv_nsec >= 1000000000L){
		deadline.tv_sec++;
		deadline.tv_nsec -= 1000000000L;
	}

	while (1){
		pthread_mutex_lock(&director.urb_anchor_mutex);
		empty = list_empty(&anchor->urb_list);
		pthread_mutex_unlock(&director.urb_anchor_mutex);

		if (empty){
			return 1;
		}

		clock_gettime(CLOCK_MONOTONIC, &now);
		if (now.tv_sec > deadline.tv_sec ||
		    (now.tv_sec == deadline.tv_sec && now.tv_nsec >= deadline.tv_nsec)){
			return 0;
		}

		libusb_handle_events_timeout(NULL, &tv);
	}
}
