struct ieee80211_hw *wiphy_to_ieee80211_hw(struct wiphy *wiphy){
	if(wiphy == NULL || director.ieee80211_local == NULL){
		return NULL;
	}

	return director.ieee80211_local->ieee80211_hw;
}
