void skb_reset_tail_pointer(struct sk_buff *skb){
	#if BITS_PER_LONG > 32
	skb->tail = skb->data - skb->head;
	#else
	skb->tail = skb->data;
	#endif
}
