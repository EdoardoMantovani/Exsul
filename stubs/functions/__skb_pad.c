int __skb_pad(struct sk_buff *skb, int pad, bool free_on_error){
	int ret = 0;
	int ntail = 0;

	/* If the skbuff is non linear tailroom is always zero.. */
	if (!skb_cloned(skb) && skb_tailroom(skb) >= pad) {
		memset(skb->data+skb->len, 0, pad);
		return 0;
	}

	ntail = skb->data_len + pad - (skb->end - skb->tail);
	if (likely(skb_cloned(skb) || ntail > 0)) {
		ret = pskb_expand_head(skb, 0, ntail, GFP_ATOMIC);
		if (unlikely(ret)){
			goto free_skb;
		}
	}

	ret = skb_linearize(skb);
	if (unlikely(ret)){
		goto free_skb;
	}
	memset(skb->data + skb->len, 0, pad);
	return 0;

free_skb:
	if (free_on_error){
		kfree_skb(skb);
	}
	return ret;
}
