struct sk_buff *skb_clone_sk(struct sk_buff *skb){
	struct sk_buff *clone;

	clone = skb_clone(skb, GFP_ATOMIC);
	if(clone == NULL){
		return NULL;
	}
	clone->destructor = sock_efree;

	return clone;
}
