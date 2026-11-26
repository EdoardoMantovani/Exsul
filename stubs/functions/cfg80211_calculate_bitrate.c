u32 cfg80211_calculate_bitrate(struct rate_info *rate){
	int ret = 0;

	if (rate->flags & RATE_INFO_FLAGS_MCS){
		ret = cfg80211_calculate_bitrate_ht(rate);
		return ret;
	}
	if (rate->flags & RATE_INFO_FLAGS_DMG){
		ret = cfg80211_calculate_bitrate_dmg(rate);
		return ret;
	}
	if (rate->flags & RATE_INFO_FLAGS_EXTENDED_SC_DMG){
		ret = cfg80211_calculate_bitrate_extended_sc_dmg(rate);
		return ret;
	}
	if (rate->flags & RATE_INFO_FLAGS_EDMG){
		ret = cfg80211_calculate_bitrate_edmg(rate);
		return ret;
	}
	if (rate->flags & RATE_INFO_FLAGS_VHT_MCS){
		ret = cfg80211_calculate_bitrate_vht(rate);
		return ret;
	}
	if (rate->flags & RATE_INFO_FLAGS_HE_MCS){
		ret = cfg80211_calculate_bitrate_he(rate);
		return ret;
	}
	if (rate->flags & RATE_INFO_FLAGS_EHT_MCS){
		ret = cfg80211_calculate_bitrate_eht(rate);
		return ret;
	}
	if (rate->flags & RATE_INFO_FLAGS_S1G_MCS){
		ret = cfg80211_calculate_bitrate_s1g(rate);
		return ret;
	}
	ret = rate->legacy;
	return ret;
}
