struct wiphy *wiphy_new_nm(const struct cfg80211_ops *ops, int sizeof_priv, const char *requested_name){
	int alloc_size = 0;
	struct device *dev = NULL;
	struct wiphy *wiphy = NULL;
	struct cfg80211_registered_device *rdev = NULL;

	alloc_size = sizeof_priv + sizeof(struct cfg80211_registered_device);
	rdev = (struct cfg80211_registered_device *)malloc(alloc_size);
	director.rdev = rdev;
	wiphy = (struct wiphy *)((char *)rdev + sizeof(struct cfg80211_registered_device));
	rdev->ops = ops;
	rdev->wiphy_idx = 0;
	INIT_LIST_HEAD(&rdev->beacon_registrations);
	INIT_LIST_HEAD(&rdev->sched_scan_req_list);
	/*
	 * Initialize wiphy parameters to IEEE 802.11 MIB default values.
	 * Fragmentation and RTS threshold are disabled by default with the
	 * special -1 value.
	 */
	wiphy->retry_short = 7;
	wiphy->retry_long = 4;
	wiphy->frag_threshold = (u32) -1;
	wiphy->rts_threshold = (u32) -1;
	wiphy->coverage_class = 0;

	wiphy->max_num_csa_counters = 1;

	wiphy->max_sched_scan_plans = 1;
	wiphy->max_sched_scan_plan_interval = ~0U;

	dev = &wiphy->dev;
	if(requested_name != NULL){
		dev->init_name = requested_name;
	}else{
		dev->init_name = "phy0";
	}

	return wiphy;
}
