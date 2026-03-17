struct ieee80211_hw *ieee80211_alloc_hw_nm(size_t priv_data_len, struct ieee80211_ops *ieee80211_ops, const char *requested_name){
	int priv_size = 0;
	bool emulate_chanctx = false;
	struct wiphy *wiphy = NULL;
	struct ieee80211_hw *hw = NULL;
	struct ieee80211_local *local = NULL;

	if(priv_data_len == 0 || ieee80211_ops == NULL){
		return NULL;
	}

	if (director.runtime_generated_structs == NULL) {
		return NULL;
	}

	/* exclude from the check 'ieee80211_ops->tx' because it is often the first pointer in the struct */
	if (ieee80211_ops->start == NULL || ieee80211_ops->stop == NULL || ieee80211_ops->config == NULL){
		return NULL;
	}

	if (ieee80211_ops->sta_state && (ieee80211_ops->sta_add || ieee80211_ops->sta_remove)){
		return NULL;
	}

	if (!!ieee80211_ops->link_info_changed != !!ieee80211_ops->vif_cfg_changed || (ieee80211_ops->link_info_changed && ieee80211_ops->bss_info_changed)){
		return NULL;
	}

	/* check all or no channel context operations exist */
	if (!ieee80211_ops->add_chanctx || !ieee80211_ops->remove_chanctx || !ieee80211_ops->change_chanctx || !ieee80211_ops->assign_vif_chanctx || !ieee80211_ops->unassign_vif_chanctx){
		return NULL;
	}

	/* Ensure 32-byte alignment of our private data and hw private data.
	 * We use the wiphy priv data for both our ieee80211_local and for
	 * the driver's private data
	 *
	 * In memory it'll be like this:
	 *
	 * +-------------------------+
	 * | struct wiphy	    |
	 * +-------------------------+
	 * | struct ieee80211_local  |
	 * +-------------------------+
	 * | driver's private data   |
	 * +-------------------------+
	 *
	 */
	priv_size = ALIGN(sizeof(*local), 32) + priv_data_len; /* 'NETDEV_ALIGN' = 32 */

	wiphy = wiphy_new_nm(NULL, /*&mac80211_config_ops,*/ priv_size, requested_name);

	if (wiphy == NULL){
		return NULL;
	}

	wiphy->flags |= WIPHY_FLAG_NETNS_OK |
			WIPHY_FLAG_4ADDR_AP |
			WIPHY_FLAG_4ADDR_STATION |
			WIPHY_FLAG_REPORTS_OBSS |
			WIPHY_FLAG_OFFCHAN_TX;

	if (emulate_chanctx || ieee80211_ops->remain_on_channel){
		wiphy->flags |= WIPHY_FLAG_HAS_REMAIN_ON_CHANNEL;
	}

	wiphy->bss_param_support = WIPHY_BSS_PARAM_CTS_PROT |
				   WIPHY_BSS_PARAM_SHORT_PREAMBLE |
				   WIPHY_BSS_PARAM_SHORT_SLOT_TIME |
				   WIPHY_BSS_PARAM_BASIC_RATES |
				   WIPHY_BSS_PARAM_AP_ISOLATE |
				   WIPHY_BSS_PARAM_HT_OPMODE |
				   WIPHY_BSS_PARAM_P2P_CTWINDOW |
				   WIPHY_BSS_PARAM_P2P_OPPPS;
	wiphy->features |= NL80211_FEATURE_SK_TX_STATUS |
			   NL80211_FEATURE_SAE |
			   NL80211_FEATURE_HT_IBSS |
			   NL80211_FEATURE_VIF_TXPOWER |
			   NL80211_FEATURE_MAC_ON_CREATE |
			   NL80211_FEATURE_USERSPACE_MPM |
			   NL80211_FEATURE_FULL_AP_CLIENT_STATE;
	wiphy_ext_feature_set(wiphy, NL80211_EXT_FEATURE_FILS_STA);
	wiphy_ext_feature_set(wiphy,
			      NL80211_EXT_FEATURE_CONTROL_PORT_OVER_NL80211);
	wiphy_ext_feature_set(wiphy,
			      NL80211_EXT_FEATURE_CONTROL_PORT_NO_PREAUTH);
	wiphy_ext_feature_set(wiphy,
			      NL80211_EXT_FEATURE_CONTROL_PORT_OVER_NL80211_TX_STATUS);
	wiphy_ext_feature_set(wiphy,
			      NL80211_EXT_FEATURE_SCAN_FREQ_KHZ);
	wiphy_ext_feature_set(wiphy,
			      NL80211_EXT_FEATURE_POWERED_ADDR_CHANGE);

	if (ieee80211_ops->hw_scan == NULL) {
		wiphy->features |= NL80211_FEATURE_LOW_PRIORITY_SCAN |
				   NL80211_FEATURE_AP_SCAN;
		/*
		 * if the driver behaves correctly using the probe request
		 * (template) from mac80211, then both of these should be
		 * supported even with hw scan - but let drivers opt in.
		 */
		wiphy_ext_feature_set(wiphy,
				      NL80211_EXT_FEATURE_SCAN_RANDOM_SN);
		wiphy_ext_feature_set(wiphy,
				      NL80211_EXT_FEATURE_SCAN_MIN_PREQ_CONTENT);
	}

	if (!ieee80211_ops->set_key) {
		wiphy->flags |= WIPHY_FLAG_IBSS_RSN;
		#ifndef NL80211_EXT_FEATURE_SPP_AMSDU_SUPPORT
		#define NL80211_EXT_FEATURE_SPP_AMSDU_SUPPORT 70
		#endif
		wiphy_ext_feature_set(wiphy, NL80211_EXT_FEATURE_SPP_AMSDU_SUPPORT);
	}

	wiphy_ext_feature_set(wiphy, NL80211_EXT_FEATURE_TXQS);
	wiphy_ext_feature_set(wiphy, NL80211_EXT_FEATURE_RRM);

	wiphy->bss_priv_size = sizeof(struct ieee80211_bss);

	local = wiphy->priv;
	director.ieee80211_local = (struct ieee80211_local *)malloc(sizeof(struct ieee80211_local));
	local = director.ieee80211_local;

	if (sta_info_init(local)){
		goto err_free;
	}

	hw = &local->hw;

	hw->wiphy = wiphy;
	hw->priv = (char *)local + ALIGN(sizeof(*local), 32); /** 'NETDEV_ALIGN' = 32 */

	local->ieee80211_ops = ieee80211_ops;
	local->emulate_chanctx = emulate_chanctx;

	/*
	 * We need a bit of data queued to build aggregates properly, so
	 * instruct the TCP stack to allow more than a single ms of data
	 * to be queued in the stack. The value is a bit-shift of 1
	 * second, so 7 is ~8ms of queued data. Only affects local TCP
	 * sockets.
	 * This is the default, anyhow - drivers may need to override it
	 * for local reasons (longer buffers, longer completion time, or
	 * similar).
	 */
	hw->tx_sk_pacing_shift = 7;
	/* set up some defaults */
	hw->queues = 1;
	hw->max_rates = 1;
	hw->max_report_rates = 0;
	hw->max_rx_aggregation_subframes = IEEE80211_MAX_AMPDU_BUF_HT;
	hw->max_tx_aggregation_subframes = IEEE80211_MAX_AMPDU_BUF_HT;
	hw->offchannel_tx_hw_queue = IEEE80211_INVAL_HW_QUEUE;
	hw->conf.long_frame_max_tx_count = wiphy->retry_long;
	hw->conf.short_frame_max_tx_count = wiphy->retry_short;
	hw->radiotap_mcs_details = IEEE80211_RADIOTAP_MCS_HAVE_MCS |
					 IEEE80211_RADIOTAP_MCS_HAVE_GI |
					 IEEE80211_RADIOTAP_MCS_HAVE_BW;
	hw->radiotap_vht_details = IEEE80211_RADIOTAP_VHT_KNOWN_GI |
					 IEEE80211_RADIOTAP_VHT_KNOWN_BANDWIDTH;
	hw->uapsd_queues = IEEE80211_DEFAULT_UAPSD_QUEUES;
	hw->uapsd_max_sp_len = IEEE80211_DEFAULT_MAX_SP_LEN;
	hw->max_mtu = IEEE80211_MAX_DATA_LEN;
	local->ext_capa[7] = WLAN_EXT_CAPA8_OPMODE_NOTIF;

	INIT_LIST_HEAD(&local->interfaces);
	INIT_LIST_HEAD(&local->mon_list);

	pthread_mutex_init(&local->iflist_mtx, NULL);
	local->airtime_flags = AIRTIME_USE_TX | AIRTIME_USE_RX;
	local->aql_threshold = IEEE80211_AQL_THRESHOLD;
	atomic_store(&local->aql_total_pending_airtime, 0);
	INIT_LIST_HEAD(&local->chanctx_list);

	for (int i = 0; i < IEEE80211_MAX_QUEUES; i++) {
		skb_queue_head_init(&local->pending[i]);
		atomic_store(&local->agg_queue_stop[i], 0);
	}

	skb_queue_head_init(&local->skb_queue);
	skb_queue_head_init(&local->skb_queue_unreliable);

	return hw;
err_free:
	wiphy_free(wiphy);
	return NULL;
}
