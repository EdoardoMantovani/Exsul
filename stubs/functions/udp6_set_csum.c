void udp6_set_csum(bool nocheck, struct sk_buff *skb, struct in6_addr *saddr, struct in6_addr *daddr, int len){
	struct udphdr *uh = NULL;

	uh = udp_hdr(skb);
	if (nocheck == true){
		uh->check = 0;
	}else if (skb_is_gso(skb)){
		uh->check = ~udp_v6_check(len, saddr, daddr, 0);
	}else if (skb->ip_summed == CHECKSUM_PARTIAL) {
		uh->check = 0;
		uh->check = udp_v6_check(len, saddr, daddr, lco_csum(skb));
		if (uh->check == 0){
			uh->check = CSUM_MANGLED_0;
		}
	}else{
		skb->ip_summed = CHECKSUM_PARTIAL;
		skb->csum_start = skb_transport_header(skb) - skb->head;
		skb->csum_offset = offsetof(struct udphdr, check);
		uh->check = ~udp_v6_check(len, saddr, daddr, 0);
	}
}
