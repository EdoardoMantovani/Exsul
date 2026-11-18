u32 ieee80211_calc_tx_airtime(struct ieee80211_hw *hw, struct ieee80211_tx_info *info, int len){
	u32 duration = 0;
	u32 cur_duration = 0;
	struct ieee80211_tx_rate *rate = NULL;

	for (int i = 0; i < ARRAY_SIZE(info->status.rates); i++) {
		rate = &info->status.rates[i];

		cur_duration = ieee80211_calc_tx_airtime_rate(hw, rate, NULL, info->band, len);
		if (cur_duration == 0){
			break;
		}
		duration += cur_duration * rate->count;
	}
	return duration;
}
