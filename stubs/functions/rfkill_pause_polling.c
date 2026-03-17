void rfkill_pause_polling(struct rfkill *rfkill){
	struct cfg80211_registered_device *rdev = NULL;

	rdev = director.rdev;
	if (rdev->rfkill_ops.poll == NULL){
		return;
	}

	director.rfkill->polling_paused = true;
	//cancel_delayed_work_sync(&rfkill->poll_work);
}
