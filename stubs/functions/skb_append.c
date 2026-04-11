void skb_append(struct sk_buff *old, struct sk_buff *newsk, struct sk_buff_head *list){
	pthread_mutex_lock(&director.skb_queue_lock);
	__skb_queue_after(list, old, newsk);
	pthread_mutex_unlock(&director.skb_queue_lock);
}
