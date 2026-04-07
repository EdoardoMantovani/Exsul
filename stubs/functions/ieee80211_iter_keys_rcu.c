void ieee80211_iter_keys_rcu(struct ieee80211_hw *hw, struct ieee80211_vif *vif, void (*iter)(struct ieee80211_hw *hw, struct ieee80211_vif *vif, struct ieee80211_sta *sta, struct ieee80211_key_conf *key, void *data), void *iter_data){
	struct ieee80211_local *local = NULL;
	struct ieee80211_sub_if_data *sdata = NULL;

	local = director.ieee80211_local;

	if (vif != NULL) {
		sdata = local->sdata;
		_ieee80211_iter_keys_rcu(hw, sdata, iter, iter_data);
	} else {
		list_for_each_entry_rcu(sdata, &local->interfaces, list)
			_ieee80211_iter_keys_rcu(hw, sdata, iter, iter_data);
	}
}
