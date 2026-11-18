struct sk_buff *ieee80211_beacon_get_tim(struct ieee80211_hw *hw, struct ieee80211_vif *ieee80211_vif, u16 *tim_offset, u16 *tim_length, unsigned int link_id){
	static const u8 rates[] = {0x82, 0x84, 0x8b, 0x96, 0x0c, 0x12, 0x18, 0x24};
	static const u8 ext_rates[] = {0x30, 0x48, 0x60, 0x6c};
	size_t fixed_len = offsetof(struct ieee80211_mgmt, u.beacon.variable);
	size_t ssid_len = director.ap_ssid_len;
	size_t ies_len = 2 + ssid_len + 2 + sizeof(rates) + 3 + 6 + 2 + sizeof(ext_rates);
	struct ieee80211_bss_conf *link_conf = NULL;
	struct ieee80211_mgmt *mgmt = NULL;
	struct sk_buff *skb = NULL;
	u8 *pos = NULL;
	u8 dtim_period = 1;

	if(director.peer_runtime != NULL && ieee80211_vif != NULL && ieee80211_vif == director.peer_runtime->vif && (director.peer_runtime->mesh_mode || director.peer_runtime->p2p_go)){
		return peer_beacon_get(hw, ieee80211_vif, tim_offset, tim_length);
	}
	(void)link_id;
	if(hw == NULL || ieee80211_vif == NULL || director.ap_ssid == NULL ||
	   ssid_len == 0 || ssid_len > IEEE80211_MAX_SSID_LEN ||
	   ieee80211_vif->type != NL80211_IFTYPE_AP){
		return NULL;
	}
	link_conf = &ieee80211_vif->bss_conf;
	if(link_conf->dtim_period != 0){
		dtim_period = link_conf->dtim_period;
	}
	skb = dev_alloc_skb(hw->extra_tx_headroom + fixed_len + ies_len +
			    hw->extra_beacon_tailroom);
	if(skb == NULL){
		return NULL;
	}
	skb_reserve(skb, hw->extra_tx_headroom);
	mgmt = skb_put_zero(skb, fixed_len);
	mgmt->frame_control = cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_BEACON);
	eth_broadcast_addr(mgmt->da);
	ether_addr_copy(mgmt->sa, ieee80211_vif->addr);
	ether_addr_copy(mgmt->bssid, ieee80211_vif->addr);
	mgmt->u.beacon.beacon_int = cpu_to_le16(link_conf->beacon_int != 0 ?
						 link_conf->beacon_int : 100);
	mgmt->u.beacon.capab_info = cpu_to_le16(WLAN_CAPABILITY_ESS |
						 WLAN_CAPABILITY_SHORT_PREAMBLE |
						 WLAN_CAPABILITY_SHORT_SLOT_TIME);

	pos = skb_put(skb, ies_len);
	*pos++ = WLAN_EID_SSID;
	*pos++ = ssid_len;
	memcpy(pos, director.ap_ssid, ssid_len);
	pos += ssid_len;
	*pos++ = WLAN_EID_SUPP_RATES;
	*pos++ = sizeof(rates);
	memcpy(pos, rates, sizeof(rates));
	pos += sizeof(rates);
	*pos++ = WLAN_EID_DS_PARAMS;
	*pos++ = 1;
	*pos++ = director.ap_channel != 0 ? director.ap_channel : 1;
	if(tim_offset != NULL){
		*tim_offset = pos - skb->data;
	}
	*pos++ = WLAN_EID_TIM;
	*pos++ = 4;
	*pos++ = 0;
	*pos++ = dtim_period;
	*pos++ = 0;
	*pos++ = 0;
	if(tim_length != NULL){
		*tim_length = 6;
	}
	*pos++ = WLAN_EID_EXT_SUPP_RATES;
	*pos++ = sizeof(ext_rates);
	memcpy(pos, ext_rates, sizeof(ext_rates));
	return skb;
}
