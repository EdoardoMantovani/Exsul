struct nlmsghdr *__nlmsg_put(struct sk_buff *skb, u32 portid, u32 seq, int type, int len, int flags){
	int size = 0;
	struct nlmsghdr *nlh = NULL;;

	size = len + NLMSG_HDRLEN;
	nlh = skb_put(skb, NLMSG_ALIGN(size));
	nlh->nlmsg_type = type;
	nlh->nlmsg_len = size;
	nlh->nlmsg_flags = flags;
	nlh->nlmsg_pid = portid;
	nlh->nlmsg_seq = seq;
	if (!__builtin_constant_p(size) || NLMSG_ALIGN(size) - size != 0){
		memset((unsigned char *)(nlh + NLMSG_HDRLEN) + len, 0, NLMSG_ALIGN(size) - size);
	}
	return nlh;
}

