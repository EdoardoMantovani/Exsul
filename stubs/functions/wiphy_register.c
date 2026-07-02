int wiphy_register(struct wiphy *wiphy){
	int ret = 0;
	int i = 0;
	u16 ifmodes = 0;
	bool have_band = false;
	enum nl80211_band band = 0;
	struct ieee80211_local *local = NULL;
	struct regulatory_request *request = NULL;
	struct ieee80211_ops *ieee80211_ops = NULL;
	struct ieee80211_supported_band *sband = NULL;
	struct cfg80211_registered_device *rdev = NULL;
	void (*reg_notifier)(struct wiphy *wiphy, struct regulatory_request *request) = NULL;

	local = director.ieee80211_local;
	rdev = director.rdev;
	if(rdev == NULL){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] ieee80211_rdev is null!\n");
		#endif
		ret = -EINVAL;
		return ret;
	}
	if(local != NULL){
		ieee80211_ops = local->ieee80211_ops;
	}
	if(local != NULL && ieee80211_ops == NULL){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] ieee80211_ops is null!\n");
		#endif
		ret = -EINVAL;
		return ret;
	}

	ifmodes = wiphy->interface_modes;

	if (ieee80211_ops != NULL &&
	    (wiphy->interface_modes & BIT(NL80211_IFTYPE_NAN)) &&
	    (!ieee80211_ops->start_nan || !ieee80211_ops->stop_nan ||
	     !ieee80211_ops->add_nan_func || !ieee80211_ops->del_nan_func ||
	     !(wiphy->nan_supported_bands & BIT(NL80211_BAND_2GHZ)))){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] missing callbacks in ieee80211_ops!\n");
		#endif
		ret = -EINVAL;
		return ret;
	}

	if (wiphy->interface_modes & BIT(NL80211_IFTYPE_WDS)){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] wrong interface modes!\n");
		#endif
		ret = -EINVAL;
		return ret;
	}

	if (wiphy->pmsr_capa && wiphy->pmsr_capa->ftm.supported) {
		if (!wiphy->pmsr_capa->ftm.asap && !wiphy->pmsr_capa->ftm.non_asap){
			ret = -EINVAL;
			return ret;
		}

		if (!wiphy->pmsr_capa->ftm.preambles || !wiphy->pmsr_capa->ftm.bandwidths){
			ret = -EINVAL;
			return ret;
		}

		if (wiphy->pmsr_capa->ftm.preambles & ~(BIT(NL80211_PREAMBLE_LEGACY) | BIT(NL80211_PREAMBLE_HT) | BIT(NL80211_PREAMBLE_VHT) | BIT(NL80211_PREAMBLE_HE) | BIT(NL80211_PREAMBLE_DMG))){
			ret = -EINVAL;
			return ret;
		}

		if ((wiphy->pmsr_capa->ftm.trigger_based || wiphy->pmsr_capa->ftm.non_trigger_based) && !(wiphy->pmsr_capa->ftm.preambles & BIT(NL80211_PREAMBLE_HE))){
			ret = -EINVAL;
			return ret;
		}

		if (wiphy->ap_sme_capa && !(wiphy->flags & WIPHY_FLAG_HAVE_AP_SME)){
			ret = -EINVAL;
			return ret;
		}

		if (wiphy->addresses && !wiphy->n_addresses){
			ret = -EINVAL;
			return ret;
		}

		if (ieee80211_ops != NULL && wiphy->max_acl_mac_addrs &&
			    (!(wiphy->flags & WIPHY_FLAG_HAVE_AP_SME) || !ieee80211_ops->set_mac_acl)){
			ret = -EINVAL;
			return ret;
		}

		/* sanity check supported bands/channels */
		for (band = 0; band < NUM_NL80211_BANDS; band++) {
			const struct ieee80211_sband_iftype_data *iftd;
			u16 types = 0;
			bool have_he = false;

			sband = wiphy->bands[band];
			if (sband == NULL){
				continue;
			}
			sband->band = band;
			if (!sband->n_channels){
				ret = -EINVAL;
				return ret;
			}

			if ((band != NL80211_BAND_60GHZ && band != NL80211_BAND_S1GHZ) && !sband->n_bitrates){
				ret = -EINVAL;
				return ret;
			}

			if (band == NL80211_BAND_6GHZ && (sband->ht_cap.ht_supported || sband->vht_cap.vht_supported)){
				ret = -EINVAL;
				return ret;
			}

			if (sband->n_bitrates > 32){
				ret = -EINVAL;
				return ret;
			}

			for (i = 0; i < sband->n_channels; i++) {
				sband->channels[i].orig_flags = sband->channels[i].flags;
				sband->channels[i].orig_mag = INT_MAX;
				sband->channels[i].orig_mpwr = sband->channels[i].max_power;
				sband->channels[i].band = band;

				if (sband->channels[i].freq_offset >= 1000){
					ret = -EINVAL;
					return ret;
				}
			}

			for_each_sband_iftype_data(sband, i, iftd) {
				bool has_ap, has_non_ap;
				u32 ap_bits = BIT(NL80211_IFTYPE_AP) | BIT(NL80211_IFTYPE_P2P_GO);

				if (!iftd->types_mask){
					ret = -EINVAL;
					return ret;
				}
				if (types & iftd->types_mask){
					ret = -EINVAL;
					return ret;
				}
				/* at least one piece of information must be present */
				if (!iftd->he_cap.has_he){
					ret = -EINVAL;
					return ret;
				}
				types |= iftd->types_mask;

				if (i == 0){
					have_he = iftd->he_cap.has_he;
				}else{
					have_he = have_he && iftd->he_cap.has_he;
				}
				has_ap = iftd->types_mask & ap_bits;
				has_non_ap = iftd->types_mask & ~ap_bits;

				/*
				 * For EHT 20 MHz STA, the capabilities format differs
				 * but to simplify, don't check 20 MHz but rather check
				 * only if AP and non-AP were mentioned at the same time,
				 * reject if so.
				 */
				if (iftd->eht_cap.has_eht && has_ap && has_non_ap){
					ret = -EINVAL;
					return ret;
				}
			}

			if (!have_he && band == NL80211_BAND_6GHZ){
				ret = -EINVAL;
				return ret;
			}
			have_band = true;
		}
	}

	/* set up regulatory info */
	if (wiphy->reg_notifier != NULL) {
		request = NULL;
		if (local != NULL && !list_empty(&local->reg_requests_list)) {
			request = list_last_entry(&local->reg_requests_list, struct regulatory_request, list);
		}

		if (request == NULL) {
			request = __kmalloc_large_noprof(sizeof(struct regulatory_request), GFP_KERNEL);
			if (request == NULL) {
				#ifdef ENABLE_DBG_PRINT
				printf("[!] failed to allocate the regulatory_request!\n");
				#endif
				ret = -ENOMEM;
				return ret;
			}
			memset(request, 0, sizeof(*request));
			request->initiator   = NL80211_REGDOM_SET_BY_CORE;
			request->alpha2[0]   = '0';
			request->alpha2[1]   = '0';
			request->alpha2[2]   = '0';
		}

		reg_notifier = wiphy->reg_notifier;
		if(reg_notifier != NULL){
			reg_notifier(wiphy, request);
		}

		if (request != NULL){
			kfree(request);
		}
	}
	return ret;
}
