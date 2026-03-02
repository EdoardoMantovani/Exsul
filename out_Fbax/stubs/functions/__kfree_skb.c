void __kfree_skb(struct sk_buff *skb){
	if(skb != NULL){
		free(skb);
	}
}
