struct sk_buff *ieee80211_pspoll_get(struct ieee80211_hw *hw, struct ieee80211_vif *ieee80211_vif){
	struct sk_buff *skb = NULL;
	struct ieee80211_local *local = NULL;
	struct ieee80211_vif_cfg *cfg = NULL;
	struct ieee80211_pspoll *pspoll = NULL;
	struct ieee80211_sub_if_data *sdata = NULL;

	local = director.ieee80211_local;
	sdata = local->sdata;
	if (ieee80211_vif->type != NL80211_IFTYPE_STATION){
		return NULL;
	}
	skb = dev_alloc_skb(hw->extra_tx_headroom + sizeof(*pspoll));
	if (skb == NULL){
		return NULL;
	}
	skb_reserve(skb, hw->extra_tx_headroom);
	cfg = &ieee80211_vif->cfg;
	pspoll = skb_put_zero(skb, sizeof(*pspoll));
	pspoll->frame_control = cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_PSPOLL);
	pspoll->aid = cpu_to_le16(cfg->aid);

	/* aid in PS-Poll has its two MSBs each set to 1 */
	pspoll->aid |= cpu_to_le16(1 << 15 | 1 << 14);
	memcpy(pspoll->bssid, cfg->ap_addr, ETH_ALEN);
	memcpy(pspoll->ta, ieee80211_vif->addr, ETH_ALEN);
	return skb;
}
