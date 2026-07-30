void kfree_skb_list_reason(void *list, int reason){
	struct sk_buff *skb = list;
	struct sk_buff *next = NULL;

	while (skb != NULL){
		next = skb_list_next(skb);
		skb_list_next(skb) = NULL;
		skb_list_prev(skb) = NULL;
		kfree_skb_reason(skb, reason);
		skb = next;
	}
}
