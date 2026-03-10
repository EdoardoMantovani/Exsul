int skb_copy_ubufs(struct sk_buff *skb, int gfp_mask){
	int ret = 0;
	unsigned int old_len = 0;
	unsigned int nr_frags = 0;
	unsigned int extra_tail = 0;
	unsigned int linearized_len = 0;
	unsigned char *dst = NULL;
	struct skb_shared_info *shinfo = NULL;

	if (skb == NULL){
		ret = -EINVAL;
		return ret;
	}

	if (skb_shared(skb) || skb_unclone(skb, gfp_mask)){
		ret = -EINVAL;
		return ret;
	}

	if (!skb_frags_readable(skb)){
		ret = -EFAULT;
		return ret;
	}

	shinfo = skb_shinfo(skb);
	nr_frags = shinfo->nr_frags;

	if (skb_has_frag_list(skb)){
		ret = -EOPNOTSUPP;
		return ret;
	}

	linearized_len = skb->data_len;
	if (nr_frags == 0 || linearized_len == 0){
		goto release;
	}

	extra_tail = 0;

	if (skb_tailroom(skb) < linearized_len){
		extra_tail = linearized_len - skb_tailroom(skb);
	}

	if (extra_tail) {
		ret = pskb_expand_head(skb, 0, extra_tail, gfp_mask);
		if (ret){
			return ret;
		}
	}

	shinfo = skb_shinfo(skb);
	dst = skb_tail_pointer(skb);
	old_len = skb->len;

	skb_put(skb, linearized_len);
	skb->len = old_len;
	skb->data_len = 0;

	for (int i = 0; i < nr_frags; i++){
		//skb_frag_unref(skb, i);
	}

	memset(shinfo->frags, 0, sizeof(shinfo->frags));
	shinfo->nr_frags = 0;

	release:
	skb_zcopy_clear(skb, false);
	return ret;
}
