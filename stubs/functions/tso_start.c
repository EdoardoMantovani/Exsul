int tso_start(struct sk_buff *skb, struct tso_t *tso){
	int tlen = 0;
	int hdr_len = 0;
	skb_frag_t *frag = NULL;
	struct iphdr *iphdr = NULL;
	struct tcphdr *tcphdr = NULL;

	tlen = skb_is_gso_tcp(skb) ? tcp_hdrlen(skb) : sizeof(struct udphdr);
	hdr_len = skb_transport_offset(skb) + tlen;
	tso->tlen = tlen;
	/* 'ip_hdr' is given by:'(struct iphdr *)skb_network_header(skb); which is 'skb->head + skb->network_header' */
	iphdr = (struct iphdr *)(skb->head + skb->network_header);
	tso->ip_id = ntohs(iphdr->id);
	/* 'tcp_hdr' is given by: '(struct tcphdr *)skb_transport_header(skb)' which is 'skb->head + skb->transport_header' */
	tcphdr = (struct tcphdr *)(skb->head + skb->transport_header);
	tso->tcp_seq = (tlen != sizeof(struct udphdr)) ? ntohl(tcphdr->seq) : 0;
	tso->next_frag_idx = 0;
	tso->ipv6 = vlan_get_protocol(skb) == htons(ETH_P_IPV6);

	/* Build first data */
	tso->size = skb_headlen(skb) - hdr_len;
	tso->data = skb->data + hdr_len;
	if ((tso->size == 0) && (tso->next_frag_idx < skb_shinfo(skb)->nr_frags)) {
		frag = &skb_shinfo(skb)->frags[tso->next_frag_idx];

		/* Move to next segment */
		tso->size = skb_frag_size(frag);
		tso->data = skb_frag_address(frag);
		tso->next_frag_idx++;
	}
	return hdr_len;
}
