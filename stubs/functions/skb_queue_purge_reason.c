void skb_queue_purge_reason(struct sk_buff_head *list, int reason){
	struct sk_buff_head tmp = {};

	if (skb_queue_empty_lockless(list)){
		return;
	}

	__skb_queue_head_init(&tmp);
	skb_queue_splice_init(list, &tmp);
	__skb_queue_purge_reason(&tmp, reason);
}
