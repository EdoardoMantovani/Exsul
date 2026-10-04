void ieee80211_send_bar(struct ieee80211_vif *vif, u8 *ra, u16 tid, u16 ssn){
	u16 bar_control = 0;
	struct sk_buff *skb = NULL;
	struct ieee80211_bar *bar = NULL;
	struct ieee80211_local *local = NULL;

	local = director.ieee80211_local;
	skb = dev_alloc_skb(sizeof(*bar) + local->hw.extra_tx_headroom);
	if (skb == NULL){
		goto end;
	}
	skb_reserve(skb, local->hw.extra_tx_headroom);
	bar = skb_put_zero(skb, sizeof(*bar));
	bar->frame_control = cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_BACK_REQ);
	memcpy(bar->ra, ra, ETH_ALEN);
	memcpy(bar->ta, vif->addr, ETH_ALEN);
	bar_control |= (u16)IEEE80211_BAR_CTRL_ACK_POLICY_NORMAL;
	bar_control |= (u16)IEEE80211_BAR_CTRL_CBMTID_COMPRESSED_BA;
	bar_control |= (u16)(tid << IEEE80211_BAR_CTRL_TID_INFO_SHIFT);
	bar->control = cpu_to_le16(bar_control);
	bar->start_seq_num = cpu_to_le16(ssn);

	IEEE80211_SKB_CB(skb)->flags |= IEEE80211_TX_INTFL_DONT_ENCRYPT | IEEE80211_TX_CTL_REQ_TX_STATUS;
	// TODO
	//ieee80211_tx_skb_tid(sdata, skb, tid, -1);
	end:
}
