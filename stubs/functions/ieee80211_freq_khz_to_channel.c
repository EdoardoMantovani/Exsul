int ieee80211_freq_khz_to_channel(u32 freq)
{
	/* TODO: just handle MHz for now */
	freq = KHZ_TO_MHZ(freq);

	/* see 802.11 17.3.8.3.2 and Annex J */
	if (freq == 2484)
		return 14;
	else if (freq < 2484)
		return (freq - 2407) / 5;
	else if (freq >= 4910 && freq <= 4980)
		return (freq - 4000) / 5;
	else if (freq < 5925)
		return (freq - 5000) / 5;
	else if (freq == 5935)
		return 2;
	else if (freq <= 45000) /* DMG band lower limit */
		/* see 802.11ax D6.1 27.3.22.2 */
		return (freq - 5950) / 5;
	else if (freq >= 58320 && freq <= 70200)
		return (freq - 56160) / 2160;
	else
		return 0;
}
