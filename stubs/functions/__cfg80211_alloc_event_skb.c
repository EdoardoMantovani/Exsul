struct sk_buff *__cfg80211_alloc_event_skb(void *wiphy, void *wdev, int approxlen, int event_idx, int gfp){
	return __alloc_skb(approxlen > 0 ? approxlen : 256, gfp, 0, 0);
}
