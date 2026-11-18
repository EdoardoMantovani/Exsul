struct sk_buff *ieee80211_nullfunc_get(struct ieee80211_hw *hw, struct ieee80211_vif *ieee80211_vif, int link_id, bool qos_ok){
	bool qos = false;
	unsigned int extra_tx_headroom = 0;
	struct sk_buff *skb = NULL;
	struct sta_info *sta = NULL;
	struct ieee80211_vif_cfg *cfg = NULL;
	struct ieee80211_local *local = NULL;
	struct ieee80211_link_data *link = NULL;
	struct ieee80211_sub_if_data *sdata = NULL;
	struct ieee80211_hdr_3addr *nullfunc = NULL;
	struct ieee80211_bss_conf *link_conf = NULL;

	if (hw == NULL || ieee80211_vif == NULL){
		return NULL;
	}
	if (ieee80211_vif->type != NL80211_IFTYPE_STATION){
		return NULL;
	}

	local = director.ieee80211_local;
	if (local != NULL){
		extra_tx_headroom = hw->extra_tx_headroom;
		sdata = connection_find_sdata(local, ieee80211_vif);
		if (sdata == NULL){
			sdata = local->sdata;
		}
	}else{
		extra_tx_headroom = hw->extra_tx_headroom;
	}

	cfg = &ieee80211_vif->cfg;
	if (qos_ok && sdata != NULL) {
		sta = sta_info_get(sdata, cfg->ap_addr);
		qos = sta != NULL;
	}

	if (link_id >= 0) {
		if ((unsigned int)link_id >= ARRAY_SIZE(ieee80211_vif->link_conf)){
			return NULL;
		}
		if (sdata != NULL && (unsigned int)link_id < ARRAY_SIZE(sdata->link)){
			link = rcu_dereference(sdata->link[link_id]);
			if (link != NULL){
				link_conf = link->conf;
			}
		}
		if (link_conf == NULL){
			link_conf = rcu_dereference(ieee80211_vif->link_conf[link_id]);
		}
		if (link_conf == NULL){
			return NULL;
		}
	}

	skb = dev_alloc_skb(extra_tx_headroom + sizeof(*nullfunc) + 2);
	if (skb == NULL){
		return NULL;
	}

	skb_reserve(skb, extra_tx_headroom);

	nullfunc = skb_put_zero(skb, sizeof(*nullfunc));
	nullfunc->frame_control = cpu_to_le16(IEEE80211_FTYPE_DATA | IEEE80211_STYPE_NULLFUNC | IEEE80211_FCTL_TODS);
	if (qos) {
		__le16 qoshdr = cpu_to_le16(7);

		nullfunc->frame_control |= cpu_to_le16(IEEE80211_STYPE_QOS_NULLFUNC);
		skb->priority = 7;
		skb_set_queue_mapping(skb, IEEE80211_AC_VO);
		skb_put_data(skb, &qoshdr, sizeof(qoshdr));
	}

	if (link_conf != NULL) {
		memcpy(nullfunc->addr1, link_conf->bssid, ETH_ALEN);
		memcpy(nullfunc->addr2, link_conf->addr, ETH_ALEN);
		memcpy(nullfunc->addr3, link_conf->bssid, ETH_ALEN);
	}else{
		memcpy(nullfunc->addr1, cfg->ap_addr, ETH_ALEN);
		memcpy(nullfunc->addr2, ieee80211_vif->addr, ETH_ALEN);
		memcpy(nullfunc->addr3, cfg->ap_addr, ETH_ALEN);
	}
	return skb;
}
