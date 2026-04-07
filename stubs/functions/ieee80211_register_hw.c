int ieee80211_register_hw(struct ieee80211_hw *hw){
	int ret = 0;
	int i = 0;
	int result = 0;
	int channels = 0;
	int max_bitrates = 0;
	bool supp_ht = false;
	bool supp_vht = false;
	bool supp_he  = false;
	bool supp_eht = false;
	bool supp_s1g = false;
	enum nl80211_band band = NL80211_BAND_2GHZ;
	struct cfg80211_chan_def dflt_chandef = {};
	struct ieee80211_local *local = NULL;
	struct ieee80211_ops *ieee80211_ops = NULL;
	struct wiphy *wiphy = NULL;

	local = director.ieee80211_local;
	if(local == NULL){
		ret = -EINVAL;
		return ret;
	}

	ieee80211_ops = local->ieee80211_ops;
	if(ieee80211_ops == NULL){
		ret = -EINVAL;
		return ret;
	}

	wiphy = local->wiphy;
	if(wiphy == NULL){
		ret = -EINVAL;
		return ret;
	}

	hw = local->ieee80211_hw;
	if(hw == NULL){
		ret = -EINVAL;
		return ret;
	}
	printf("ieee80211_register_hw4\n");
	if (ieee80211_hw_check(hw, QUEUE_CONTROL) && (hw->offchannel_tx_hw_queue == IEEE80211_INVAL_HW_QUEUE || hw->offchannel_tx_hw_queue >= hw->queues)){
		ret = -EINVAL;
		return ret;
	}
	printf("ieee80211_register_hw5\n");
	if ((wiphy->features & NL80211_FEATURE_TDLS_CHANNEL_SWITCH) && (ieee80211_ops->tdls_channel_switch == NULL || ieee80211_ops->tdls_cancel_channel_switch == NULL || ieee80211_ops->tdls_recv_channel_switch == NULL)){
		ret = -EOPNOTSUPP;
		return ret;
	}
	printf("ieee80211_register_hw6\n");
	if (ieee80211_hw_check(hw, SUPPORTS_TX_FRAG) && ieee80211_ops->set_frag_threshold == NULL){
		ret = -EINVAL;
		return ret;
	}

	if (wiphy->interface_modes & BIT(NL80211_IFTYPE_NAN) && (ieee80211_ops->start_nan == NULL || ieee80211_ops->stop_nan == NULL)){
		ret = -EINVAL;
		return ret;
	}

	if (wiphy->flags & WIPHY_FLAG_SUPPORTS_MLO) {
		/*
		 * For drivers capable of doing MLO, assume modern driver
		 * or firmware facilities, so software doesn't have to do
		 * as much, e.g. monitoring beacons would be hard if we
		 * might not even know which link is active at which time.
		 */
		if (local->emulate_chanctx){
			ret = -EINVAL;
			return ret;
		}

		if (ieee80211_ops->link_info_changed == NULL){
			ret = -EINVAL;
			return ret;
		}

		if (!ieee80211_hw_check(hw, HAS_RATE_CONTROL)){
			ret = -EINVAL;
			return ret;
		}

		if (!ieee80211_hw_check(hw, AMPDU_AGGREGATION)){
			ret = -EINVAL;
			return ret;
		}

		if (!ieee80211_hw_check(hw, HOST_BROADCAST_PS_BUFFERING)){
			ret = -EINVAL;
			return ret;
		}
		if (ieee80211_hw_check(hw, SUPPORTS_PS) && (!ieee80211_hw_check(hw, SUPPORTS_DYNAMIC_PS) || ieee80211_hw_check(hw, PS_NULLFUNC_STACK))){
			ret = -EINVAL;
			return ret;
		}

		if (!ieee80211_hw_check(hw, MFP_CAPABLE)){
			ret = -EINVAL;
			return ret;
		}

		if (ieee80211_hw_check(hw, NEED_DTIM_BEFORE_ASSOC)){
			ret = -EINVAL;
			return ret;
		}

		if (ieee80211_hw_check(hw, TIMING_BEACON_ONLY)){
			ret = -EINVAL;
			return ret;
		}

		if (!ieee80211_hw_check(hw, AP_LINK_PS)){
			ret = -EINVAL;
			return ret;
		}
	}

	/* for now deactivate the 'wiphy radio' component */
	#if 0
	if (wiphy->n_radio) {
		for (i = 0; i < wiphy->n_radio; i++) {
			const struct wiphy_radio *radio = &wiphy->radio[i];

			if (!ieee80211_ifcomb_check(radio->iface_combinations, radio->n_iface_combinations)){
				ret = -EINVAL;
				return ret;
			}
		}
	}else{
		if (!ieee80211_ifcomb_check(wiphy->iface_combinations, wiphy->n_iface_combinations)){
			ret = -EINVAL;
			return ret;
		}
	}
	#endif

	/* Only HW csum features are currently compatible with mac80211 */
	if (hw->netdev_features & ~MAC80211_SUPPORTED_FEATURES){
		ret = -EINVAL;
		return ret;
	}

	if (hw->max_report_rates == 0){
		hw->max_report_rates = hw->max_rates;
	}

	local->rx_chains = 1;

	/*
	 * generic code guarantees at least one band,
	 * set this very early because much code assumes
	 * that hw.conf.channel is assigned
	 */
	channels = 0;
	max_bitrates = 0;
	supp_ht = false;
	supp_vht = false;
	supp_he = false;
	supp_eht = false;
	supp_s1g = false;
	for (band = 0; band < NUM_NL80211_BANDS; band++) {
		const struct ieee80211_sband_iftype_data *iftd = NULL;
		struct ieee80211_supported_band *sband = NULL;

		sband = wiphy->bands + sizeof(struct ieee80211_supported_band) * band;
		if (!sband){
			continue;
		}

		if (!dflt_chandef.chan) {
			/*
			 * Assign the first enabled channel to dflt_chandef
			 * from the list of channels. For S1G interfaces
			 * ensure it can be used as a primary.
			 */
			for (i = 0; i < sband->n_channels; i++){
				if (!(sband->channels[i].flags & (IEEE80211_CHAN_DISABLED | IEEE80211_CHAN_S1G_NO_PRIMARY))){
					break;
				}
			}
			/* if none found then use the first anyway */
			if (i == sband->n_channels){
				i = 0;
			}

			cfg80211_chandef_create(&dflt_chandef, &sband->channels[i], NL80211_CHAN_NO_HT);
			/* init channel we're on */
			local->monitor_chanreq.oper = dflt_chandef;
			if (local->emulate_chanctx) {
				local->dflt_chandef = dflt_chandef;
				hw->conf.chandef = dflt_chandef;
			}
		}

		channels += sband->n_channels;

		/*
		 * Due to the way the aggregation code handles this and it
		 * being an HT capability, we can't really support delayed
		 * BA in MLO (yet).
		 */
		if (sband->ht_cap.ht_supported && (sband->ht_cap.cap & IEEE80211_HT_CAP_DELAY_BA) && wiphy->flags & WIPHY_FLAG_SUPPORTS_MLO){
			ret = -EINVAL;
			return ret;
		}

		if (max_bitrates < sband->n_bitrates){
			max_bitrates = sband->n_bitrates;
		}

		supp_ht = supp_ht || sband->ht_cap.ht_supported;
		supp_vht = supp_vht || sband->vht_cap.vht_supported;
		supp_s1g = supp_s1g || sband->s1g_cap.s1g;

		for_each_sband_iftype_data(sband, i, iftd) {
			u8 he_40_mhz_cap;

			supp_he = supp_he || iftd->he_cap.has_he;
			supp_eht = supp_eht || iftd->eht_cap.has_eht;

			if (band == NL80211_BAND_2GHZ){
				he_40_mhz_cap = IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_40MHZ_IN_2G;
			}else{
				he_40_mhz_cap = IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_40MHZ_80MHZ_IN_5G;
			}
			/* currently no support for HE client where HT has 40 MHz but not HT */
			if (iftd->he_cap.has_he && iftd->types_mask & (BIT(NL80211_IFTYPE_STATION) | BIT(NL80211_IFTYPE_P2P_CLIENT)) &&
			    sband->ht_cap.ht_supported && sband->ht_cap.cap & IEEE80211_HT_CAP_SUP_WIDTH_20_40 && !(iftd->he_cap.he_cap_elem.phy_cap_info[0] & he_40_mhz_cap)){
				ret = -EINVAL;
				return ret;
			}

			/* no support for per-band vendor elems with MLO */
			if (iftd->vendor_elems.len && (wiphy->flags & WIPHY_FLAG_SUPPORTS_MLO)){
				ret = -EINVAL;
				return ret;
			}
		}

		/* HT, VHT, HE require QoS, thus >= 4 queues */
		if (hw->queues < IEEE80211_NUM_ACS && (supp_ht || supp_vht || supp_he)){
			ret = -EINVAL;
			return ret;
		}

		/* EHT requires HE support */
		if (supp_eht && !supp_he){
			ret = -EINVAL;
			return ret;
		}

		if (!sband->ht_cap.ht_supported){
			continue;
		}

		/* TODO: consider VHT for RX chains, hopefully it's the same */
		local->rx_chains =
			max(ieee80211_mcs_to_chains(&sband->ht_cap.mcs),
			    local->rx_chains);

		/* no need to mask, SM_PS_DISABLED has all bits set */
		sband->ht_cap.cap |= WLAN_HT_CAP_SM_PS_DISABLED <<
			             IEEE80211_HT_CAP_SM_PS_SHIFT;
	}

	/* if low-level driver supports AP, we also support VLAN.
	 * drivers advertising SW_CRYPTO_CONTROL should enable AP_VLAN
	 * based on their support to transmit SW encrypted packets.
	 */
	if (wiphy->interface_modes & BIT(NL80211_IFTYPE_AP) &&
	    !ieee80211_hw_check(hw, SW_CRYPTO_CONTROL)) {
		wiphy->interface_modes |= BIT(NL80211_IFTYPE_AP_VLAN);
		wiphy->software_iftypes |= BIT(NL80211_IFTYPE_AP_VLAN);
	}

	/* mac80211 always supports monitor */
	wiphy->interface_modes |= BIT(NL80211_IFTYPE_MONITOR);
	wiphy->software_iftypes |= BIT(NL80211_IFTYPE_MONITOR);


	local->int_scan_req = malloc(sizeof(struct cfg80211_scan_request) + (sizeof(struct cfg80211_scan_request) * channels));
	if (!local->int_scan_req){
		ret = -ENOMEM;
		return ret;
	}

	eth_broadcast_addr(local->int_scan_req->bssid);

	for (band = 0; band < NUM_NL80211_BANDS; band++) {
		if (!wiphy->bands + (sizeof(struct ieee80211_supported_band) * band)){
			continue;
		}
		local->int_scan_req->rates[band] = (u32) -1;
	}

	/* if the underlying driver supports mesh, mac80211 will (at least)
	 * provide routing of mesh authentication frames to userspace */
	if (wiphy->interface_modes & BIT(NL80211_IFTYPE_MESH_POINT)){
		wiphy->flags |= WIPHY_FLAG_MESH_AUTH;
	}

	/* mac80211 supports control port protocol changing */
	wiphy->flags |= WIPHY_FLAG_CONTROL_PORT_PROTOCOL;

	if (ieee80211_hw_check(hw, SIGNAL_DBM)) {
		wiphy->signal_type = CFG80211_SIGNAL_TYPE_MBM;
	} else if (ieee80211_hw_check(hw, SIGNAL_UNSPEC)) {
		wiphy->signal_type = CFG80211_SIGNAL_TYPE_UNSPEC;
		if (hw->max_signal <= 0) {
			result = -EINVAL;
			goto fail_workqueue;
		}
	}

	/* Mac80211 and therefore all drivers using SW crypto only
	 * are able to handle PTK rekeys and Extended Key ID.
	 */
	if (ieee80211_ops->set_key == NULL) {
		wiphy_ext_feature_set(wiphy, NL80211_EXT_FEATURE_CAN_REPLACE_PTK0);
		wiphy_ext_feature_set(wiphy, NL80211_EXT_FEATURE_EXT_KEY_ID);
	}

	if (wiphy->interface_modes & BIT(NL80211_IFTYPE_ADHOC)){
		wiphy_ext_feature_set(wiphy, NL80211_EXT_FEATURE_DEL_IBSS_STA);
	}

	/*
	 * Calculate scan IE length -- we need this to alloc
	 * memory and to subtract from the driver limit. It
	 * includes the DS Params, (extended) supported rates, and HT
	 * information -- SSID is the driver's responsibility.
	 */
	local->scan_ies_len = 4 + max_bitrates /* (ext) supp rates */ + 3 /* DS Params */;
	if (supp_ht){
		local->scan_ies_len += 2 + sizeof(struct ieee80211_ht_cap);
	}

	if (supp_vht){
		local->scan_ies_len += 2 + sizeof(struct ieee80211_vht_cap);
	}

	if (supp_s1g){
		local->scan_ies_len += 2 + sizeof(struct ieee80211_s1g_cap);
	}

	/*
	 * HE cap element is variable in size - set len to allow max size */
	if (supp_he) {
		local->scan_ies_len +=
			3 + sizeof(struct ieee80211_he_cap_elem) +
			sizeof(struct ieee80211_he_mcs_nss_supp) +
			IEEE80211_HE_PPE_THRES_MAX_LEN;

		if (supp_eht){
			local->scan_ies_len +=
				3 + sizeof(struct ieee80211_eht_cap_elem) +
				sizeof(struct ieee80211_eht_mcs_nss_supp) +
				IEEE80211_EHT_PPE_THRES_MAX_LEN;
		}
	}

	if (ieee80211_ops->hw_scan == NULL) {
		/* For hw_scan, driver needs to set these up. */
		wiphy->max_scan_ssids = 4;
		wiphy->max_scan_ie_len = IEEE80211_MAX_DATA_LEN;
	}

	/*
	 * If the driver supports any scan IEs, then assume the
	 * limit includes the IEs mac80211 will add, otherwise
	 * leave it at zero and let the driver sort it out; we
	 * still pass our IEs to the driver but userspace will
	 * not be allowed to in that case.
	 */
	if (wiphy->max_scan_ie_len){
		wiphy->max_scan_ie_len -= local->scan_ies_len;
	}

	result = ieee80211_init_cipher_suites(local);
	if (result < 0){
		goto fail_workqueue;
	}

	if (ieee80211_ops->remain_on_channel == NULL){
		wiphy->max_remain_on_channel_duration = 5000;
	}

	/* mac80211 based drivers don't support internal TDLS setup */
	if (wiphy->flags & WIPHY_FLAG_SUPPORTS_TDLS){
		wiphy->flags |= WIPHY_FLAG_TDLS_EXTERNAL_SETUP;
	}

	/* mac80211 supports eCSA, if the driver supports STA CSA at all */
	if (ieee80211_hw_check(hw, CHANCTX_STA_CSA)){
		local->ext_capa[0] |= WLAN_EXT_CAPA1_EXT_CHANNEL_SWITCHING;
	}

	/* mac80211 supports multi BSSID, if the driver supports it */
	if (ieee80211_hw_check(hw, SUPPORTS_MULTI_BSSID)) {
		wiphy->support_mbssid = true;
		if (ieee80211_hw_check(hw, SUPPORTS_ONLY_HE_MULTI_BSSID)){
			wiphy->support_only_he_mbssid = true;
		}else{
			local->ext_capa[2] |= WLAN_EXT_CAPA3_MULTI_BSSID_SUPPORT;
		}
	}

	wiphy->max_num_csa_counters = IEEE80211_MAX_CNTDWN_COUNTERS_NUM;

	/*
	 * We use the number of queues for feature tests (QoS, HT) internally
	 * so restrict them appropriately.
	 */
	if (hw->queues > IEEE80211_MAX_QUEUES){
		hw->queues = IEEE80211_MAX_QUEUES;
	}

	local->workqueue = alloc_workqueue(wiphy_name(wiphy), WQ_UNBOUND | __WQ_ORDERED, 1);
	if (!local->workqueue) {
		result = -ENOMEM;
		goto fail_workqueue;
	}

	/*
	 * The hardware needs headroom for sending the frame,
	 * and we need some headroom for passing the frame to monitor
	 * interfaces, but never both at the same time.
	 */
	local->tx_headroom = max_t(unsigned int , wiphy->extra_tx_headroom, IEEE80211_TX_STATUS_HEADROOM);

	/*
	 * if the driver doesn't specify a max listen interval we
	 * use 5 which should be a safe default
	 */
	if (hw->max_listen_interval == 0){
		hw->max_listen_interval = 5;
	}

	hw->conf.listen_interval = hw->max_listen_interval;

	local->dynamic_ps_forced_timeout = -1;

	if (!hw->max_nan_de_entries){
		hw->max_nan_de_entries = IEEE80211_MAX_NAN_INSTANCE_ID;
	}

	if (!hw->weight_multiplier){
		hw->weight_multiplier = 1;
	}

	ieee80211_wep_init(local);

	hw->conf.flags = IEEE80211_CONF_IDLE;

	ieee80211_led_init(local);

	result = ieee80211_txq_setup_flows(local);
	if (result){
		goto fail_flows;
	}

	result = ieee80211_init_rate_ctrl_alg(local, hw->rate_control_algorithm);

	if (result < 0) {
		printf("[!][wiphy] Failed to initialize rate control algorithm\n");
		goto fail_rate;
	}

	/*
	 * If the VHT capabilities don't have IEEE80211_VHT_EXT_NSS_BW_CAPABLE,
	 * or have it when we don't, copy the sband structure and set/clear it.
	 * This is necessary because rate scaling algorithms could be switched
	 * and have different support values.
	 * Print a message so that in the common case the reallocation can be
	 * avoided.
	 */
	for (band = 0; band < NUM_NL80211_BANDS; band++) {
		struct ieee80211_supported_band *sband;
		bool local_cap, ie_cap;

		local_cap = ieee80211_hw_check(hw, SUPPORTS_VHT_EXT_NSS_BW);

		sband = wiphy->bands + (sizeof(struct ieee80211_supported_band) * band);
		if (!sband || !sband->vht_cap.vht_supported){
			continue;
		}

		ie_cap = !!(sband->vht_cap.vht_mcs.tx_highest & cpu_to_le16(IEEE80211_VHT_EXT_NSS_BW_CAPABLE));

		if (local_cap == ie_cap){
			continue;
		}

		sband = memdup(sband, sizeof(*sband));
		if (!sband) {
			result = -ENOMEM;
			ret = -ENOMEM;
			goto fail_rate;
		}

		//wiphy_dbg(wiphy, "copying sband (band %d) due to VHT EXT NSS BW flag\n", band);

		sband->vht_cap.vht_mcs.tx_highest ^= cpu_to_le16(IEEE80211_VHT_EXT_NSS_BW_CAPABLE);

		// TODO!
		//wiphy->bands + sizeof(struct ieee80211_supported_band) * band = sband;
		local->sband_allocated |= BIT(band);
	}

	result = wiphy_register(wiphy);
	if (result < 0){
		goto fail_wiphy_register;
	}

	debugfs_hw_add(local);
	rate_control_add_debugfs(local);

	ieee80211_check_wbrf_support(local);

	wiphy_lock(wiphy);

	/* add one default STA interface if supported */
	if (wiphy->interface_modes & BIT(NL80211_IFTYPE_STATION) &&
	    !ieee80211_hw_check(hw, NO_AUTO_VIF)) {
		struct vif_params params = {0};

		result = ieee80211_if_add(local, "wlan%d", 1, NULL, NL80211_IFTYPE_STATION, &params);
		if (result){
			printf("[!] Failed to add default virtual iface\n");
		}
	}

	wiphy_unlock(wiphy);

	return 0;

	wiphy_unregister(wiphy);
 fail_wiphy_register:
	rate_control_deinitialize(local);
	ieee80211_remove_interfaces(local);
 fail_rate:
	ieee80211_txq_teardown_flows(local);
 fail_flows:
	ieee80211_led_exit(local);
	destroy_workqueue(local->workqueue);
 fail_workqueue:
	kfree(local->int_scan_req);
	return result;
}
