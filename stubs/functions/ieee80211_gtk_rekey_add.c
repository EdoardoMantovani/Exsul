struct ieee80211_key_conf *ieee80211_gtk_rekey_add(struct ieee80211_vif *ieee80211_vif, u8 idx, u8 *key_data, u8 key_len, int link_id){
	int ret = 0;
	struct ieee80211_sub_if_data *sdata = NULL;
	struct ieee80211_local *local = NULL;
	struct ieee80211_key *prev_key = NULL;
	struct ieee80211_key *key = NULL;

	struct ieee80211_link_data *link_data = NULL;

	local = director.ieee80211_local;

	link_data = link_id < 0 ? &sdata->deflink : sdata_dereference(sdata->link[link_id], sdata);

	if (link_data == NULL){
		return ERR_PTR(-EINVAL);
	}

	if (local->wowlan == NULL){
		return ERR_PTR(-EINVAL);
	}

	if (ieee80211_vif->type != NL80211_IFTYPE_STATION){
		return ERR_PTR(-EINVAL);
	}

	if (idx >= NUM_DEFAULT_KEYS + NUM_DEFAULT_MGMT_KEYS + NUM_DEFAULT_BEACON_KEYS){
		return ERR_PTR(-EINVAL);
	}

	prev_key = wiphy_dereference(local->hw.wiphy, link_data->gtk[idx]);

	if (prev_key == NULL) {
		if (idx < NUM_DEFAULT_KEYS) {
			for (int i = 0; i < NUM_DEFAULT_KEYS; i++) {
				if (i == idx){
					continue;
				}
				prev_key = wiphy_dereference(local->hw.wiphy, link_data->gtk[i]);
				if (prev_key){
					break;
				}
			}
		}else{
			/* For IGTK we have 4 and 5 and for BIGTK - 6 and 7 */
			prev_key = wiphy_dereference(local->hw.wiphy, link_data->gtk[idx ^ 1]);
		}
	}

	if(prev_key == NULL){
		return ERR_PTR(-EINVAL);
	}

	if (key_len < prev_key->conf.keylen){
		return ERR_PTR(-EINVAL);
	}

	key = ieee80211_key_alloc(prev_key->conf.cipher, idx, prev_key->conf.keylen, key_data, 0, NULL);
	if (key){
		return ERR_CAST(key);
	}

	if (sdata->u.mgd.mfp != IEEE80211_MFP_DISABLED){
		key->conf.flags |= IEEE80211_KEY_FLAG_RX_MGMT;
	}

	key->conf.link_id = link_data->link_id;

	// TODO
	//ret = ieee80211_key_link(key, link_data, NULL);
	if (ret){
		return ERR_PTR(ret);
	}

	return &key->conf;
}
