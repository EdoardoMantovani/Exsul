int regulatory_hint(struct wiphy *wiphy, const char *alpha2){
	int ret = 0;
	struct regulatory_request *request = NULL;

	if (!alpha2 || !wiphy){
		ret = -EINVAL;
		return ret;
	}

	wiphy->regulatory_flags &= ~REGULATORY_CUSTOM_REG;

	request = __kmalloc_large_noprof(sizeof(struct regulatory_request), GFP_KERNEL);
	if (!request){
		ret = -ENOMEM;
		return ret;
	}
	//request->wiphy_idx = get_wiphy_idx(wiphy);

	request->alpha2[0] = alpha2[0];
	request->alpha2[1] = alpha2[1];
	request->initiator = NL80211_REGDOM_SET_BY_DRIVER;

	/* Allow calling CRDA again */
	director.ieee80211_local->reg_crda_timeouts = 0;

	/* inline here '' */
	request->alpha2[0] = toupper(request->alpha2[0]);
	request->alpha2[1] = toupper(request->alpha2[1]);

	pthread_mutex_lock(&director.ieee80211_local->regulatory_req_mutex);
	list_add_tail(&request->list, &director.ieee80211_local->reg_requests_list);
	pthread_mutex_unlock(&director.ieee80211_local->regulatory_req_mutex);

	return ret;
}
