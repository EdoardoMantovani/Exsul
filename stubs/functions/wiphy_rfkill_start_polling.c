void wiphy_rfkill_start_polling(struct wiphy *wiphy){
	void (*rfkill_poll_cb)(struct rfkill *rfkill, void *data) = NULL;

	rfkill_poll_cb = search_similar_function("_rfkill_poll");

	if(rfkill_poll_cb == NULL){
		return;
	}

	director.rdev.rfkill_ops.poll = rfkill_poll_cb;
	director.rfkill.polling_paused = false;
	if (director.rfkill.suspended == true){
		return;
	}
	//director.rdev->rfkill_ops.poll();
}
