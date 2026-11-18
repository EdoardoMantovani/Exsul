struct link_sta_info *link_sta_info_get_bss(struct ieee80211_sub_if_data *sdata, const u8 *addr){
	static const struct rhashtable_params link_sta_rht_params = {
		.nelem_hint = 3,
		.automatic_shrinking = true,
		.head_offset = offsetof(struct link_sta_info, link_hash_node),
		.key_offset = offsetof(struct link_sta_info, addr),
		.key_len = ETH_ALEN,
		.max_size = CONFIG_MAC80211_STA_HASH_MAX_SIZE,
	};
	struct ieee80211_local *local = director.ieee80211_local;
	struct rhlist_head *head = NULL;
	struct rhlist_head *tmp = NULL;
	struct link_sta_info *link_sta = NULL;
	struct link_sta_info *fallback = NULL;

	if (local == NULL || addr == NULL){
		return NULL;
	}

	head = rhltable_lookup(&local->link_sta_hash, (void *)addr, link_sta_rht_params);
	for (tmp = head; tmp != NULL; tmp = tmp->next) {
		link_sta = (struct link_sta_info *)((char *)tmp - offsetof(struct link_sta_info, link_hash_node));
		if (fallback == NULL){
			fallback = link_sta;
		}
		if (sdata == NULL){
			return link_sta;
		}
		if (link_sta->sta == NULL){
			continue;
		}
		if (&link_sta->sta->sdata == sdata || (link_sta->sta->sdata.bss != NULL && link_sta->sta->sdata.bss == sdata->bss)){
			return link_sta;
		}
	}
	return fallback;
}
