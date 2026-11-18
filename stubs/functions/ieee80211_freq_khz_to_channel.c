int ieee80211_freq_khz_to_channel(u32 freq){
	int ret = 0;

	/* TODO: just handle MHz for now */
	freq = KHZ_TO_MHZ(freq);

	/* see 802.11 17.3.8.3.2 and Annex J */
	if (freq == 2484){
		ret = 14;
		return ret;
	}else if (freq < 2484){
		ret = (freq - 2407) / 5;
		return ret;
	}else if (freq >= 4910 && freq <= 4980){
		ret = (freq - 4000) / 5;
		return ret;
	}else if (freq < 5925){
		ret = (freq - 5000) / 5;
		return ret;
	}else if (freq == 5935){
		ret = 2;
		return ret;
	}else if (freq <= 45000){ /* DMG band lower limit */
		/* see 802.11ax D6.1 27.3.22.2 */
		ret = (freq - 5950) / 5;
		return ret;
	}else if (freq >= 58320 && freq <= 70200){
		ret = (freq - 56160) / 2160;
		return ret;
	}else{
		return ret;
	}
}
