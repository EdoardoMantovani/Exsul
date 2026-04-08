void wiphy_rfkill_start_polling(struct wiphy *wiphy){
	struct cfg80211_registered_device *rdev = NULL;
	void (*rfkill_poll_cb)(struct rfkill *rfkill, void *data) = NULL;

	fast_get_symbol("_rfkill_poll", &rfkill_poll_cb);

	if(rfkill_poll_cb == NULL){
		goto end;
	}

	rdev = director.rdev;
	rdev->rfkill_ops.poll = rfkill_poll_cb;
	director.rfkill->polling_paused = false;
	if (director.rfkill->suspended == true){
		goto end;
	}
	//rdev->rfkill_ops.poll();
	end:
}
