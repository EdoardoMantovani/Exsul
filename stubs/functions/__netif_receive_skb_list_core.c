void __netif_receive_skb_list_core(struct list_head *head, bool pfmemalloc){
	struct sk_buff *skb = NULL;
	struct list_head *pos = NULL;
	struct list_head *tmp = NULL;

	pos = head->next;
	while (pos != head) {
		tmp = pos->next;
		skb = (struct sk_buff *)pos;
		skb_list_del_init(skb);
		consume_skb(skb);
		pos = tmp;
	}
}
