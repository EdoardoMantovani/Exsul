void skb_reset_tail_pointer(struct sk_buff *skb){
	#if BITS_PER_LONG > 32
	*(sk_buff_data_t *)skb->tail = *(unsigned char *)skb->data - *(unsigned char *)skb->head;
	#else
	*(sk_buff_data_t *)skb->tail = *(unsigned char *)skb->data;
	#endif
}
