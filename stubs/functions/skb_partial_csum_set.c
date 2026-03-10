bool skb_partial_csum_set(struct sk_buff *skb, u16 start, u16 off){
	u32 csum_end = 0;
	u32 csum_start = 0;

	csum_end = (u32)start + (u32)off + sizeof(__sum16);
	csum_start = (skb->data - skb->head) + (u32)start;

	if (unlikely(csum_start >= (u16)~0U || csum_end > (skb->len - skb->data_len))) {
		return false;
	}
	skb->ip_summed = CHECKSUM_PARTIAL;
	skb->csum_start = csum_start;
	skb->csum_offset = off;
	skb->transport_header = csum_start;
	return true;
}
