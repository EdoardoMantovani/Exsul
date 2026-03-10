void ieee80211_set_key_rx_seq(struct ieee80211_key_conf *keyconf, int tid, struct ieee80211_key_seq *seq){
	u8 *pn = NULL;
	struct ieee80211_key *key = NULL;

	key = container_of(keyconf, struct ieee80211_key, conf);

	switch (key->conf.cipher) {
	case WLAN_CIPHER_SUITE_TKIP:
		if (tid < 0 || tid >= IEEE80211_NUM_TIDS){
			return;
		}
		key->u.tkip.rx[tid].iv32 = seq->tkip.iv32;
		key->u.tkip.rx[tid].iv16 = seq->tkip.iv16;
		break;
	case WLAN_CIPHER_SUITE_CCMP:
	case WLAN_CIPHER_SUITE_CCMP_256:
		if (tid < -1 || tid >= IEEE80211_NUM_TIDS){
			return;
		}
		if (tid < 0){
			pn = key->u.ccmp.rx_pn[IEEE80211_NUM_TIDS];
		}else{
			pn = key->u.ccmp.rx_pn[tid];
		}
		memcpy(pn, seq->ccmp.pn, IEEE80211_CCMP_PN_LEN);
		break;
	case WLAN_CIPHER_SUITE_AES_CMAC:
	case WLAN_CIPHER_SUITE_BIP_CMAC_256:
		if (tid != 0){
			return;
		}
		pn = key->u.aes_cmac.rx_pn;
		memcpy(pn, seq->aes_cmac.pn, IEEE80211_CMAC_PN_LEN);
		break;
	case WLAN_CIPHER_SUITE_BIP_GMAC_128:
	case WLAN_CIPHER_SUITE_BIP_GMAC_256:
		if (tid != 0){
			return;
		}
		pn = key->u.aes_gmac.rx_pn;
		memcpy(pn, seq->aes_gmac.pn, IEEE80211_GMAC_PN_LEN);
		break;
	case WLAN_CIPHER_SUITE_GCMP:
	case WLAN_CIPHER_SUITE_GCMP_256:
		if (tid < -1 || tid >= IEEE80211_NUM_TIDS){
			return;
		}
		if (tid < 0){
			pn = key->u.gcmp.rx_pn[IEEE80211_NUM_TIDS];
		}else{
			pn = key->u.gcmp.rx_pn[tid];
		}
		memcpy(pn, seq->gcmp.pn, IEEE80211_GCMP_PN_LEN);
		break;
	default:
		break;
	}
}
