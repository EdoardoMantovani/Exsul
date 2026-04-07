void wiphy_free(void *wiphy){
	if(wiphy != NULL){
		free((char *)wiphy - sizeof(struct cfg80211_registered_device));
	}
}
