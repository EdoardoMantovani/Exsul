void wiphy_free(void *wiphy){
	if(wiphy != NULL){
		free(wiphy);
	}
}
