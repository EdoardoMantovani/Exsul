u32 ieee80211_calc_rx_airtime(struct ieee80211_hw *hw, struct ieee80211_rx_status *status, int len){
	int ret = 0;
	bool sp = false;
	bool cck = false;
	u32 duration = 0;
	u32 overhead = 0;
	struct wiphy *wiphy = NULL;
	struct ieee80211_rate *rate = NULL;
	struct ieee80211_local *local = NULL;
	struct ieee80211_supported_band *sband = NULL;

	local = director.ieee80211_local;
	wiphy = local->wiphy;
	if (status->encoding == RX_ENC_LEGACY) {
		rate = NULL;
		sp = status->enc_flags & RX_ENC_FLAG_SHORTPRE;
		cck = false;

		/* on 60GHz or sub-1GHz band, there are no legacy rates */
		if (status->band == NL80211_BAND_60GHZ || status->band == NL80211_BAND_S1GHZ){
			return ret;
		}
		sband = wiphy->bands[status->band];
		if (!sband || status->rate_idx >= sband->n_bitrates){
			return ret;
		}
		rate = &sband->bitrates[status->rate_idx];
		cck = rate->flags & IEEE80211_RATE_MANDATORY_B;

		ret = ieee80211_calc_legacy_rate_duration(rate->bitrate, sp, cck, len);
		return ret;
	}

	duration = ieee80211_get_rate_duration(hw, status, &overhead);
	if (duration == 0){
		return ret;
	}
	duration *= len;
	duration /= AVG_PKT_SIZE;
	duration /= 1024;
	return duration;
}
