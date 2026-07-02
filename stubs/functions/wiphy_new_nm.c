struct wiphy *wiphy_new_nm(const struct cfg80211_ops *ops, int sizeof_priv, const char *requested_name){
	size_t alloc_size = 0;
	size_t wiphy_size = 0;
	void *allocation = NULL;
	struct device *dev = NULL;
	struct wiphy *wiphy = NULL;
	struct cfg80211_registered_device *rdev = NULL;

	wiphy_size = director.struct_sizes.wiphy;
	if(sizeof_priv < 0 || wiphy_size == 0){
		#ifdef ENABLE_DBG_PRINT
		printf("[!]  wiphy_size or sizeof_priv are wrong!\n");
		#endif
		return NULL;
	}

	alloc_size = sizeof(struct cfg80211_registered_device) + sizeof(void *) + 31 + wiphy_size + sizeof_priv;
	allocation = calloc(1, alloc_size);
	if(allocation == NULL){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] failed to allocate the new wiphy!\n");
		#endif
		return NULL;
	}
	rdev = allocation;
	director.rdev = rdev;
	wiphy = (struct wiphy *)ALIGN((uintptr_t)((char *)allocation +
					 sizeof(struct cfg80211_registered_device) +
					 sizeof(void *)), 32);
	((void **)wiphy)[-1] = allocation;
	rdev->ops = ops;
	rdev->wiphy_idx = 0;
	INIT_LIST_HEAD(&rdev->beacon_registrations);
	INIT_LIST_HEAD(&rdev->sched_scan_req_list);
	INIT_LIST_HEAD(&rdev->wiphy_work_list);
	rdev->wiphy = wiphy;
	rdev->wiphy_work.func = cfg80211_wiphy_worker;
	/*
	 * Initialize wiphy parameters to IEEE 802.11 MIB default values.
	 * Fragmentation and RTS threshold are disabled by default with the
	 * special -1 value.
	 */
	wiphy->retry_short = 7;
	wiphy->retry_long = 4;
	wiphy->frag_threshold = (u32)-1;
	wiphy->rts_threshold = (u32)-1;
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
