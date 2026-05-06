void netif_receive_skb_list(struct list_head *head){
	struct sk_buff *skb = NULL;

	if (list_empty(head)){
		printf("empty\n");
		return;
	}
	netif_receive_skb_list_internal(head);
}
