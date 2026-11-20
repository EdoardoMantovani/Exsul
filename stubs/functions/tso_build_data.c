void tso_build_data(const struct sk_buff *skb, struct tso_t *tso, int size){
	skb_frag_t *frag = NULL;
	tso->tcp_seq += size; /* not worth avoiding this operation for UDP */
	tso->size -= size;
	tso->data += size;

	if ((tso->size == 0) && (tso->next_frag_idx < skb_shinfo(skb)->nr_frags)) {
		frag = &skb_shinfo(skb)->frags[tso->next_frag_idx];

		/* Move to next segment */
		tso->size = skb_frag_size(frag);
		tso->data = skb_frag_address(frag);
		tso->next_frag_idx++;
	}
}
