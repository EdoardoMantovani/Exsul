void tso_build_hdr(const struct sk_buff *skb, char *hdr, struct tso_t *tso, int size, bool is_last){
	int hdr_len = 0;
	int mac_hdr_len = 0;

	hdr_len = skb_transport_offset(skb) + tso->tlen;
	mac_hdr_len = skb_network_offset(skb);
	memcpy(hdr, skb->data, hdr_len);
	if (!tso->ipv6) {
		struct iphdr *iph = (void *)(hdr + mac_hdr_len);

		iph->id = htons(tso->ip_id);
		iph->tot_len = htons(size + hdr_len - mac_hdr_len);
		tso->ip_id++;
	} else {
		struct ipv6hdr *iph = (void *)(hdr + mac_hdr_len);

		iph->payload_len = htons(size + tso->tlen);
	}
	hdr += skb_transport_offset(skb);
	if (tso->tlen != sizeof(struct udphdr)) {
		struct tcphdr *tcph = (struct tcphdr *)hdr;

		put_unaligned_be32(tso->tcp_seq, &tcph->seq);

		if (!is_last) {
			/* Clear all special flags for not last packet */
			tcph->psh = 0;
			tcph->fin = 0;
			tcph->rst = 0;
		}
	} else {
		struct udphdr *uh = (struct udphdr *)hdr;

		uh->len = htons(sizeof(*uh) + size);
	}
}
