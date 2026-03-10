const struct element *cfg80211_find_vendor_elem(unsigned int oui, int oui_type, const u8 *ies, unsigned int len) {
	const struct element *elem = NULL;
	u8 match[] = { oui >> 16, oui >> 8, oui, oui_type };
	int match_len = (oui_type < 0) ? 3 : sizeof(match);

	if (oui_type > 0xff){
		return NULL;
	}

	elem = cfg80211_find_elem_match(WLAN_EID_VENDOR_SPECIFIC, ies, len, match, match_len, 0);

	if (!elem || elem->datalen < 4){
		return NULL;
	}
	return elem;
}
