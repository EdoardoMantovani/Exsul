struct ieee80211_channel *ieee80211_get_channel_khz(struct wiphy *wiphy, u32 freq){
	enum nl80211_band band = 0;
	struct ieee80211_channel *chan = NULL;
	struct ieee80211_supported_band *sband = NULL;

	for (band = 0; band < NUM_NL80211_BANDS; band++) {
		sband = wiphy->bands[band];

		if (sband == NULL){
			continue;
		}
		for (int i = 0; i < sband->n_channels; i++) {
			chan = &sband->channels[i];

			if (ieee80211_channel_to_khz(chan) == freq){
				return chan;
			}
		}
	}
	return NULL;
}
