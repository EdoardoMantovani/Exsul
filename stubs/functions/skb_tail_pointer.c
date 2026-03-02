unsigned char *skb_tail_pointer(const struct sk_buff *skb){
	#if BITS_PER_LONG > 32
	return *(unsigned char *)skb->head + *(sk_buff_data_t *)skb->tail;
	#else
	return *(sk_buff_data_t *)skb->tail;
	#endif
}

