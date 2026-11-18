void netif_receive_skb_list(struct list_head *head){
	netif_receive_skb_list_internal(head);
}
