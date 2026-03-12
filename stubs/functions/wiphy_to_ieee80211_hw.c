struct ieee80211_hw *wiphy_to_ieee80211_hw(struct wiphy *wiphy){
	struct ieee80211_local *local = NULL;

	if(wiphy == NULL){
		return NULL;
	}

	local = &wiphy->priv;
	/* in the kernel's version of ieee80211_local, 'ieee80211_hw' is always the first element of the struct, play on this for overcoming this issue. */
	return local;
}
