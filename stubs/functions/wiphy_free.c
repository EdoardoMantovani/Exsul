void wiphy_free(void *wiphy){
	void *allocation = NULL;
	struct cfg80211_registered_device *rdev = NULL;

	if(wiphy != NULL){
		allocation = ((void **)wiphy)[-1];
		rdev = allocation;
		cancel_work_sync(&rdev->wiphy_work);
		if((void *)director.rdev == allocation){
			director.rdev = NULL;
		}
		if(director.wiphy == wiphy){
			director.wiphy = NULL;
		}
		free(allocation);
	}
}
