struct sk_buff *skb_dequeue_tail(struct sk_buff_head *list){
	struct sk_buff *result = NULL;

	pthread_mutex_lock(&director.skb_queue_lock);
	result = __skb_dequeue_tail(list);
	pthread_mutex_unlock(&director.skb_queue_lock);
	return result;
}
