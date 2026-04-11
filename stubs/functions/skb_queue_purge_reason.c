void skb_queue_purge_reason(struct sk_buff_head *list, int reason){
	struct sk_buff_head tmp = {};

	if (skb_queue_empty_lockless(list)){
		return;
	}

	__skb_queue_head_init(&tmp);

	pthread_mutex_lock(&director.skb_queue_lock);
	skb_queue_splice_init(list, &tmp);
	pthread_mutex_unlock(&director.skb_queue_lock);

	__skb_queue_purge_reason(&tmp, reason);
}
