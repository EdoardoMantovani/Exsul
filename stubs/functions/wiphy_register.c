int wiphy_register(struct wiphy *wiphy){
	int ret = 0;
	int i = 0;
	u16 ifmodes = 0;
	bool have_band = false;
	enum nl80211_band band = 0;
	struct ieee80211_supported_band *sband = NULL;

	struct ieee80211_local *local = NULL;
	struct cfg80211_registered_device *rdev = NULL;
	struct ieee80211_ops *ops = NULL;

	local = &director.ieee80211_local;
	if(local == NULL){

	}
	rdev = &director.ieee80211_local->rdev;
	if(rdev == NULL){

	}
	ops = local->ops;
	if(ops == NULL){
		ret = -EINVAL;
		return ret;
	}

	ifmodes = wiphy->interface_modes;

	if ((wiphy->interface_modes & BIT(NL80211_IFTYPE_NAN)) && (!ops->start_nan || !ops->stop_nan || !ops->add_nan_func || !ops->del_nan_func || !(wiphy->nan_supported_bands & BIT(NL80211_BAND_2GHZ)))){
		ret = -EINVAL;
		return ret;
	}

	if (wiphy->interface_modes & BIT(NL80211_IFTYPE_WDS)){
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
			return -EINVAL;
		}

		if (wiphy->addresses && !wiphy->n_addresses){
			return -EINVAL;
		}

		if (wiphy->max_acl_mac_addrs && (!(wiphy->flags & WIPHY_FLAG_HAVE_AP_SME) || !ops->set_mac_acl)){
			return -EINVAL;
		}

		/* sanity check supported bands/channels */
		for (band = 0; band < NUM_NL80211_BANDS; band++) {
			const struct ieee80211_sband_iftype_data *iftd;
			u16 types = 0;
			bool have_he = false;

			sband = wiphy->bands + sizeof(struct ieee80211_supported_band) * band;
			if (sband == NULL){
				continue;
			}
			sband->band = band;
			if (!sband->n_channels){
				return -EINVAL;
			}
			/*
			 * on 60GHz or sub-1Ghz band, there are no legacy rates, so
			 * n_bitrates is 0
			 */
			if ((band != NL80211_BAND_60GHZ && band != NL80211_BAND_S1GHZ) && !sband->n_bitrates){
				return -EINVAL;
			}

			if (band == NL80211_BAND_6GHZ && (sband->ht_cap.ht_supported || sband->vht_cap.vht_supported)){
				return -EINVAL;
			}

			/*
			 * Since we use a u32 for rate bitmaps in
			 * ieee80211_get_response_rate, we cannot
			 * have more than 32 legacy rates.
			 */
			if (sband->n_bitrates > 32){
				return -EINVAL;
			}

			for (i = 0; i < sband->n_channels; i++) {
				sband->channels[i].orig_flags = sband->channels[i].flags;
				sband->channels[i].orig_mag = INT_MAX;
				sband->channels[i].orig_mpwr = sband->channels[i].max_power;
				sband->channels[i].band = band;

				if (sband->channels[i].freq_offset >= 1000){
					return -EINVAL;
				}
			}

			for_each_sband_iftype_data(sband, i, iftd) {
				bool has_ap, has_non_ap;
				u32 ap_bits = BIT(NL80211_IFTYPE_AP) | BIT(NL80211_IFTYPE_P2P_GO);

				if (!iftd->types_mask){
					return -EINVAL;
				}
				if (types & iftd->types_mask){
					return -EINVAL;
				}
				/* at least one piece of information must be present */
				if (!iftd->he_cap.has_he){
					return -EINVAL;
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
					return -EINVAL;
				}
			}

			if (!have_he && band == NL80211_BAND_6GHZ){
				return -EINVAL;
			}
			have_band = true;
		}
	}
	return ret;
}
