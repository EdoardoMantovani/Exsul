void *cfg80211_inform_bss_frame_data(void *wiphy, void *data, void *mgmt, size_t len, int gfp){
	static unsigned char bss = 0;

	return &bss;
}
