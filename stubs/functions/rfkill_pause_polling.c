void rfkill_pause_polling(struct rfkill *rfkill){
	if (director.rdev->rfkill_ops.poll == NULL){
		return;
	}

	director.rfkill->polling_paused = true;
	//cancel_delayed_work_sync(&rfkill->poll_work);
}
