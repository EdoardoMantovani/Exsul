__sum16 csum_ipv6_magic(struct in6_addr *saddr, struct in6_addr *daddr, __u32 len, __u8 proto, __wsum csum){
	int carry = 0;
	__u32 ulen = 0;
	__u32 uproto = 0;
	__u32 sum = (__force u32)csum;

	sum += (__force u32)saddr->s6_addr32[0];
	carry = (sum < (__force u32)saddr->s6_addr32[0]);
	sum += carry;

	sum += (__force u32)saddr->s6_addr32[1];
	carry = (sum < (__force u32)saddr->s6_addr32[1]);
	sum += carry;

	sum += (__force u32)saddr->s6_addr32[2];
	carry = (sum < (__force u32)saddr->s6_addr32[2]);
	sum += carry;

	sum += (__force u32)saddr->s6_addr32[3];
	carry = (sum < (__force u32)saddr->s6_addr32[3]);
	sum += carry;

	sum += (__force u32)daddr->s6_addr32[0];
	carry = (sum < (__force u32)daddr->s6_addr32[0]);
	sum += carry;

	sum += (__force u32)daddr->s6_addr32[1];
	carry = (sum < (__force u32)daddr->s6_addr32[1]);
	sum += carry;

	sum += (__force u32)daddr->s6_addr32[2];
	carry = (sum < (__force u32)daddr->s6_addr32[2]);
	sum += carry;

	sum += (__force u32)daddr->s6_addr32[3];
	carry = (sum < (__force u32)daddr->s6_addr32[3]);
	sum += carry;

	ulen = (__force u32)htonl((__u32) len);
	sum += ulen;
	carry = (sum < ulen);
	sum += carry;

	uproto = (__force u32)htonl(proto);
	sum += uproto;
	carry = (sum < uproto);
	sum += carry;

	return csum_fold((__force __wsum)sum);
}
