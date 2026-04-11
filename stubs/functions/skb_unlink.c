void skb_unlink(struct sk_buff *skb, struct sk_buff_head *list){
	pthread_mutex_lock(&director.skb_queue_lock);
	__skb_unlink(skb, list);
	pthread_mutex_unlock(&director.skb_queue_lock);
}
