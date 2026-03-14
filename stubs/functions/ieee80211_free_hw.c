void ieee80211_free_hw(struct ieee80211_hw *hw){
	struct wiphy *wiphy = NULL;
	struct ieee80211_local *local = NULL;

	local = &director.ieee80211_local;
	wiphy = local->wiphy;
	if(wiphy == NULL){
		goto end;
	}

	sta_info_stop(local);

	for (enum nl80211_band band = 0; band < NUM_NL80211_BANDS; band++) {
		if (!(local->sband_allocated & BIT(band))){
			continue;
		}
		kfree(wiphy->bands + sizeof(struct ieee80211_supported_band) * band);
	}

	wiphy_free(wiphy);
	end:
}
