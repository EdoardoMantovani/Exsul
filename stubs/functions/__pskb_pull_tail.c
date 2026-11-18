void *__pskb_pull_tail(struct sk_buff *skb, int delta){
	int eat = 0;
	struct sk_buff *list  = NULL;
	struct sk_buff *clone = NULL;
	struct sk_buff *insp  = NULL;

	eat = (skb->tail + delta) - skb->end;
	if (!skb_frags_readable(skb)){
		return NULL;
	}
	if ((eat > 0 || skb_cloned(skb)) && pskb_expand_head(skb, 0, eat > 0 ? eat + 128 : 0, 0)){
		return NULL;
	}
	skb_copy_bits(skb, skb_headlen(skb), skb_tail_pointer(skb), delta);
	if (!skb_has_frag_list(skb)){
		goto pull_pages;
	}
	eat = delta;
	for (int i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
		eat -= skb_frag_size(&skb_shinfo(skb)->frags[i]);
		if (eat <= 0){
			goto pull_pages;
		}
	}

	if (eat) {
		list  = skb_shinfo(skb)->frag_list;
		do {
			if (list->len <= eat) {
				eat -= list->len;
				insp = list = list->next;
			}else{
				if (skb_is_gso(skb) && !list->head_frag && skb_headlen(list)){
					skb_shinfo(skb)->gso_type |= SKB_GSO_DODGY;
				}
				if (skb_shared(list)) {
					if (!(clone = skb_clone(list, GFP_ATOMIC))){
						return NULL;
					}
					insp = list->next;
					list = clone;
				}else{
					insp = list;
			}
			if (!pskb_pull(list, eat)) {
				kfree_skb(clone);
				return NULL;
			}
			break;
		}
	}while (eat);
		while ((list = skb_shinfo(skb)->frag_list) != insp) {
			skb_shinfo(skb)->frag_list = list->next;
			consume_skb(list);
		}
		if (clone) {
			clone->next = list;
			skb_shinfo(skb)->frag_list = clone;
		}
	}

	pull_pages:
	eat = delta;
	int k = 0;
	for (int i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
		int size         = skb_frag_size(&skb_shinfo(skb)->frags[i]);
		skb_frag_t *frag = &skb_shinfo(skb)->frags[k];
		if (size <= eat) {
			skb_frag_unref(skb, i);
			eat -= size;
		}else{
			*frag = skb_shinfo(skb)->frags[i];
			if (eat) {
				skb_frag_off_add(frag, eat);
				skb_frag_size_sub(frag, eat);
				if (i == 0){
					goto end;
				}
				eat = 0;
			}
			k++;
		}
	}
	skb_shinfo(skb)->nr_frags = k;
	end:
	skb->tail     += delta;
	skb->data_len -= delta;
	if (skb->data_len == 0){
		skb_zcopy_clear(skb, false);
	}
	return skb_tail_pointer(skb);
}
