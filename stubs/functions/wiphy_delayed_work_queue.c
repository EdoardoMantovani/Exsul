void wiphy_delayed_work_queue(struct wiphy *wiphy, struct wiphy_delayed_work *dwork, unsigned long delay){
	if (delay == 0) {
		timer_delete(&dwork->timer);
		wiphy_work_queue(wiphy, &dwork->work);
		return;
	}

	dwork->wiphy = wiphy;
	mod_timer(&dwork->timer, director.jiffies + delay);
}
