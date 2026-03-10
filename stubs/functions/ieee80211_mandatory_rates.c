u32 ieee80211_mandatory_rates(struct ieee80211_supported_band *sband){
	struct ieee80211_rate *bitrates = NULL;
	u32 mandatory_rates = 0;
	enum ieee80211_rate_flags mandatory_flag = 0;

	if (!sband){
		return 1;
	}

	if (sband->band == NL80211_BAND_2GHZ){
		mandatory_flag = IEEE80211_RATE_MANDATORY_B;
	}else{
		mandatory_flag = IEEE80211_RATE_MANDATORY_A;
	}
	bitrates = sband->bitrates;
	for (int i = 0; i < sband->n_bitrates; i++){
		if (bitrates[i].flags & mandatory_flag){
			mandatory_rates |= BIT(i);
		}
	}
	return mandatory_rates;
}
