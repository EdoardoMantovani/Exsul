void skb_append(struct sk_buff *old, struct sk_buff *newsk, struct sk_buff_head *list){
	__skb_queue_after(list, old, newsk);
}
