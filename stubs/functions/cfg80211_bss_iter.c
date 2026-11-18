void cfg80211_bss_iter(struct wiphy *wiphy, struct cfg80211_chan_def *chandef, void (*iter)(struct wiphy *wiphy, struct cfg80211_bss *bss, void *data), void *iter_data){
	struct ieee80211_local *local = NULL;
	struct cfg80211_internal_bss *bss = NULL;
	struct cfg80211_registered_device *rdev = NULL;

	local = director.ieee80211_local;

	rdev = local->rdev;
	if(rdev == NULL){
		goto end;
	}
	pthread_mutex_lock(&local->bss_lock);
	list_for_each_entry(bss, &rdev->bss_list, list) {
		if (!chandef || cfg80211_is_sub_chan(chandef, bss->pub.channel, false)){
			iter(wiphy, &bss->pub, iter_data);
		}
	}
	pthread_mutex_unlock(&local->bss_lock);
	end:
}
