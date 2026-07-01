struct sk_buff *__cfg80211_alloc_reply_skb(void *wiphy, int approxlen){
	return __alloc_skb(approxlen > 0 ? approxlen : 256, 0, 0, 0);
}
