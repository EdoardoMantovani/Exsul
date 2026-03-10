struct sk_buff *skb_dequeue_tail(struct sk_buff_head *list){
	struct sk_buff *result = NULL;

	result = __skb_dequeue_tail(list);
	return result;
}
