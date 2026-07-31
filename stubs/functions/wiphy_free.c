void wiphy_free(void *wiphy){
	void *allocation = NULL;

	if(wiphy != NULL){
		allocation = ((void **)wiphy)[-1];
		if((void *)director.rdev == allocation){
			director.rdev = NULL;
		}
		if(director.wiphy == wiphy){
			director.wiphy = NULL;
		}
		free(allocation);
	}
}
