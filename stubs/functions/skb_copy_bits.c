int skb_copy_bits(const struct sk_buff *skb, int offset, void *to, int len){
	int ret = 0;
	int copy = 0;
	int headlen = 0;

	headlen = skb->len - skb->data_len;
	if (offset < 0 || len < 0 || offset + len > skb->len){
		ret = -EFAULT;
		return ret;
	}
	if (offset < headlen) {
		copy = headlen - offset;
		if (copy > len){
			copy = len;
		}
		memcpy(to, skb->data + offset, copy);
		len    -= copy;
		offset  = 0;
		to      = (char *)(to + copy);
	}else{
		offset -= headlen;
	}

	if (len){
		ret = -EFAULT;
		return ret;
	}
	return ret;
}
