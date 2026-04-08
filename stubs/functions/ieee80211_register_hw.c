int ieee80211_register_hw(struct ieee80211_hw *hw) {
	int ret = 0;
	int i = 0;
	int channels = 0;
	int max_bitrates = 0;
	bool supp_ht = false;
	bool supp_vht = false;
	bool supp_he = false;
	bool supp_eht = false;
	bool supp_s1g = false;
	enum nl80211_band band;
	struct wiphy *wiphy = NULL;
	struct ieee80211_ops *ops = NULL;
	struct ieee80211_local *local = NULL;
	struct cfg80211_chan_def dflt_chandef = {};

	local = director.ieee80211_local;
	if (local == NULL){
		ret = -EINVAL;
		return ret;
	}
	ops = local->ieee80211_ops;
	if (ops == NULL){
		ret = -EINVAL;
		return ret;
	}
	wiphy = local->wiphy;
	if (wiphy == NULL){
		ret = -EINVAL;
		return ret;
	}
	hw = local->ieee80211_hw;
	if (hw == NULL){
		ret = -EINVAL;
		return ret;
	}
	if (ieee80211_hw_check(hw, QUEUE_CONTROL) &&
		(hw->offchannel_tx_hw_queue == IEEE80211_INVAL_HW_QUEUE || hw->offchannel_tx_hw_queue >= hw->queues)){
		ret = -EINVAL;
		return ret;
	}
	if ((wiphy->features & NL80211_FEATURE_TDLS_CHANNEL_SWITCH) &&
		(!ops->tdls_channel_switch || !ops->tdls_cancel_channel_switch ||!ops->tdls_recv_channel_switch)){
		ret = -EOPNOTSUPP;
		return ret;
	}

	if (wiphy->interface_modes & BIT(NL80211_IFTYPE_NAN) && (!ops->start_nan || !ops->stop_nan)){
		ret = -EINVAL;
		return ret;
	}

	if (wiphy->flags & WIPHY_FLAG_SUPPORTS_MLO) {
		if (local->emulate_chanctx){
			ret = -EINVAL;
			return ret;
		}
		if (!ops->link_info_changed){
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
		if (ieee80211_hw_check(hw, SUPPORTS_PS) &&
		    (!ieee80211_hw_check(hw, SUPPORTS_DYNAMIC_PS) ||
		      ieee80211_hw_check(hw, PS_NULLFUNC_STACK))){
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

	if (hw->netdev_features & ~MAC80211_SUPPORTED_FEATURES){
		ret = -EINVAL;
		return ret;
	}

	if (!hw->max_report_rates){
		hw->max_report_rates = hw->max_rates;
	}
	local->rx_chains = 1;

	for (band = 0; band < NUM_NL80211_BANDS; band++) {
		const struct ieee80211_sband_iftype_data *iftd;
		struct ieee80211_supported_band *sband = wiphy->bands[band];

		if (sband == NULL){
			continue;
		}
		if (!dflt_chandef.chan) {
			for (i = 0; i < sband->n_channels; i++) {
				if (!(sband->channels[i].flags &
				      (IEEE80211_CHAN_DISABLED | IEEE80211_CHAN_S1G_NO_PRIMARY))){
					break;
				}
			}
			if (i == sband->n_channels){
				i = 0;
			}
			cfg80211_chandef_create(&dflt_chandef, &sband->channels[i], NL80211_CHAN_NO_HT);
			local->monitor_chanreq.oper = dflt_chandef;
			if (local->emulate_chanctx) {
				local->dflt_chandef = dflt_chandef;
				hw->conf.chandef = dflt_chandef;
			}
		}

		channels += sband->n_channels;

		if (sband->ht_cap.ht_supported &&
		    (sband->ht_cap.cap & IEEE80211_HT_CAP_DELAY_BA) &&
		    (wiphy->flags & WIPHY_FLAG_SUPPORTS_MLO)){
			ret = -EINVAL;
			return ret;
		}

		if (max_bitrates < sband->n_bitrates){
			max_bitrates = sband->n_bitrates;
		}
		supp_ht  = supp_ht  || sband->ht_cap.ht_supported;
		supp_vht = supp_vht || sband->vht_cap.vht_supported;
		supp_s1g = supp_s1g || sband->s1g_cap.s1g;

		for_each_sband_iftype_data(sband, i, iftd) {
			supp_he  = supp_he  || iftd->he_cap.has_he;
			supp_eht = supp_eht || iftd->eht_cap.has_eht;
		}

		if (hw->queues < IEEE80211_NUM_ACS && (supp_ht || supp_vht || supp_he)){
			ret = -EINVAL;
			return ret;
		}
		if (supp_eht && !supp_he){
			ret = -EINVAL;
			return ret;
		}
		if (!sband->ht_cap.ht_supported){
			continue;
		}
		local->rx_chains = max(ieee80211_mcs_to_chains(&sband->ht_cap.mcs),
				       local->rx_chains);

		sband->ht_cap.cap |= WLAN_HT_CAP_SM_PS_DISABLED << IEEE80211_HT_CAP_SM_PS_SHIFT;
	}

	if (wiphy->interface_modes & BIT(NL80211_IFTYPE_AP) &&
	    !ieee80211_hw_check(hw, SW_CRYPTO_CONTROL)) {
		wiphy->interface_modes |= BIT(NL80211_IFTYPE_AP_VLAN);
		wiphy->software_iftypes |= BIT(NL80211_IFTYPE_AP_VLAN);
	}

	wiphy->interface_modes |= BIT(NL80211_IFTYPE_MONITOR);
	wiphy->software_iftypes |= BIT(NL80211_IFTYPE_MONITOR);

	local->int_scan_req = calloc(1, sizeof(struct cfg80211_scan_request) +
				        sizeof(struct cfg80211_scan_request) * channels);
	if (local->int_scan_req == NULL){
		ret = -ENOMEM;
		return ret;
	}

	eth_broadcast_addr(local->int_scan_req->bssid);

	for (band = 0; band < NUM_NL80211_BANDS; band++) {
		if (!wiphy->bands[band]){
			continue;
		}
		local->int_scan_req->rates[band] = (u32)-1;
	}

	if (wiphy->interface_modes & BIT(NL80211_IFTYPE_MESH_POINT)){
		wiphy->flags |= WIPHY_FLAG_MESH_AUTH;
	}

	wiphy->flags |= WIPHY_FLAG_CONTROL_PORT_PROTOCOL;

	if (ieee80211_hw_check(hw, SIGNAL_DBM)) {
		wiphy->signal_type = CFG80211_SIGNAL_TYPE_MBM;
	} else if (ieee80211_hw_check(hw, SIGNAL_UNSPEC)) {
		wiphy->signal_type = CFG80211_SIGNAL_TYPE_UNSPEC;
		if (hw->max_signal <= 0) {
			ret = -EINVAL;
			goto fail_workqueue;
		}
	}

	if (!ops->set_key) {
		wiphy_ext_feature_set(wiphy, NL80211_EXT_FEATURE_CAN_REPLACE_PTK0);
		wiphy_ext_feature_set(wiphy, NL80211_EXT_FEATURE_EXT_KEY_ID);
	}

	if (wiphy->interface_modes & BIT(NL80211_IFTYPE_ADHOC)){
		wiphy_ext_feature_set(wiphy, NL80211_EXT_FEATURE_DEL_IBSS_STA);
	}

	local->scan_ies_len = 4 + max_bitrates + 3;
	if (supp_ht){
		local->scan_ies_len += 2 + sizeof(struct ieee80211_ht_cap);
	}
	if (supp_vht){
		local->scan_ies_len += 2 + sizeof(struct ieee80211_vht_cap);
	}
	if (supp_s1g){
		local->scan_ies_len += 2 + sizeof(struct ieee80211_s1g_cap);
	}
	if (supp_he) {
		local->scan_ies_len += 3 + sizeof(struct ieee80211_he_cap_elem) +
				       sizeof(struct ieee80211_he_mcs_nss_supp) +
				       IEEE80211_HE_PPE_THRES_MAX_LEN;
		if (supp_eht){
			local->scan_ies_len += 3 + sizeof(struct ieee80211_eht_cap_elem) +
					       sizeof(struct ieee80211_eht_mcs_nss_supp) +
					       IEEE80211_EHT_PPE_THRES_MAX_LEN;
		}
	}

	if (!ops->hw_scan) {
		wiphy->max_scan_ssids = 4;
		wiphy->max_scan_ie_len = IEEE80211_MAX_DATA_LEN;
	}

	if (wiphy->max_scan_ie_len){
		wiphy->max_scan_ie_len -= local->scan_ies_len;
	}

	ret = ieee80211_init_cipher_suites(local);
	if (ret < 0){
		goto fail_workqueue;
	}
	if (!ops->remain_on_channel){
		wiphy->max_remain_on_channel_duration = 5000;
	}
	if (wiphy->flags & WIPHY_FLAG_SUPPORTS_TDLS){
		wiphy->flags |= WIPHY_FLAG_TDLS_EXTERNAL_SETUP;
	}
	if (ieee80211_hw_check(hw, CHANCTX_STA_CSA)){
		local->ext_capa[0] |= WLAN_EXT_CAPA1_EXT_CHANNEL_SWITCHING;
	}
	wiphy->max_num_csa_counters = IEEE80211_MAX_CNTDWN_COUNTERS_NUM;

	if (hw->queues > IEEE80211_MAX_QUEUES){
		hw->queues = IEEE80211_MAX_QUEUES;
	}
	local->workqueue = alloc_workqueue(wiphy_name(wiphy), WQ_UNBOUND | __WQ_ORDERED, 1);
	if (local->workqueue == NULL) {
		ret = -ENOMEM;
		goto fail_workqueue;
	}

	local->tx_headroom = max_t(unsigned int, wiphy->extra_tx_headroom, IEEE80211_TX_STATUS_HEADROOM);

	if (!hw->max_listen_interval){
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
	ret = ieee80211_txq_setup_flows(local);
	if (ret){
		goto fail_flows;
	}
	ret = ieee80211_init_rate_ctrl_alg(local, hw->rate_control_algorithm);
	if (ret < 0) {
		printf("[!][wiphy] Failed to initialize rate control algorithm\n");
		goto fail_rate;
	}

	for (band = 0; band < NUM_NL80211_BANDS; band++) {
		struct ieee80211_supported_band *sband = NULL;
		bool local_cap = false;
		bool ie_cap = false;

		local_cap = ieee80211_hw_check(hw, SUPPORTS_VHT_EXT_NSS_BW);
		sband = wiphy->bands[band];
		if (sband == NULL || !sband->vht_cap.vht_supported){
			continue;
		}
		ie_cap = !!(sband->vht_cap.vht_mcs.tx_highest & cpu_to_le16(IEEE80211_VHT_EXT_NSS_BW_CAPABLE));
		if (local_cap == ie_cap){
			continue;
		}
		sband = memdup(sband, sizeof(*sband));
		if (sband == NULL) {
			ret = -ENOMEM;
			goto fail_rate;
		}

		sband->vht_cap.vht_mcs.tx_highest ^= cpu_to_le16(IEEE80211_VHT_EXT_NSS_BW_CAPABLE);
		local->sband_allocated |= BIT(band);
	}
	ret = wiphy_register(wiphy);
	if (ret < 0){
		goto fail_wiphy_register;
	}
	debugfs_hw_add(local);
	rate_control_add_debugfs(local);
	ieee80211_check_wbrf_support(local);

	if (wiphy->interface_modes & BIT(NL80211_IFTYPE_STATION) && !ieee80211_hw_check(hw, NO_AUTO_VIF)) {
		struct vif_params params = {0};

		ret = ieee80211_if_add(local, "wlan%d", 1, NULL, NL80211_IFTYPE_STATION, &params);
		if (ret){
			printf("[!] Failed to add default virtual iface\n");
		}
	}

	return ret;

 fail_wiphy_register:
	//wiphy_unregister(wiphy);
	rate_control_deinitialize(local);
	ieee80211_remove_interfaces(local);
 fail_rate:
	ieee80211_txq_teardown_flows(local);
 fail_flows:
	ieee80211_led_exit(local);
	destroy_workqueue(local->workqueue);
 fail_workqueue:
	kfree(local->int_scan_req);
	return ret;
}
