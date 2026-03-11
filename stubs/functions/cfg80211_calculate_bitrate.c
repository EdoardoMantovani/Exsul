u32 cfg80211_calculate_bitrate(struct rate_info *rate){
	if (rate->flags & RATE_INFO_FLAGS_MCS){
		return cfg80211_calculate_bitrate_ht(rate);
	}
	if (rate->flags & RATE_INFO_FLAGS_DMG){
		return cfg80211_calculate_bitrate_dmg(rate);
	}
	if (rate->flags & RATE_INFO_FLAGS_EXTENDED_SC_DMG){
		return cfg80211_calculate_bitrate_extended_sc_dmg(rate);
	}
	if (rate->flags & RATE_INFO_FLAGS_EDMG){
		return cfg80211_calculate_bitrate_edmg(rate);
	}
	if (rate->flags & RATE_INFO_FLAGS_VHT_MCS){
		return cfg80211_calculate_bitrate_vht(rate);
	}
	if (rate->flags & RATE_INFO_FLAGS_HE_MCS){
		return cfg80211_calculate_bitrate_he(rate);
	}
	if (rate->flags & RATE_INFO_FLAGS_EHT_MCS){
		return cfg80211_calculate_bitrate_eht(rate);
	}
	if (rate->flags & RATE_INFO_FLAGS_S1G_MCS){
		return cfg80211_calculate_bitrate_s1g(rate);
	}
	return rate->legacy;
}
