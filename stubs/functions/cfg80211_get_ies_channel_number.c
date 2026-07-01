int cfg80211_get_ies_channel_number(const u8 *ie, size_t ielen, enum nl80211_band band){
	int ret = 0;
	const struct element *tmp = NULL;
	struct ieee80211_ht_operation *htop = NULL;
	struct ieee80211_s1g_oper_ie *s1gop = NULL;
	struct ieee80211_he_operation *he_oper = NULL;
	const struct ieee80211_he_6ghz_oper *he_6ghz_oper = NULL;

	if (band == NL80211_BAND_6GHZ) {
		he_oper = NULL;
		tmp = cfg80211_find_ext_elem(WLAN_EID_EXT_HE_OPERATION, ie, ielen);
		if (tmp && tmp->datalen >= sizeof(*he_oper) && tmp->datalen >= ieee80211_he_oper_size(&tmp->data[1])) {
			he_oper = (void *)&tmp->data[1];

			he_6ghz_oper = ieee80211_he_6ghz_oper(he_oper);
			if (!he_6ghz_oper){
				ret = -EINVAL;
				return ret;
			}
			return he_6ghz_oper->primary;
		}
	}else if (band == NL80211_BAND_S1GHZ) {
		tmp = cfg80211_find_elem(WLAN_EID_S1G_OPERATION, ie, ielen);
		if (tmp && tmp->datalen >= sizeof(struct ieee80211_s1g_oper_ie)) {
			s1gop = (void *)tmp->data;
			ret = s1gop->oper_ch;
			return ret;
		}
	}else{
		tmp = cfg80211_find_elem(WLAN_EID_DS_PARAMS, ie, ielen);
		if (tmp && tmp->datalen == 1){
			ret = tmp->data[0];
			return ret;
		}
		tmp = cfg80211_find_elem(WLAN_EID_HT_OPERATION, ie, ielen);
		if (tmp && tmp->datalen >= sizeof(struct ieee80211_ht_operation)) {
			htop = (void *)tmp->data;
			ret = htop->primary_chan;
			return ret;
		}
	}

	ret = -EINVAL;
	return ret;
}
