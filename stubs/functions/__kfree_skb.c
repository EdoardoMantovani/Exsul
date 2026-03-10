void __kfree_skb(struct sk_buff *skb){
	int old = 0;
	bool free_head = false;
	if(skb == NULL){
		goto end;
	}

	if (skb->head != NULL) {
		old = atomic_fetch_sub(&skb_shinfo(skb)->dataref, 1);
		if(old == 1){
			free_head = true;
		}

		if (free_head && !skb->head_frag) {
			free(skb->head);
		}

		skb->head = NULL;
		skb->data = NULL;
		skb_set_tail_offset(skb, 0);
		skb_set_end_offset(skb, 0);
	}
	free(skb);
	end:
}
