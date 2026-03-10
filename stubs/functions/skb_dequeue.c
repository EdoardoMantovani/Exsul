struct sk_buff *skb_dequeue(struct sk_buff_head *list){
	struct sk_buff *result = NULL;

	result = __skb_dequeue(list);
	return result;
}
