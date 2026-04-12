void ieee80211_parse_qos(struct ieee80211_rx_data *rx){
	int tid = 0;
	u8 *qc = NULL;
	struct sk_buff *skb = NULL;
	struct ieee80211_hdr *hdr = NULL;

	skb = (struct sk_buff *)rx->skb;
	hdr = (struct ieee80211_hdr *)skb->data;

	if (ieee80211_is_data_qos(hdr->frame_control)) {
		qc = ieee80211_get_qos_ctl(hdr);
		tid = *qc & IEEE80211_QOS_CTL_TID_MASK;
		rx->seqno_idx   = tid;
		rx->security_idx = tid;
	}else{
		rx->seqno_idx   = IEEE80211_NUM_TIDS;
		rx->security_idx = 0;
	}
}
