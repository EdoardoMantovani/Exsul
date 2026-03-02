void skb_set_tail_pointer(struct sk_buff *skb, const int offset){
	#if BITS_PER_LONG > 32
	skb_reset_tail_pointer(skb);
	*(sk_buff_data_t *)skb->tail += offset;
	#else
	*(sk_buff_data_t *)skb->tail = *(unsigned char *)skb->data + offset;
	#endif
}
