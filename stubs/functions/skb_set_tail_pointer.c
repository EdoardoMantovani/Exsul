void skb_set_tail_pointer(struct sk_buff *skb, const int offset){
	#if BITS_PER_LONG > 32
	skb_reset_tail_pointer(skb);
	skb->tail += offset;
	#else
	skb->tail = skb->data + offset;
	#endif
}
