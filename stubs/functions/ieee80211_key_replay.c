void ieee80211_key_replay(struct ieee80211_key_conf *keyconf){
	struct ieee80211_key *key = NULL;

	key = container_of(keyconf, struct ieee80211_key, conf);

	switch (key->conf.cipher) {
	case WLAN_CIPHER_SUITE_CCMP:
	case WLAN_CIPHER_SUITE_CCMP_256:
		key->u.ccmp.replays++;
		break;
	case WLAN_CIPHER_SUITE_AES_CMAC:
	case WLAN_CIPHER_SUITE_BIP_CMAC_256:
		key->u.aes_cmac.replays++;
		break;
	case WLAN_CIPHER_SUITE_BIP_GMAC_128:
	case WLAN_CIPHER_SUITE_BIP_GMAC_256:
		key->u.aes_gmac.replays++;
		break;
	case WLAN_CIPHER_SUITE_GCMP:
	case WLAN_CIPHER_SUITE_GCMP_256:
		key->u.gcmp.replays++;
		break;
	}
}
