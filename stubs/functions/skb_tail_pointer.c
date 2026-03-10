unsigned char *skb_tail_pointer(const struct sk_buff *skb){
	#if BITS_PER_LONG > 32
	return skb->head + skb->tail;
	#else
	return skb->tail;
	#endif
}

