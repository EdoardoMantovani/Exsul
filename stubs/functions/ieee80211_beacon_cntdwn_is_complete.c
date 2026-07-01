bool ieee80211_beacon_cntdwn_is_complete(struct ieee80211_vif *ieee80211_vif, unsigned int link_id){
	int ret = 0;
	struct ieee80211_sub_if_data *sdata = NULL;
	struct ieee80211_link_data *link = NULL;
	struct beacon_data *beacon = NULL;
	u8 *beacon_data = NULL;
	size_t beacon_data_len = 0;

	if (!ieee80211_sdata_running(sdata)){
		return false;
	}

	if (link_id >= IEEE80211_MLD_MAX_NUM_LINKS){
		return ret;
	}

	link = rcu_dereference(sdata->link[link_id]);
	if (link == NULL){
		goto out;
	}

	if (ieee80211_vif->type == NL80211_IFTYPE_AP) {
		beacon = rcu_dereference(link->u.ap.beacon);
		if (!beacon || !beacon->tail){
			goto out;
		}
		beacon_data = beacon->tail;
		beacon_data_len = beacon->tail_len;
	}else{
		goto out;
	}

	if (!beacon->cntdwn_counter_offsets[0]){
		goto out;
	}

	if (beacon->cntdwn_counter_offsets[0] > beacon_data_len){
		goto out;
	}
	if (beacon_data[beacon->cntdwn_counter_offsets[0]] == 1){
		ret = true;
	}
 out:
	return ret;
}
