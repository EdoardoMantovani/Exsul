unsigned int ieee80211_get_hdrlen_from_skb(const struct sk_buff *skb){
	int ret = 0;
	unsigned int hdrlen = 0;
	const struct ieee80211_hdr *hdr = NULL;

	hdr = (const struct ieee80211_hdr *)skb->data;

	if (unlikely(skb->len < 10)){
		return ret;
	}
	hdrlen = ieee80211_hdrlen(hdr->frame_control);
	if (unlikely(hdrlen > skb->len)){
		return ret;
	}
	return hdrlen;
}
