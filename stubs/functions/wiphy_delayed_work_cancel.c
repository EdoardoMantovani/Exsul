void wiphy_delayed_work_cancel(struct wiphy *wiphy, struct wiphy_delayed_work *dwork){
	timer_delete_sync(&dwork->timer);
	wiphy_work_cancel(wiphy, &dwork->work);
}
