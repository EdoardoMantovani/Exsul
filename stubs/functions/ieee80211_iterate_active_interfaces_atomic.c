void ieee80211_iterate_active_interfaces_atomic(struct ieee80211_hw *hw, u32 iter_flags, void (*iterator)(void *data, u8 *mac, struct ieee80211_vif *vif), void *data){
	struct ieee80211_local *local = NULL;

	local = director.ieee80211_local;
	__iterate_interfaces(local, iter_flags | IEEE80211_IFACE_ITER_ACTIVE, iterator, data);
}
