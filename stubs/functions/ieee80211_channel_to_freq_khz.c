u32 ieee80211_channel_to_freq_khz(int chan, enum nl80211_band band){
	int ret = 0;

	/* see 802.11 17.3.8.3.2 and Annex J there are overlapping channel numbers in 5GHz and 2GHz bands */
	if (chan <= 0){
		return ret; /* not supported */
	}
	switch (band) {
	case NL80211_BAND_2GHZ:
	case NL80211_BAND_LC:
		if (chan == 14){
			return MHZ_TO_KHZ(2484);
		}else if (chan < 14){
			return MHZ_TO_KHZ(2407 + chan * 5);
		}
		break;
	case NL80211_BAND_5GHZ:
		if (chan >= 182 && chan <= 196){
			return MHZ_TO_KHZ(4000 + chan * 5);
		}else{
			return MHZ_TO_KHZ(5000 + chan * 5);
		}
		break;
	case NL80211_BAND_6GHZ:
		/* see 802.11ax D6.1 27.3.23.2 */
		if (chan == 2){
			return MHZ_TO_KHZ(5935);
		}
		if (chan <= 233){
			return MHZ_TO_KHZ(5950 + chan * 5);
		}
		break;
	case NL80211_BAND_60GHZ:
		if (chan < 7){
			return MHZ_TO_KHZ(56160 + chan * 2160);
		}
		break;
	case NL80211_BAND_S1GHZ:
		return 902000 + chan * 500;
	default:
		;
	}
	return ret; /* not supported */
}
