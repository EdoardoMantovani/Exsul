#ifndef __SKB_H__
#define __SKB_H__

#if BITS_PER_LONG > 32
#define NET_SKBUFF_DATA_USES_OFFSET	1
#endif

#ifndef CONFIG_MAX_SKB_FRAGS
#define CONFIG_MAX_SKB_FRAGS		17
#endif

#ifndef NET_SKB_PAD
#define NET_SKB_PAD			max(32, L1_CACHE_BYTES)
#endif

#define NET_IP_ALIGN			2
#define MAX_SKB_FRAGS			CONFIG_MAX_SKB_FRAGS

#define SKB_ALLOC_FCLONE		0x01
#define SKB_ALLOC_RX			0x02
#define SKB_ALLOC_NAPI			0x04

#define SKB_DST_NOREF 			1UL
#define SKB_DATAREF_SHIFT		16
#define SKB_DATAREF_MASK		((1 << SKB_DATAREF_SHIFT) - 1)

#define SKBFL_ZEROCOPY_FRAG		(SKBFL_ZEROCOPY_ENABLE | SKBFL_SHARED_FRAG)
#define SKBFL_ALL_ZEROCOPY		(SKBFL_ZEROCOPY_FRAG | SKBFL_PURE_ZEROCOPY | SKBFL_DONT_ORPHAN | SKBFL_MANAGED_FRAG_REFS)

#define MAX_TCP_HEADER			L1_CACHE_ALIGN(128 + MAX_HEADER)
#define LL_MAX_HEADER			96
#define MAX_HEADER			(LL_MAX_HEADER)
#define GRO_MAX_HEAD			(MAX_HEADER + 128)
#define GRO_MAX_HEAD_PAD		(GRO_MAX_HEAD + NET_SKB_PAD + NET_IP_ALIGN)
#define SKB_SMALL_HEAD_SIZE		SKB_HEAD_ALIGN(max(MAX_TCP_HEADER, GRO_MAX_HEAD_PAD))

#define SKB_SMALL_HEAD_CACHE_SIZE	(is_power_of_2(SKB_SMALL_HEAD_SIZE) ? (SKB_SMALL_HEAD_SIZE + L1_CACHE_BYTES) : SKB_SMALL_HEAD_SIZE)
#define SKB_HEAD_ALIGN(X)		(SKB_DATA_ALIGN(X) + SKB_DATA_ALIGN(director.struct_sizes.skb_shared_info))

#define dev_kfree_skb(a)		consume_skb(a)

#ifdef NET_SKBUFF_DATA_USES_OFFSET
typedef unsigned int sk_buff_data_t;
#else
typedef unsigned char *sk_buff_data_t;
#endif

#define SKB_DATA_ALIGN(X)		ALIGN(X, SMP_CACHE_BYTES)
#define SKB_WITH_OVERHEAD(X)		((X) - SKB_DATA_ALIGN(director.struct_sizes.skb_shared_info))

/* return minimum truesize of one skb containing X bytes of data */
#define SKB_TRUESIZE(X) ((X) +						\
			 SKB_DATA_ALIGN(SK_BUFF_STRUCT_SIZE) +		\
			 SKB_DATA_ALIGN(sizeof(struct skb_shared_info)))

#define skb_shinfo(SKB)			((struct skb_shared_info *)(skb_end_pointer(SKB)))
#define skb_uarg(SKB)			((struct ubuf_info *)(skb_shinfo(SKB)->destructor_arg))

#define kfree_skb			__kfree_skb
#define alloc_skb(size, priority)	__alloc_skb(size, priority, 0, 0)
#define dev_alloc_skb(length)		__netdev_alloc_skb(NULL, length, GFP_ATOMIC)

/**
 * typedef netmem_ref - a nonexistent type marking a reference to generic
 * network memory.
 *
 * A netmem_ref can be a struct page* or a struct net_iov* underneath.
 *
 * Use the supplied helpers to obtain the underlying memory pointer and fields.
 */
typedef unsigned long __bitwise netmem_ref;

#define ENCAP_TYPE_ETHER	0
#define ENCAP_TYPE_IPPROTO	1

/* Don't change this without changing skb_csum_unnecessary! */
#define CHECKSUM_NONE		0
#define CHECKSUM_UNNECESSARY	1
#define CHECKSUM_COMPLETE	2
#define CHECKSUM_PARTIAL	3

/* ignore 'CHECK_SKB_FIELD' */
#define CHECK_SKB_FIELD(x)

#define skb_queue_walk(queue, skb) \
		for (skb = (queue)->next;					\
		     skb != (struct sk_buff *)(queue);				\
		     skb = skb->next)

#define skb_queue_walk_safe(queue, skb, tmp)					\
		for (skb = (queue)->next, tmp = skb->next;			\
		     skb != (struct sk_buff *)(queue);				\
		     skb = tmp, tmp = skb->next)

#define skb_queue_walk_from(queue, skb)						\
		for (; skb != (struct sk_buff *)(queue);			\
		     skb = skb->next)

#define skb_rbtree_walk(skb, root)						\
		for (skb = skb_rb_first(root); skb != NULL;			\
		     skb = skb_rb_next(skb))

#define skb_rbtree_walk_from(skb)						\
		for (; skb != NULL;						\
		     skb = skb_rb_next(skb))

#define skb_rbtree_walk_from_safe(skb, tmp)					\
		for (; tmp = skb ? skb_rb_next(skb) : NULL, (skb != NULL);	\
		     skb = tmp)

#define skb_queue_walk_from_safe(queue, skb, tmp)				\
		for (tmp = skb->next;						\
		     skb != (struct sk_buff *)(queue);				\
		     skb = tmp, tmp = skb->next)

#define skb_queue_reverse_walk(queue, skb) \
		for (skb = (queue)->prev;					\
		     skb != (struct sk_buff *)(queue);				\
		     skb = skb->prev)

#define skb_queue_reverse_walk_safe(queue, skb, tmp)				\
		for (skb = (queue)->prev, tmp = skb->prev;			\
		     skb != (struct sk_buff *)(queue);				\
		     skb = tmp, tmp = skb->prev)

#define skb_queue_reverse_walk_from_safe(queue, skb, tmp)			\
		for (tmp = skb->prev;						\
		     skb != (struct sk_buff *)(queue);				\
		     skb = tmp, tmp = skb->prev)

enum {
	SKB_FCLONE_UNAVAILABLE,	/* skb has no fclone (from head_cache) */
	SKB_FCLONE_ORIG,	/* orig skb (from fclone_cache) */
	SKB_FCLONE_CLONE,	/* companion fclone skb (from fclone_cache) */
};

enum skb_tstamp_type {
	SKB_CLOCK_REALTIME,
	SKB_CLOCK_MONOTONIC,
	SKB_CLOCK_TAI,
	__SKB_CLOCK_MAX = SKB_CLOCK_TAI,
};

/* Definitions for flags in struct skb_shared_info */
enum {
	/* use zcopy routines */
	SKBFL_ZEROCOPY_ENABLE = BIT(0),
	/* This indicates at least one fragment might be overwritten
	 * (as in vmsplice(), sendfile() ...)
	 * If we need to compute a TX checksum, we'll need to copy
	 * all frags to avoid possible bad checksum
	 */
	SKBFL_SHARED_FRAG = BIT(1),

	/* segment contains only zerocopy data and should not be
	 * charged to the kernel memory.
	 */
	SKBFL_PURE_ZEROCOPY = BIT(2),
	SKBFL_DONT_ORPHAN = BIT(3),
	/* page references are managed by the ubuf_info, so it's safe to
	 * use frags only up until ubuf_info is released
	 */
	SKBFL_MANAGED_FRAG_REFS = BIT(4),
};

enum {
	SKB_GSO_TCPV4 = 1 << 0,
	/* This indicates the skb is from an untrusted source. */
	SKB_GSO_DODGY = 1 << 1,
	/* This indicates the tcp segment has CWR set. */
	SKB_GSO_TCP_ECN = 1 << 2,
	__SKB_GSO_TCP_FIXEDID = 1 << 3,
	SKB_GSO_TCPV6 = 1 << 4,
	SKB_GSO_FCOE = 1 << 5,
	SKB_GSO_GRE = 1 << 6,
	SKB_GSO_GRE_CSUM = 1 << 7,
	SKB_GSO_IPXIP4 = 1 << 8,
	SKB_GSO_IPXIP6 = 1 << 9,
	SKB_GSO_UDP_TUNNEL = 1 << 10,
	SKB_GSO_UDP_TUNNEL_CSUM = 1 << 11,
	SKB_GSO_PARTIAL = 1 << 12,
	SKB_GSO_TUNNEL_REMCSUM = 1 << 13,
	SKB_GSO_SCTP = 1 << 14,
	SKB_GSO_ESP = 1 << 15,
	SKB_GSO_UDP = 1 << 16,
	SKB_GSO_UDP_L4 = 1 << 17,
	SKB_GSO_FRAGLIST = 1 << 18,
	SKB_GSO_TCP_ACCECN = 1 << 19,

	/* These indirectly map onto the same netdev feature.
	 * If NETIF_F_TSO_MANGLEID is set it may mangle both inner and outer IDs.
	 */
	SKB_GSO_TCP_FIXEDID = 1 << 30,
	SKB_GSO_TCP_FIXEDID_INNER = 1 << 31,
};

/**
 * enum skb_drop_reason - the reasons of skb drops
 *
 * The reason of skb drop, which is used in kfree_skb_reason().
 */
enum skb_drop_reason {
	/**
	 * @SKB_NOT_DROPPED_YET: skb is not dropped yet (used for no-drop case)
	 */
	SKB_NOT_DROPPED_YET = 0,
	/** @SKB_CONSUMED: packet has been consumed */
	SKB_CONSUMED,
	/** @SKB_DROP_REASON_NOT_SPECIFIED: drop reason is not specified */
	SKB_DROP_REASON_NOT_SPECIFIED,
	/**
	 * @SKB_DROP_REASON_NO_SOCKET: no valid socket that can be used.
	 * Reason could be one of three cases:
	 * 1) no established/listening socket found during lookup process
	 * 2) no valid request socket during 3WHS process
	 * 3) no valid child socket during 3WHS process
	 */
	SKB_DROP_REASON_NO_SOCKET,
	/** @SKB_DROP_REASON_SOCKET_CLOSE: socket is close()d */
	SKB_DROP_REASON_SOCKET_CLOSE,
	/** @SKB_DROP_REASON_SOCKET_FILTER: dropped by socket filter */
	SKB_DROP_REASON_SOCKET_FILTER,
	/** @SKB_DROP_REASON_SOCKET_RCVBUFF: socket receive buff is full */
	SKB_DROP_REASON_SOCKET_RCVBUFF,
	/**
	 * @SKB_DROP_REASON_UNIX_DISCONNECT: recv queue is purged when SOCK_DGRAM
	 * or SOCK_SEQPACKET socket re-connect()s to another socket or notices
	 * during send() that the peer has been close()d.
	 */
	SKB_DROP_REASON_UNIX_DISCONNECT,
	/**
	 * @SKB_DROP_REASON_UNIX_SKIP_OOB: Out-Of-Band data is skipped by
	 * recv() without MSG_OOB so dropped.
	 */
	SKB_DROP_REASON_UNIX_SKIP_OOB,
	/** @SKB_DROP_REASON_PKT_TOO_SMALL: packet size is too small */
	SKB_DROP_REASON_PKT_TOO_SMALL,
	/** @SKB_DROP_REASON_TCP_CSUM: TCP checksum error */
	SKB_DROP_REASON_TCP_CSUM,
	/** @SKB_DROP_REASON_UDP_CSUM: UDP checksum error */
	SKB_DROP_REASON_UDP_CSUM,
	/** @SKB_DROP_REASON_NETFILTER_DROP: dropped by netfilter */
	SKB_DROP_REASON_NETFILTER_DROP,
	/**
	 * @SKB_DROP_REASON_OTHERHOST: packet don't belong to current host
	 * (interface is in promisc mode)
	 */
	SKB_DROP_REASON_OTHERHOST,
	/** @SKB_DROP_REASON_IP_CSUM: IP checksum error */
	SKB_DROP_REASON_IP_CSUM,
	/**
	 * @SKB_DROP_REASON_IP_INHDR: there is something wrong with IP header (see
	 * IPSTATS_MIB_INHDRERRORS)
	 */
	SKB_DROP_REASON_IP_INHDR,
	/**
	 * @SKB_DROP_REASON_IP_RPFILTER: IP rpfilter validate failed. see the
	 * document for rp_filter in ip-sysctl.rst for more information
	 */
	SKB_DROP_REASON_IP_RPFILTER,
	/**
	 * @SKB_DROP_REASON_UNICAST_IN_L2_MULTICAST: destination address of L2 is
	 * multicast, but L3 is unicast.
	 */
	SKB_DROP_REASON_UNICAST_IN_L2_MULTICAST,
	/** @SKB_DROP_REASON_XFRM_POLICY: xfrm policy check failed */
	SKB_DROP_REASON_XFRM_POLICY,
	/** @SKB_DROP_REASON_IP_NOPROTO: no support for IP protocol */
	SKB_DROP_REASON_IP_NOPROTO,
	/**
	 * @SKB_DROP_REASON_PROTO_MEM: proto memory limitation, such as
	 * udp packet drop out of udp_memory_allocated.
	 */
	SKB_DROP_REASON_PROTO_MEM,
	/**
	 * @SKB_DROP_REASON_TCP_AUTH_HDR: TCP-MD5 or TCP-AO hashes are met
	 * twice or set incorrectly.
	 */
	SKB_DROP_REASON_TCP_AUTH_HDR,
	/**
	 * @SKB_DROP_REASON_TCP_MD5NOTFOUND: no MD5 hash and one expected,
	 * corresponding to LINUX_MIB_TCPMD5NOTFOUND
	 */
	SKB_DROP_REASON_TCP_MD5NOTFOUND,
	/**
	 * @SKB_DROP_REASON_TCP_MD5UNEXPECTED: MD5 hash and we're not expecting
	 * one, corresponding to LINUX_MIB_TCPMD5UNEXPECTED
	 */
	SKB_DROP_REASON_TCP_MD5UNEXPECTED,
	/**
	 * @SKB_DROP_REASON_TCP_MD5FAILURE: MD5 hash and its wrong, corresponding
	 * to LINUX_MIB_TCPMD5FAILURE
	 */
	SKB_DROP_REASON_TCP_MD5FAILURE,
	/**
	 * @SKB_DROP_REASON_TCP_AONOTFOUND: no TCP-AO hash and one was expected,
	 * corresponding to LINUX_MIB_TCPAOREQUIRED
	 */
	SKB_DROP_REASON_TCP_AONOTFOUND,
	/**
	 * @SKB_DROP_REASON_TCP_AOUNEXPECTED: TCP-AO hash is present and it
	 * was not expected, corresponding to LINUX_MIB_TCPAOKEYNOTFOUND
	 */
	SKB_DROP_REASON_TCP_AOUNEXPECTED,
	/**
	 * @SKB_DROP_REASON_TCP_AOKEYNOTFOUND: TCP-AO key is unknown,
	 * corresponding to LINUX_MIB_TCPAOKEYNOTFOUND
	 */
	SKB_DROP_REASON_TCP_AOKEYNOTFOUND,
	/**
	 * @SKB_DROP_REASON_TCP_AOFAILURE: TCP-AO hash is wrong,
	 * corresponding to LINUX_MIB_TCPAOBAD
	 */
	SKB_DROP_REASON_TCP_AOFAILURE,
	/**
	 * @SKB_DROP_REASON_SOCKET_BACKLOG: failed to add skb to socket backlog (
	 * see LINUX_MIB_TCPBACKLOGDROP)
	 */
	SKB_DROP_REASON_SOCKET_BACKLOG,
	/** @SKB_DROP_REASON_TCP_FLAGS: TCP flags invalid */
	SKB_DROP_REASON_TCP_FLAGS,
	/**
	 * @SKB_DROP_REASON_TCP_ABORT_ON_DATA: abort on data, corresponding to
	 * LINUX_MIB_TCPABORTONDATA
	 */
	SKB_DROP_REASON_TCP_ABORT_ON_DATA,
	/**
	 * @SKB_DROP_REASON_TCP_ZEROWINDOW: TCP receive window size is zero,
	 * see LINUX_MIB_TCPZEROWINDOWDROP
	 */
	SKB_DROP_REASON_TCP_ZEROWINDOW,
	/**
	 * @SKB_DROP_REASON_TCP_OLD_DATA: the TCP data received is already
	 * received before (spurious retrans may happened), see
	 * LINUX_MIB_DELAYEDACKLOST
	 */
	SKB_DROP_REASON_TCP_OLD_DATA,
	/**
	 * @SKB_DROP_REASON_TCP_OVERWINDOW: the TCP data is out of window,
	 * the seq of the first byte exceed the right edges of receive
	 * window
	 */
	SKB_DROP_REASON_TCP_OVERWINDOW,
	/**
	 * @SKB_DROP_REASON_TCP_OFOMERGE: the data of skb is already in the ofo
	 * queue, corresponding to LINUX_MIB_TCPOFOMERGE
	 */
	SKB_DROP_REASON_TCP_OFOMERGE,
	/**
	 * @SKB_DROP_REASON_TCP_RFC7323_PAWS: PAWS check, corresponding to
	 * LINUX_MIB_PAWSESTABREJECTED, LINUX_MIB_PAWSACTIVEREJECTED
	 */
	SKB_DROP_REASON_TCP_RFC7323_PAWS,
	/**
	 * @SKB_DROP_REASON_TCP_RFC7323_PAWS_ACK: PAWS check, old ACK packet.
	 * Corresponds to LINUX_MIB_PAWS_OLD_ACK.
	 */
	SKB_DROP_REASON_TCP_RFC7323_PAWS_ACK,
	/**
	 * @SKB_DROP_REASON_TCP_RFC7323_TW_PAWS: PAWS check, socket is in
	 * TIME_WAIT state.
	 * Corresponds to LINUX_MIB_PAWS_TW_REJECTED.
	 */
	SKB_DROP_REASON_TCP_RFC7323_TW_PAWS,
	/**
	 * @SKB_DROP_REASON_TCP_RFC7323_TSECR: PAWS check, invalid TSEcr.
	 * Corresponds to LINUX_MIB_TSECRREJECTED.
	 */
	SKB_DROP_REASON_TCP_RFC7323_TSECR,
	/** @SKB_DROP_REASON_TCP_LISTEN_OVERFLOW: listener queue full. */
	SKB_DROP_REASON_TCP_LISTEN_OVERFLOW,
	/** @SKB_DROP_REASON_TCP_OLD_SEQUENCE: Old SEQ field (duplicate packet) */
	SKB_DROP_REASON_TCP_OLD_SEQUENCE,
	/** @SKB_DROP_REASON_TCP_INVALID_SEQUENCE: Not acceptable SEQ field. */
	SKB_DROP_REASON_TCP_INVALID_SEQUENCE,
	/**
	 * @SKB_DROP_REASON_TCP_INVALID_END_SEQUENCE:
	 * Not acceptable END_SEQ field.
	 * Corresponds to LINUX_MIB_BEYOND_WINDOW.
	 */
	SKB_DROP_REASON_TCP_INVALID_END_SEQUENCE,
	/**
	 * @SKB_DROP_REASON_TCP_INVALID_ACK_SEQUENCE: Not acceptable ACK SEQ
	 * field because ack sequence is not in the window between snd_una
	 * and snd_nxt
	 */
	SKB_DROP_REASON_TCP_INVALID_ACK_SEQUENCE,
	/** @SKB_DROP_REASON_TCP_RESET: Invalid RST packet */
	SKB_DROP_REASON_TCP_RESET,
	/**
	 * @SKB_DROP_REASON_TCP_INVALID_SYN: Incoming packet has unexpected
	 * SYN flag
	 */
	SKB_DROP_REASON_TCP_INVALID_SYN,
	/** @SKB_DROP_REASON_TCP_CLOSE: TCP socket in CLOSE state */
	SKB_DROP_REASON_TCP_CLOSE,
	/** @SKB_DROP_REASON_TCP_FASTOPEN: dropped by FASTOPEN request socket */
	SKB_DROP_REASON_TCP_FASTOPEN,
	/** @SKB_DROP_REASON_TCP_OLD_ACK: TCP ACK is old, but in window */
	SKB_DROP_REASON_TCP_OLD_ACK,
	/** @SKB_DROP_REASON_TCP_TOO_OLD_ACK: TCP ACK is too old */
	SKB_DROP_REASON_TCP_TOO_OLD_ACK,
	/**
	 * @SKB_DROP_REASON_TCP_ACK_UNSENT_DATA: TCP ACK for data we haven't
	 * sent yet
	 */
	SKB_DROP_REASON_TCP_ACK_UNSENT_DATA,
	/** @SKB_DROP_REASON_TCP_OFO_QUEUE_PRUNE: pruned from TCP OFO queue */
	SKB_DROP_REASON_TCP_OFO_QUEUE_PRUNE,
	/** @SKB_DROP_REASON_TCP_OFO_DROP: data already in receive queue */
	SKB_DROP_REASON_TCP_OFO_DROP,
	/** @SKB_DROP_REASON_IP_OUTNOROUTES: route lookup failed */
	SKB_DROP_REASON_IP_OUTNOROUTES,
	/**
	 * @SKB_DROP_REASON_BPF_CGROUP_EGRESS: dropped by BPF_PROG_TYPE_CGROUP_SKB
	 * eBPF program
	 */
	SKB_DROP_REASON_BPF_CGROUP_EGRESS,
	/** @SKB_DROP_REASON_IPV6DISABLED: IPv6 is disabled on the device */
	SKB_DROP_REASON_IPV6DISABLED,
	/** @SKB_DROP_REASON_NEIGH_CREATEFAIL: failed to create neigh entry */
	SKB_DROP_REASON_NEIGH_CREATEFAIL,
	/** @SKB_DROP_REASON_NEIGH_FAILED: neigh entry in failed state */
	SKB_DROP_REASON_NEIGH_FAILED,
	/** @SKB_DROP_REASON_NEIGH_QUEUEFULL: arp_queue for neigh entry is full */
	SKB_DROP_REASON_NEIGH_QUEUEFULL,
	/** @SKB_DROP_REASON_NEIGH_DEAD: neigh entry is dead */
	SKB_DROP_REASON_NEIGH_DEAD,
	/** @SKB_DROP_REASON_NEIGH_HH_FILLFAIL: failed to fill the device hard header */
	SKB_DROP_REASON_NEIGH_HH_FILLFAIL,
	/** @SKB_DROP_REASON_TC_EGRESS: dropped in TC egress HOOK */
	SKB_DROP_REASON_TC_EGRESS,
	/** @SKB_DROP_REASON_SECURITY_HOOK: dropped due to security HOOK */
	SKB_DROP_REASON_SECURITY_HOOK,
	/**
	 * @SKB_DROP_REASON_QDISC_DROP: dropped by qdisc when packet outputting (
	 * failed to enqueue to current qdisc)
	 */
	SKB_DROP_REASON_QDISC_DROP,
	/**
	 * @SKB_DROP_REASON_QDISC_BURST_DROP: dropped when net.core.qdisc_max_burst
	 * limit is hit.
	 */
	SKB_DROP_REASON_QDISC_BURST_DROP,
	/**
	 * @SKB_DROP_REASON_QDISC_OVERLIMIT: dropped by qdisc when a qdisc
	 * instance exceeds its total buffer size limit.
	 */
	SKB_DROP_REASON_QDISC_OVERLIMIT,
	/**
	 * @SKB_DROP_REASON_QDISC_CONGESTED: dropped by a qdisc AQM algorithm
	 * due to congestion.
	 */
	SKB_DROP_REASON_QDISC_CONGESTED,
	/**
	 * @SKB_DROP_REASON_CAKE_FLOOD: dropped by the flood protection part of
	 * CAKE qdisc AQM algorithm (BLUE).
	 */
	SKB_DROP_REASON_CAKE_FLOOD,
	/**
	 * @SKB_DROP_REASON_FQ_BAND_LIMIT: dropped by fq qdisc when per band
	 * limit is reached.
	 */
	SKB_DROP_REASON_FQ_BAND_LIMIT,
	/**
	 * @SKB_DROP_REASON_FQ_HORIZON_LIMIT: dropped by fq qdisc when packet
	 * timestamp is too far in the future.
	 */
	SKB_DROP_REASON_FQ_HORIZON_LIMIT,
	/**
	 * @SKB_DROP_REASON_FQ_FLOW_LIMIT: dropped by fq qdisc when a flow
	 * exceeds its limits.
	 */
	SKB_DROP_REASON_FQ_FLOW_LIMIT,
	/**
	 * @SKB_DROP_REASON_CPU_BACKLOG: failed to enqueue the skb to the per CPU
	 * backlog queue. This can be caused by backlog queue full (see
	 * netdev_max_backlog in net.rst) or RPS flow limit
	 */
	SKB_DROP_REASON_CPU_BACKLOG,
	/** @SKB_DROP_REASON_XDP: dropped by XDP in input path */
	SKB_DROP_REASON_XDP,
	/** @SKB_DROP_REASON_TC_INGRESS: dropped in TC ingress HOOK */
	SKB_DROP_REASON_TC_INGRESS,
	/** @SKB_DROP_REASON_UNHANDLED_PROTO: protocol not implemented or not supported */
	SKB_DROP_REASON_UNHANDLED_PROTO,
	/** @SKB_DROP_REASON_SKB_CSUM: sk_buff checksum computation error */
	SKB_DROP_REASON_SKB_CSUM,
	/** @SKB_DROP_REASON_SKB_GSO_SEG: gso segmentation error */
	SKB_DROP_REASON_SKB_GSO_SEG,
	/**
	 * @SKB_DROP_REASON_SKB_UCOPY_FAULT: failed to copy data from user space,
	 * e.g., via zerocopy_sg_from_iter() or skb_orphan_frags_rx()
	 */
	SKB_DROP_REASON_SKB_UCOPY_FAULT,
	/** @SKB_DROP_REASON_DEV_HDR: device driver specific header/metadata is invalid */
	SKB_DROP_REASON_DEV_HDR,
	/**
	 * @SKB_DROP_REASON_DEV_READY: the device is not ready to xmit/recv due to
	 * any of its data structure that is not up/ready/initialized,
	 * e.g., the IFF_UP is not set, or driver specific tun->tfiles[txq]
	 * is not initialized
	 */
	SKB_DROP_REASON_DEV_READY,
	/** @SKB_DROP_REASON_FULL_RING: ring buffer is full */
	SKB_DROP_REASON_FULL_RING,
	/** @SKB_DROP_REASON_NOMEM: error due to OOM */
	SKB_DROP_REASON_NOMEM,
	/**
	 * @SKB_DROP_REASON_HDR_TRUNC: failed to trunc/extract the header from
	 * networking data, e.g., failed to pull the protocol header from
	 * frags via pskb_may_pull()
	 */
	SKB_DROP_REASON_HDR_TRUNC,
	/**
	 * @SKB_DROP_REASON_TAP_FILTER: dropped by (ebpf) filter directly attached
	 * to tun/tap, e.g., via TUNSETFILTEREBPF
	 */
	SKB_DROP_REASON_TAP_FILTER,
	/**
	 * @SKB_DROP_REASON_TAP_TXFILTER: dropped by tx filter implemented at
	 * tun/tap, e.g., check_filter()
	 */
	SKB_DROP_REASON_TAP_TXFILTER,
	/** @SKB_DROP_REASON_ICMP_CSUM: ICMP checksum error */
	SKB_DROP_REASON_ICMP_CSUM,
	/**
	 * @SKB_DROP_REASON_INVALID_PROTO: the packet doesn't follow RFC 2211,
	 * such as a broadcasts ICMP_TIMESTAMP
	 */
	SKB_DROP_REASON_INVALID_PROTO,
	/**
	 * @SKB_DROP_REASON_IP_INADDRERRORS: host unreachable, corresponding to
	 * IPSTATS_MIB_INADDRERRORS
	 */
	SKB_DROP_REASON_IP_INADDRERRORS,
	/**
	 * @SKB_DROP_REASON_IP_INNOROUTES: network unreachable, corresponding to
	 * IPSTATS_MIB_INADDRERRORS
	 */
	SKB_DROP_REASON_IP_INNOROUTES,
	/** @SKB_DROP_REASON_IP_LOCAL_SOURCE: the source ip is local */
	SKB_DROP_REASON_IP_LOCAL_SOURCE,
	/**
	 * @SKB_DROP_REASON_IP_INVALID_SOURCE: the source ip is invalid:
	 * 1) source ip is multicast or limited broadcast
	 * 2) source ip is zero and not IGMP
	 */
	SKB_DROP_REASON_IP_INVALID_SOURCE,
	/** @SKB_DROP_REASON_IP_LOCALNET: source or dest ip is local net */
	SKB_DROP_REASON_IP_LOCALNET,
	/**
	 * @SKB_DROP_REASON_IP_INVALID_DEST: the dest ip is invalid:
	 * 1) dest ip is 0
	 */
	SKB_DROP_REASON_IP_INVALID_DEST,
	/**
	 * @SKB_DROP_REASON_PKT_TOO_BIG: packet size is too big (maybe exceed the
	 * MTU)
	 */
	SKB_DROP_REASON_PKT_TOO_BIG,
	/** @SKB_DROP_REASON_DUP_FRAG: duplicate fragment */
	SKB_DROP_REASON_DUP_FRAG,
	/** @SKB_DROP_REASON_FRAG_REASM_TIMEOUT: fragment reassembly timeout */
	SKB_DROP_REASON_FRAG_REASM_TIMEOUT,
	/**
	 * @SKB_DROP_REASON_FRAG_TOO_FAR: ipv4 fragment too far.
	 * (/proc/sys/net/ipv4/ipfrag_max_dist)
	 */
	SKB_DROP_REASON_FRAG_TOO_FAR,
	/**
	 * @SKB_DROP_REASON_TCP_MINTTL: ipv4 ttl or ipv6 hoplimit below
	 * the threshold (IP_MINTTL or IPV6_MINHOPCOUNT).
	 */
	SKB_DROP_REASON_TCP_MINTTL,
	/** @SKB_DROP_REASON_IPV6_BAD_EXTHDR: Bad IPv6 extension header. */
	SKB_DROP_REASON_IPV6_BAD_EXTHDR,
	/** @SKB_DROP_REASON_IPV6_NDISC_FRAG: invalid frag (suppress_frag_ndisc). */
	SKB_DROP_REASON_IPV6_NDISC_FRAG,
	/** @SKB_DROP_REASON_IPV6_NDISC_HOP_LIMIT: invalid hop limit. */
	SKB_DROP_REASON_IPV6_NDISC_HOP_LIMIT,
	/** @SKB_DROP_REASON_IPV6_NDISC_BAD_CODE: invalid NDISC icmp6 code. */
	SKB_DROP_REASON_IPV6_NDISC_BAD_CODE,
	/** @SKB_DROP_REASON_IPV6_NDISC_BAD_OPTIONS: invalid NDISC options. */
	SKB_DROP_REASON_IPV6_NDISC_BAD_OPTIONS,
	/**
	 * @SKB_DROP_REASON_IPV6_NDISC_NS_OTHERHOST: NEIGHBOUR SOLICITATION
	 * for another host.
	 */
	SKB_DROP_REASON_IPV6_NDISC_NS_OTHERHOST,
	/** @SKB_DROP_REASON_QUEUE_PURGE: bulk free. */
	SKB_DROP_REASON_QUEUE_PURGE,
	/**
	 * @SKB_DROP_REASON_TC_COOKIE_ERROR: An error occurred whilst
	 * processing a tc ext cookie.
	 */
	SKB_DROP_REASON_TC_COOKIE_ERROR,
	/**
	 * @SKB_DROP_REASON_PACKET_SOCK_ERROR: generic packet socket errors
	 * after its filter matches an incoming packet.
	 */
	SKB_DROP_REASON_PACKET_SOCK_ERROR,
	/** @SKB_DROP_REASON_TC_CHAIN_NOTFOUND: tc chain lookup failed. */
	SKB_DROP_REASON_TC_CHAIN_NOTFOUND,
	/**
	 * @SKB_DROP_REASON_TC_RECLASSIFY_LOOP: tc exceeded max reclassify loop
	 * iterations.
	 */
	SKB_DROP_REASON_TC_RECLASSIFY_LOOP,
	/**
	 * @SKB_DROP_REASON_VXLAN_INVALID_HDR: VXLAN header is invalid. E.g.:
	 * 1) reserved fields are not zero
	 * 2) "I" flag is not set
	 */
	SKB_DROP_REASON_VXLAN_INVALID_HDR,
	/** @SKB_DROP_REASON_VXLAN_VNI_NOT_FOUND: no VXLAN device found for VNI */
	SKB_DROP_REASON_VXLAN_VNI_NOT_FOUND,
	/** @SKB_DROP_REASON_MAC_INVALID_SOURCE: source mac is invalid */
	SKB_DROP_REASON_MAC_INVALID_SOURCE,
	/**
	 * @SKB_DROP_REASON_VXLAN_ENTRY_EXISTS: trying to migrate a static
	 * entry or an entry pointing to a nexthop.
	 */
	SKB_DROP_REASON_VXLAN_ENTRY_EXISTS,
	/** @SKB_DROP_REASON_NO_TX_TARGET: no target found for xmit */
	SKB_DROP_REASON_NO_TX_TARGET,
	/**
	 * @SKB_DROP_REASON_IP_TUNNEL_ECN: skb is dropped according to
	 * RFC 6040 4.2, see __INET_ECN_decapsulate() for detail.
	 */
	SKB_DROP_REASON_IP_TUNNEL_ECN,
	/**
	 * @SKB_DROP_REASON_TUNNEL_TXINFO: packet without necessary metadata
	 * reached a device which is in "external" mode.
	 */
	SKB_DROP_REASON_TUNNEL_TXINFO,
	/**
	 * @SKB_DROP_REASON_LOCAL_MAC: the source MAC address is equal to
	 * the MAC address of the local netdev.
	 */
	SKB_DROP_REASON_LOCAL_MAC,
	/**
	 * @SKB_DROP_REASON_ARP_PVLAN_DISABLE: packet which is not IP is
	 * forwarded to the in_dev, and the proxy_arp_pvlan is not
	 * enabled.
	 */
	SKB_DROP_REASON_ARP_PVLAN_DISABLE,
	/**
	 * @SKB_DROP_REASON_MAC_IEEE_MAC_CONTROL: the destination MAC address
	 * is an IEEE MAC Control address.
	 */
	SKB_DROP_REASON_MAC_IEEE_MAC_CONTROL,
	/**
	 * @SKB_DROP_REASON_BRIDGE_INGRESS_STP_STATE: the STP state of the
	 * ingress bridge port does not allow frames to be forwarded.
	 */
	SKB_DROP_REASON_BRIDGE_INGRESS_STP_STATE,
	/**
	 * @SKB_DROP_REASON_CAN_RX_INVALID_FRAME: received
	 * non conform CAN frame (or device is unable to receive CAN frames)
	 */
	SKB_DROP_REASON_CAN_RX_INVALID_FRAME,
	/**
	 * @SKB_DROP_REASON_CANFD_RX_INVALID_FRAME: received
	 * non conform CAN-FD frame (or device is unable to receive CAN frames)
	 */
	SKB_DROP_REASON_CANFD_RX_INVALID_FRAME,
	/**
	 * @SKB_DROP_REASON_CANXL_RX_INVALID_FRAME: received
	 * non conform CAN-XL frame (or device is unable to receive CAN frames)
	 */
	SKB_DROP_REASON_CANXL_RX_INVALID_FRAME,
	/**
	 * @SKB_DROP_REASON_PFMEMALLOC: packet allocated from memory reserve
	 * reached a path or socket not eligible for use of memory reserves
	 */
	SKB_DROP_REASON_PFMEMALLOC,
	/**
	 * @SKB_DROP_REASON_DUALPI2_STEP_DROP: dropped by the step drop
	 * threshold of DualPI2 qdisc.
	 */
	SKB_DROP_REASON_DUALPI2_STEP_DROP,
	/** @SKB_DROP_REASON_PSP_INPUT: PSP input checks failed */
	SKB_DROP_REASON_PSP_INPUT,
	/** @SKB_DROP_REASON_PSP_OUTPUT: PSP output checks failed */
	SKB_DROP_REASON_PSP_OUTPUT,
	/**
	 * @SKB_DROP_REASON_MAX: the maximum of core drop reasons, which
	 * shouldn't be used as a real 'reason' - only for tracing code gen
	 */
	SKB_DROP_REASON_MAX,

	/**
	 * @SKB_DROP_REASON_SUBSYS_MASK: subsystem mask in drop reasons,
	 * see &enum skb_drop_reason_subsys
	 */
	SKB_DROP_REASON_SUBSYS_MASK = 0xffff0000,
};

typedef struct skb_frag {
	netmem_ref netmem;
	unsigned int len;
	unsigned int offset;
} skb_frag_t;

struct dev_kfree_skb_cb {
	enum skb_drop_reason reason;
};

struct skb_seq_state {
	__u32		lower_offset;
	__u32		upper_offset;
	__u32		frag_idx;
	__u32		stepped_offset;
	struct sk_buff	*root_skb;
	struct sk_buff	*cur_skb;
	__u8		*frag_data;
	__u32		frag_off;
};

/**
 * struct skb_shared_hwtstamps - hardware time stamps
 * @hwtstamp:		hardware time stamp transformed into duration
 *			since arbitrary point in time
 * @netdev_data:	address/cookie of network device driver used as
 *			reference to actual hardware time stamp
 *
 * Software time stamps generated by ktime_get_real() are stored in
 * skb->tstamp.
 *
 * hwtstamps can only be compared against other hwtstamps from
 * the same device.
 *
 * This structure is attached to packets as part of the
 * &skb_shared_info. Use skb_hwtstamps() to get a pointer.
 */
struct skb_shared_hwtstamps {
	union {
		s64 hwtstamp;
		void *netdev_data;
	};
};

/* Preserve some data across TX submission and completion.
 *
 * Note, this state is stored in the driver. Extending the layout
 * might need some special care.
 */
struct xsk_tx_metadata_compl {
	__u64 *tx_timestamp;
};

/*
 * The callback notifies userspace to release buffers when skb DMA is done in
 * lower device, the skb last reference should be 0 when calling this.
 * The zerocopy_success argument is true if zero copy transmit occurred,
 * false on data copy or out of memory error caused by data copy attempt.
 * The ctx field is used to track device context.
 * The desc field is used to track userspace buffer index.
 */
struct ubuf_info {
	const struct ubuf_info_ops *ops;
	refcount_t refcnt;
	u8 flags;
};

/* This data is invariant across clones and lives at
 * the end of the header data, ie. at skb->end.
 */
struct skb_shared_info {
	__u8		flags; /* in older versions is called '__unused' */
	__u8		meta_len;
	__u8		nr_frags;
	__u8		tx_flags;
	unsigned short	gso_size;
	/* Warning: this field is not always filled in (UFO)! */
	unsigned short	gso_segs;
	struct sk_buff	*frag_list;
	union {
		struct skb_shared_hwtstamps hwtstamps;
		struct xsk_tx_metadata_compl xsk_meta;
	};

	unsigned int	gso_type;
	u32		tskey;

	/*
	 * Warning : all fields before dataref are cleared in __alloc_skb()
	 */
	atomic_int	dataref;
	union {
		struct {
			u32		xdp_frags_size;
			u32		xdp_frags_truesize;
		};

		/*
		 * Intermediate layers must ensure that destructor_arg
		 * remains valid until skb destructor.
		 */
		void		*destructor_arg;
	};

	/* must be last field, see pskb_expand_head() */
	skb_frag_t	frags[MAX_SKB_FRAGS];
};

struct sk_buff_head {
        /* These two members must be first. */
        struct_group_tagged(sk_buff_list, list,
                struct sk_buff  *next;
                struct sk_buff  *prev;
        );

        __u32           qlen;
};

static inline void __net_timestamp(struct sk_buff *skb){
	skb->tstamp_type = SKB_CLOCK_REALTIME;
}

/* disable the PHY timestamp */
static inline void skb_clone_tx_timestamp(struct sk_buff *skb){

}

static inline bool skb_defer_rx_timestamp(struct sk_buff *skb){
	return false;
}

static inline void skb_set_queue_mapping(struct sk_buff *skb, u16 queue_mapping){
	skb->queue_mapping = queue_mapping;
}

static inline u16 skb_get_queue_mapping(const struct sk_buff *skb){
	return skb->queue_mapping;
}

static inline void skb_copy_queue_mapping(struct sk_buff *to, const struct sk_buff *from){
	to->queue_mapping = from->queue_mapping;
}

static inline void skb_record_rx_queue(struct sk_buff *skb, u16 rx_queue){
	skb->queue_mapping = rx_queue + 1;
}

static inline u16 skb_get_rx_queue(const struct sk_buff *skb){
	return skb->queue_mapping - 1;
}

static inline bool skb_rx_queue_recorded(const struct sk_buff *skb){
	return skb->queue_mapping != 0;
}

static inline void skb_set_dst_pending_confirm(struct sk_buff *skb, u32 val){
	skb->dst_pending_confirm = val;
}

static inline bool skb_get_dst_pending_confirm(const struct sk_buff *skb){
	return skb->dst_pending_confirm != 0;
}

static inline unsigned char *skb_tail_pointer(const struct sk_buff *skb){
        #if BITS_PER_LONG > 32
        return skb->head + skb->tail;
        #else
        return skb->tail;
        #endif
}

static inline void *__skb_put(struct sk_buff *skb, unsigned int len){
	void *tmp = skb_tail_pointer(skb);

	skb->tail += len;
	skb->len  += len;
	return tmp;
}

static inline void *__skb_put_zero(struct sk_buff *skb, unsigned int len){
	void *tmp = __skb_put(skb, len);

	memset(tmp, 0, len);
	return tmp;
}

static inline void *__skb_put_data(struct sk_buff *skb, const void *data, unsigned int len){
	void *tmp = __skb_put(skb, len);

	memcpy(tmp, data, len);
	return tmp;
}

static inline void __skb_put_u8(struct sk_buff *skb, u8 val){
	*(u8 *)__skb_put(skb, 1) = val;
}

static inline void *skb_put_zero(struct sk_buff *skb, unsigned int len){
	void *tmp = skb_put(skb, len);

	memset(tmp, 0, len);
	return tmp;
}

static inline void *skb_put_data(struct sk_buff *skb, const void *data,  unsigned int len){
	void *tmp = skb_put(skb, len);

	memcpy(tmp, data, len);
	return tmp;
}

static inline void skb_put_u8(struct sk_buff *skb, u8 val){
	*(u8 *)skb_put(skb, 1) = val;
}

static inline void skb_copy_from_linear_data(const struct sk_buff *skb, void *to, const unsigned int len){
	memcpy(to, skb->data, len);
}

static inline void skb_copy_from_linear_data_offset(const struct sk_buff *skb, const int offset, void *to, const unsigned int len){
	memcpy(to, skb->data + offset, len);
}

static inline void skb_copy_to_linear_data(struct sk_buff *skb, const void *from, const unsigned int len){
	memcpy(skb->data, from, len);
}

static inline void skb_copy_to_linear_data_offset(struct sk_buff *skb, const int offset, const void *from, const unsigned int len){
	memcpy(skb->data + offset, from, len);
}

static inline int __pskb_trim(struct sk_buff *skb, unsigned int len){
	int ret = 0;
	if (skb->data_len){
		ret = ___pskb_trim(skb, len);
		return ret;
	}
	__skb_trim(skb, len);
	return ret;
}

/*
 *	skb_shared - is the buffer shared
 *	@skb: buffer to check
 *
 *	Returns true if more than one person has a reference to this
 *	buffer.
 */
static inline int skb_shared(const struct sk_buff *skb){
	return refcount_read(&skb->users) != 1;
}

/*
 * skb_pfmemalloc - Test if the skb was allocated from PFMEMALLOC reserves
 * @skb: buffer
 */
static inline bool skb_pfmemalloc(const struct sk_buff *skb){
	return unlikely(skb->pfmemalloc);
}

static inline int skb_alloc_rx_flag(const struct sk_buff *skb){
	if (skb_pfmemalloc(skb)){
		return SKB_ALLOC_RX;
	}
	return 0;
}

/*
 * skb_frag_size() - Returns the size of a skb fragment
 * @frag: skb fragment
 */
static inline unsigned int skb_frag_size(const skb_frag_t *frag){
	return frag->len;
}

/*
 * skb_frag_size_set() - Sets the size of a skb fragment
 * @frag: skb fragment
 * @size: size of fragment
 */
static inline void skb_frag_size_set(skb_frag_t *frag, unsigned int size){
	frag->len = size;
}

/*
 * skb_frag_size_add() - Increments the size of a skb fragment by @delta
 * @frag: skb fragment
 * @delta: value to add
 */
static inline void skb_frag_size_add(skb_frag_t *frag, int delta){
	frag->len += delta;
}

/*
 * skb_frag_size_sub() - Decrements the size of a skb fragment by @delta
 * @frag: skb fragment
 * @delta: value to subtract
 */
static inline void skb_frag_size_sub(skb_frag_t *frag, int delta){
	frag->len -= delta;
}

/*
 * skb_frag_off() - Returns the offset of a skb fragment
 * @frag: the paged fragment
 */
static inline unsigned int skb_frag_off(const skb_frag_t *frag){
	return frag->offset;
}

/*
 * skb_frag_off_add() - Increments the offset of a skb fragment by @delta
 * @frag: skb fragment
 * @delta: value to add
 */
static inline void skb_frag_off_add(skb_frag_t *frag, int delta){
	frag->offset += delta;
}

/*
 * skb_frag_off_set() - Sets the offset of a skb fragment
 * @frag: skb fragment
 * @offset: offset of fragment
 */
static inline void skb_frag_off_set(skb_frag_t *frag, unsigned int offset){
	frag->offset = offset;
}

/*
 * skb_frag_off_copy() - Sets the offset of a skb fragment from another fragment
 * @fragto: skb fragment where offset is set
 * @fragfrom: skb fragment offset is copied from
 */
static inline void skb_frag_off_copy(skb_frag_t *fragto, const skb_frag_t *fragfrom){
	fragto->offset = fragfrom->offset;
}

/**
 *	skb_peek - peek at the head of an &sk_buff_head
 *	@list_: list to peek at
 *
 *	Peek an &sk_buff. Unlike most other operations you _MUST_
 *	be careful with this one. A peek leaves the buffer on the
 *	list and someone else may run off with it. You must hold
 *	the appropriate locks or have a private queue to do this.
 *
 *	Returns %NULL for an empty list or a pointer to the head element.
 *	The reference count is not incremented and the reference is therefore
 *	volatile. Use with caution.
 */
static inline struct sk_buff *skb_peek(const struct sk_buff_head *list_){
	struct sk_buff *skb = list_->next;

	if (skb == (struct sk_buff *)list_){
		skb = NULL;
	}
	return skb;
}

static inline unsigned int skb_tail_offset(const struct sk_buff *skb){
#if BITS_PER_LONG > 32
	return skb->tail;
#else
	return (unsigned int)(skb->tail - skb->head);
#endif
}

static inline void skb_set_tail_offset(struct sk_buff *skb, unsigned int tail){
#if BITS_PER_LONG > 32
	skb->tail = tail;
#else
	skb->tail = skb->head + tail;
#endif
}

static inline unsigned char *skb_end_pointer(const struct sk_buff *skb){
	#if BITS_PER_LONG > 32
		return skb->head + skb->end;
	#else
		return skb->end;
	#endif
}

static void skb_reset_tail_pointer(struct sk_buff *skb){
	#if BITS_PER_LONG > 32
	skb->tail = skb->data - skb->head;
	#else
	skb->tail = skb->data;
	#endif
}

static inline void skb_set_tail_pointer(struct sk_buff *skb, const int offset){
	#if BITS_PER_LONG > 32
	skb_reset_tail_pointer(skb);
	skb->tail += offset;
	#else
	skb->tail = skb->data + offset;
	#endif
}

/**
 *	__skb_peek - peek at the head of a non-empty &sk_buff_head
 *	@list_: list to peek at
 *
 *	Like skb_peek(), but the caller knows that the list is not empty.
 */
static inline struct sk_buff *__skb_peek(const struct sk_buff_head *list_){
	return list_->next;
}

static inline unsigned int skb_end_offset(const struct sk_buff *skb){
	#ifdef NET_SKBUFF_DATA_USES_OFFSET
	return skb->end;
	#else
	return (unsigned int)(skb->end - skb->head);
	#endif
}

static inline void skb_set_end_offset(struct sk_buff *skb, unsigned int offset){
	#ifdef NET_SKBUFF_DATA_USES_OFFSET
	skb->end = offset;
	#else
	skb->end = skb->head + offset;
	#endif
}

/**
 *      skb_get - reference buffer
 *      @skb: buffer to reference
 *
 *      Makes another reference to a socket buffer and returns a pointer
 *      to the buffer.
 */
static inline struct sk_buff *skb_get(struct sk_buff *skb){
        refcount_inc(&skb->users);
        return skb;
}

static inline bool skb_has_frag_list(const struct sk_buff *skb){
	return skb_shinfo(skb)->frag_list != NULL;
}

static inline void skb_frag_list_init(struct sk_buff *skb){
	skb_shinfo(skb)->frag_list = NULL;
}

static void skb_clone_fraglist(struct sk_buff *skb){
	struct sk_buff *list = NULL;

	for (list = skb_shinfo(skb)->frag_list; list; list = list->next){
		skb_get(list);
	}
}

/*
 * If users == 1, we are the only owner and can avoid redundant atomic changes.
 */

/**
 *	skb_cloned - is the buffer a clone
 *	@skb: buffer to check
 *
 *	Returns true if the buffer was generated with skb_clone() and is
 *	one of multiple shared copies of the buffer. Cloned buffers are
 *	shared data so must not be written to under normal circumstances.
 */
static inline int skb_cloned(const struct sk_buff *skb){
	#ifdef __clang__
	return skb->cloned;
	#else
	return skb->cloned && (__atomic_load_n(&skb_shinfo(skb)->dataref, __ATOMIC_ACQUIRE) & SKB_DATAREF_MASK) != 1;
	#endif
}

static inline int skb_unclone(struct sk_buff *skb, int pri){
	int ret = 0;
	if (skb_cloned(skb)){
		ret = pskb_expand_head(skb, 0, 0, pri);
		return ret;
	}
	return ret;
}

static struct dev_kfree_skb_cb *get_kfree_skb_cb(const struct sk_buff *skb){
	return (struct dev_kfree_skb_cb *)skb->cb;
}

static void skb_kfree_head(void *head, unsigned int end_offset){
	if(head != NULL){
		kfree(head);
	}
}

static void skb_free_head(struct sk_buff *skb){
	unsigned char *head = skb->head;
	#if 0
	if (skb->head_frag) {
		if (skb_pp_recycle(skb, head)){
			return;
		}
		skb_free_frag(head);
	} else {
		skb_kfree_head(head, skb_end_offset(skb));
	}
	#endif
}

/**
 *	skb_peek_next - peek skb following the given one from a queue
 *	@skb: skb to start from
 *	@list_: list to peek at
 *
 *	Returns %NULL when the end of the list is met or a pointer to the
 *	next element. The reference count is not incremented and the
 *	reference is therefore volatile. Use with caution.
 */
static inline struct sk_buff *skb_peek_next(struct sk_buff *skb, const struct sk_buff_head *list_){
	struct sk_buff *next = skb->next;

	if (next == (struct sk_buff *)list_){
		next = NULL;
	}
	return next;
}

/**
 *	skb_peek_tail - peek at the tail of an &sk_buff_head
 *	@list_: list to peek at
 *
 *	Peek an &sk_buff. Unlike most other operations you _MUST_
 *	be careful with this one. A peek leaves the buffer on the
 *	list and someone else may run off with it. You must hold
 *	the appropriate locks or have a private queue to do this.
 *
 *	Returns %NULL for an empty list or a pointer to the tail element.
 *	The reference count is not incremented and the reference is therefore
 *	volatile. Use with caution.
 */
static inline struct sk_buff *skb_peek_tail(const struct sk_buff_head *list_){
	struct sk_buff *skb = READ_ONCE(list_->prev);

	if (skb == (struct sk_buff *)list_){
		skb = NULL;
	}
	return skb;

}

/**
 *	skb_queue_len	- get queue length
 *	@list_: list to measure
 *
 *	Return the length of an &sk_buff queue.
 */
static inline __u32 skb_queue_len(const struct sk_buff_head *list_){
	return list_->qlen;
}

/**
 *	skb_queue_len_lockless	- get queue length
 *	@list_: list to measure
 *
 *	Return the length of an &sk_buff queue.
 *	This variant can be used in lockless contexts.
 */
static inline __u32 skb_queue_len_lockless(const struct sk_buff_head *list_){
	return READ_ONCE(list_->qlen);
}

/**
 *	__skb_queue_head_init - initialize non-spinlock portions of sk_buff_head
 *	@list: queue to initialize
 *
 *	This initializes only the list and queue length aspects of
 *	an sk_buff_head object.  This allows to initialize the list
 *	aspects of an sk_buff_head without reinitializing things like
 *	the spinlock.  It can also be used for on-stack sk_buff_head
 *	objects where the spinlock is known to not be used.
 */
static inline void __skb_queue_head_init(struct sk_buff_head *list){
	list->prev = list->next = (struct sk_buff *)list;
	list->qlen = 0;
}

/*
 * This function creates a split out lock class for each invocation;
 * this is needed for now since a whole lot of users of the skb-queue
 * infrastructure in drivers have different locking usage (in hardirq)
 * than the networking core (in softirq only). In the long run either the
 * network layer or drivers should need annotation to consolidate the
 * main types of usage into 3 classes.
 */
static inline void skb_queue_head_init(struct sk_buff_head *list){
	__skb_queue_head_init(list);
}

/**
 *	skb_queue_empty - check if a queue is empty
 *	@list: queue head
 *
 *	Returns true if the queue is empty, false otherwise.
 */
static inline int skb_queue_empty(const struct sk_buff_head *list){
	return list->next == (const struct sk_buff *) list;
}

/**
 *	skb_queue_empty_lockless - check if a queue is empty
 *	@list: queue head
 *
 *	Returns true if the queue is empty, false otherwise.
 *	This variant can be used in lockless contexts.
 */
static inline bool skb_queue_empty_lockless(const struct sk_buff_head *list){
	return READ_ONCE(list->next) == (const struct sk_buff *) list;
}

static inline void __skb_insert(struct sk_buff *skb,
				struct sk_buff *prev, struct sk_buff *next,
				struct sk_buff_head *list){
	/* See skb_queue_empty_lockless() and skb_peek_tail()
	 * for the opposite READ_ONCE()
	 *
	 * Use sk_buff_list cast (hardcoded offsets 0/8) for next/prev on skb
	 * nodes, bypassing the dynamic DWARF offsets that fail to resolve these
	 * fields when they are nested inside anonymous unions in sk_buff.
	 */
	WRITE_ONCE(((struct sk_buff_list *)skb)->next, next);
	WRITE_ONCE(((struct sk_buff_list *)skb)->prev, prev);
	WRITE_ONCE(((struct sk_buff_list *)next)->prev, skb);
	WRITE_ONCE(((struct sk_buff_list *)prev)->next, skb);
	WRITE_ONCE(list->qlen, list->qlen + 1);
}

static inline bool skb_frags_readable(const struct sk_buff *skb){
	return !skb->unreadable;
}

static inline void skb_mark_not_on_list(struct sk_buff *skb){
	((struct sk_buff_list *)skb)->next = NULL;
}

static inline bool skb_is_nonlinear(const struct sk_buff *skb){
        return skb->data_len;
}

static inline unsigned int skb_headlen(const struct sk_buff *skb){
        return skb->len - skb->data_len;
}

static inline void skb_list_del_init(struct sk_buff *skb){
	__list_del_entry(&skb->list);
	skb_mark_not_on_list(skb);
}

/**
 *	skb_headroom - bytes at buffer head
 *	@skb: buffer to check
 *
 *	Return the number of bytes of free space at the head of an &sk_buff.
 */
static inline unsigned int skb_headroom(const struct sk_buff *skb){
	return skb->data - skb->head;
}

static inline int skb_checksum_start_offset(const struct sk_buff *skb){
	return skb->csum_start - skb_headroom(skb);
}

static inline unsigned char *skb_checksum_start(const struct sk_buff *skb){
	return skb->head + skb->csum_start;
}

/**
 *	skb_tailroom - bytes at buffer end
 *	@skb: buffer to check
 *
 *	Return the number of bytes of free space at the tail of an sk_buff
 */
static inline int skb_tailroom(const struct sk_buff *skb){
	return skb_is_nonlinear(skb) ? 0 : skb->end - skb->tail;
}

/**
 *	skb_availroom - bytes at buffer end
 *	@skb: buffer to check
 *
 *	Return the number of bytes of free space at the tail of an sk_buff
 *	allocated by sk_stream_alloc()
 */
static inline int skb_availroom(const struct sk_buff *skb){
	if (skb_is_nonlinear(skb)){
		return 0;
	}
	return skb->end - skb->tail - skb->reserved_tailroom;
}

/**
 *	skb_reserve - adjust headroom
 *	@skb: buffer to alter
 *	@len: bytes to move
 *
 *	Increase the headroom of an empty &sk_buff by reducing the tail
 *	room. This is only allowed for an empty buffer.
 */
static inline void skb_reserve(struct sk_buff *skb, int len){
	skb->data += len;
	skb->tail += len;
}

/**
 *      __skb_queue_after - queue a buffer at the list head
 *      @list: list to use
 *      @prev: place after this buffer
 *      @newsk: buffer to queue
 *
 *      Queue a buffer int the middle of a list. This function takes no locks
 *      and you must therefore hold required locks before calling it.
 *
 *      A buffer cannot be placed on two lists at the same time.
 */
static inline void __skb_queue_after(struct sk_buff_head *list, struct sk_buff *prev, struct sk_buff *newsk){
        __skb_insert(newsk, prev, ((struct sk_buff_list *)prev)->next, list);
}

static inline void __skb_queue_before(struct sk_buff_head *list,
				      struct sk_buff *next,
				      struct sk_buff *newsk){
	__skb_insert(newsk, ((struct sk_buff_list *)next)->prev, next, list);
}

static inline void __skb_queue_tail(struct sk_buff_head *list, struct sk_buff *newsk){
	__skb_queue_before(list, (struct sk_buff *)list, newsk);
}

static inline void __skb_unlink(struct sk_buff *skb, struct sk_buff_head *list){
	struct sk_buff *next = ((struct sk_buff_list *)skb)->next;
	struct sk_buff *prev = ((struct sk_buff_list *)skb)->prev;
	((struct sk_buff_list *)skb)->next = NULL;
	((struct sk_buff_list *)skb)->prev = NULL;
	WRITE_ONCE(((struct sk_buff_list *)next)->prev, prev);
	WRITE_ONCE(((struct sk_buff_list *)prev)->next, next);
	WRITE_ONCE(list->qlen, list->qlen - 1);
}

static inline struct sk_buff *__skb_dequeue(struct sk_buff_head *list){
	struct sk_buff *skb = list->next;
	if (skb == (struct sk_buff *)list){
		return NULL;
	}
	__skb_unlink(skb, list);
	return skb;
}

static inline struct sk_buff *__skb_dequeue_tail(struct sk_buff_head *list){
	struct sk_buff *skb = list->prev;
	if (skb == (struct sk_buff *)list){
		return NULL;
	}
	__skb_unlink(skb, list);
	return skb;
}

static void skb_panic(struct sk_buff *skb, unsigned int sz, void *addr, const char msg[]){
	printf("%s: text:%px len:%d put:%d head:%px data:%px tail:%#lx end:%#lx dev:%s\n",
		msg,
		addr,
		skb->len,
		sz,
		skb->head,
		skb->data,
		(unsigned long)skb->tail,
		(unsigned long)skb->end,
		"<NULL>"
	);
}

static void skb_over_panic(struct sk_buff *skb, unsigned int sz, void *addr){
	skb_panic(skb, sz, addr, __func__);
}

static void skb_under_panic(struct sk_buff *skb, unsigned int sz, void *addr){
	printf("[!!] detected skb panic!...\n");
	skb_panic(skb, sz, addr, __func__);
}

static inline bool skb_unref(struct sk_buff *skb){
	int ret = 0;
	if (unlikely(skb == NULL)){
		ret = false;
		return ret;
	}

	if (likely(!refcount_dec_and_test(&skb->users))){
		ret = false;
		return ret;
	}

	ret = true;
	return ret;
}

static inline bool __sk_skb_reason_drop(struct sock *sk, struct sk_buff *skb, int reason){
        if (unlikely(!skb_unref(skb))){
                return false;
        }

        return true;
}

static inline void *__skb_pull(struct sk_buff *skb, unsigned int len){
	skb->len -= len;
	if (unlikely(skb->len < skb->data_len)) {
		/* do nothing for now */
	}
	return skb->data += len;
}

static inline void *skb_pull_inline(struct sk_buff *skb, unsigned int len){
	return unlikely(len > skb->len) ? NULL : __skb_pull(skb, len);
}

static inline void __skb_set_length(struct sk_buff *skb, unsigned int len){
	if (skb_is_nonlinear(skb) == true){
		return;
	}
	skb->len = len;
	skb_set_tail_pointer(skb, len);
}

static inline void __nf_copy(struct sk_buff *dst, const struct sk_buff *src, bool copy){

}

static inline void nf_copy(struct sk_buff *dst, const struct sk_buff *src){

}

static inline bool skb_is_gso(const struct sk_buff *skb){
	return skb_shinfo(skb)->gso_size;
}

/* Note: Should be called only if skb_is_gso(skb) is true */
static inline bool skb_is_gso_v6(const struct sk_buff *skb){
	return skb_shinfo(skb)->gso_type & SKB_GSO_TCPV6;
}

/* Note: Should be called only if skb_is_gso(skb) is true */
static inline bool skb_is_gso_sctp(const struct sk_buff *skb){
	return skb_shinfo(skb)->gso_type & SKB_GSO_SCTP;
}

/* Note: Should be called only if skb_is_gso(skb) is true */
static inline bool skb_is_gso_tcp(const struct sk_buff *skb){
	return skb_shinfo(skb)->gso_type & (SKB_GSO_TCPV4 | SKB_GSO_TCPV6);
}

static inline struct sk_buff *__pskb_copy(struct sk_buff *skb, int headroom, int gfp_mask){
        return __pskb_copy_fclone(skb, headroom, gfp_mask, false);
}

static inline struct sk_buff *pskb_copy(struct sk_buff *skb, int gfp_mask){
	return __pskb_copy(skb, skb_headroom(skb), gfp_mask);
}

static inline enum skb_drop_reason pskb_may_pull_reason(struct sk_buff *skb, unsigned int len){
	if (likely(len <= skb_headlen(skb))){
		return SKB_NOT_DROPPED_YET;
	}
	if (unlikely(len > skb->len)){
		return SKB_DROP_REASON_PKT_TOO_SMALL;
	}
	if (unlikely(!__pskb_pull_tail(skb, len - skb_headlen(skb)))){
		return SKB_DROP_REASON_NOMEM;
	}
	return SKB_NOT_DROPPED_YET;
}

static inline bool pskb_may_pull(struct sk_buff *skb, unsigned int len){
	return pskb_may_pull_reason(skb, len) == SKB_NOT_DROPPED_YET;
}

static inline void *pskb_pull(struct sk_buff *skb, unsigned int len){
	if (!pskb_may_pull(skb, len)){
		return NULL;
	}
	skb->len -= len;
	return skb->data += len;
}

static inline void __skb_dst_copy(struct sk_buff *skb, unsigned long refdst){
	skb->slow_gro |= !!refdst;
	skb->_skb_refdst = refdst;
	if (!(skb->_skb_refdst & SKB_DST_NOREF)){
		//dst_clone(skb_dst(skb));
	}
}

static inline void skb_dst_copy(struct sk_buff *nskb, const struct sk_buff *skb){
	__skb_dst_copy(nskb, skb->_skb_refdst);
}

static inline void __skb_ext_copy(struct sk_buff *dst, const struct sk_buff *src){
	#ifdef CONFIG_SKB_EXTENSIONS
	dst->active_extensions = src->active_extensions;

	if (src->active_extensions) {
		struct skb_ext *ext = src->extensions;

		refcount_inc(&ext->refcnt);
		dst->extensions = ext;
	}
	#else
	/* do nothing */
	#endif
}

static void __copy_skb_header(struct sk_buff *new, const struct sk_buff *old){
	new->tstamp		= old->tstamp;
	/* We do not copy old->sk */
	new->dev		= old->dev;

	//memcpy(new->cb, old->cb, 48);

	/* Note : this field could be in the headers group.
	 * It is not yet because we do not want to have a 16 bit hole
	 */
	new->queue_mapping = old->queue_mapping;

	new->pkt_type = old->pkt_type;
	new->protocol = old->protocol;
	new->csum = old->csum;
	new->hash = old->hash;

	CHECK_SKB_FIELD(protocol);
	CHECK_SKB_FIELD(csum);
	CHECK_SKB_FIELD(hash);
	CHECK_SKB_FIELD(priority);
	CHECK_SKB_FIELD(skb_iif);
	CHECK_SKB_FIELD(vlan_proto);
	CHECK_SKB_FIELD(vlan_tci);
	CHECK_SKB_FIELD(transport_header);
	CHECK_SKB_FIELD(network_header);
	CHECK_SKB_FIELD(mac_header);
	CHECK_SKB_FIELD(inner_protocol);
	CHECK_SKB_FIELD(inner_transport_header);
	CHECK_SKB_FIELD(inner_network_header);
	CHECK_SKB_FIELD(inner_mac_header);
	CHECK_SKB_FIELD(mark);
#ifdef CONFIG_NETWORK_SECMARK
	CHECK_SKB_FIELD(secmark);
#endif
#ifdef CONFIG_NET_RX_BUSY_POLL
	CHECK_SKB_FIELD(napi_id);
#endif
	CHECK_SKB_FIELD(alloc_cpu);
#ifdef CONFIG_XPS
	CHECK_SKB_FIELD(sender_cpu);
#endif
#ifdef CONFIG_NET_SCHED
	CHECK_SKB_FIELD(tc_index);
#endif
}

/*
 * You should not add any new code to this function.  Add it to
 * __copy_skb_header above instead.
 */
static struct sk_buff *__skb_clone(struct sk_buff *n, struct sk_buff *skb){
	#define C(x) n->x = skb->x

	n->next = n->prev = NULL;
	n->sk = NULL;
	__copy_skb_header(n, skb);

	C(len);
	C(data_len);
	C(mac_len);
	n->hdr_len = skb->nohdr ? skb_headroom(skb) : skb->hdr_len;
	n->cloned = 1;
	n->nohdr = 0;
	n->peeked = 0;
	C(pfmemalloc);
	C(pp_recycle);
	n->destructor = NULL;
	C(tail);
	C(end);
	C(head);
	C(head_frag);
	C(data);
	C(truesize);
	refcount_set(&n->users, 1);

	atomic_fetch_add_explicit(&(skb_shinfo(skb)->dataref), 1, memory_order_release);
	skb->cloned = 1;

	return n;
	#undef C
}

static inline void skb_set_inner_protocol(struct sk_buff *skb, __be16 protocol){
	skb->inner_protocol = protocol;
	skb->inner_protocol_type = ENCAP_TYPE_ETHER;
}

static inline void skb_set_inner_ipproto(struct sk_buff *skb, __u8 ipproto){
	skb->inner_ipproto = ipproto;
	skb->inner_protocol_type = ENCAP_TYPE_IPPROTO;
}

static inline void skb_reset_inner_headers(struct sk_buff *skb){
	skb->inner_mac_header = skb->mac_header;
	skb->inner_network_header = skb->network_header;
	skb->inner_transport_header = skb->transport_header;
}

static inline int skb_mac_header_was_set(const struct sk_buff *skb){
	return skb->mac_header != 0;
}

static inline unsigned char *skb_transport_header(const struct sk_buff *skb){
	return skb->head + skb->transport_header;
}

static inline struct udphdr *udp_hdr(const struct sk_buff *skb){
	return (struct udphdr *)skb_transport_header(skb);
}

static inline void __skb_queue_splice(const struct sk_buff_head *list, struct sk_buff *prev, struct sk_buff *next){
	struct sk_buff *first = list->next;
	struct sk_buff *last = list->prev;

	WRITE_ONCE(first->prev, prev);
	WRITE_ONCE(prev->next, first);
	WRITE_ONCE(last->next, next);
	WRITE_ONCE(next->prev, last);
}

/**
 *	skb_queue_splice_init - join two skb lists and reinitialise the emptied list
 *	@list: the new list to add
 *	@head: the place to add it in the first list
 *
 *	The list at @list is reinitialised
 */
static inline void skb_queue_splice_init(struct sk_buff_head *list, struct sk_buff_head *head){
	if (!skb_queue_empty(list)) {
		__skb_queue_splice(list, (struct sk_buff *) head, head->next);
		head->qlen += list->qlen;
		__skb_queue_head_init(list);
	}
}

/**
 *	skb_queue_splice_tail - join two skb lists, each list being a queue
 *	@list: the new list to add
 *	@head: the place to add it in the first list
 */
static inline void skb_queue_splice_tail(const struct sk_buff_head *list, struct sk_buff_head *head){
	if (!skb_queue_empty(list)) {
		__skb_queue_splice(list, head->prev, (struct sk_buff *) head);
		head->qlen += list->qlen;
	}
}

static inline void kfree_skb_reason(struct sk_buff *skb, int reason){
	sk_skb_reason_drop(NULL, skb, reason);
}

/**
 *	__skb_queue_purge_reason - empty a list
 *	@list: list to empty
 *	@reason: drop reason
 *
 *	Delete all buffers on an &sk_buff list. Each buffer is removed from
 *	the list and one reference dropped. This function does not take the
 *	list lock and the caller must hold the relevant locks to use it.
 */
static inline void __skb_queue_purge_reason(struct sk_buff_head *list, enum skb_drop_reason reason){
	struct sk_buff *skb = NULL;

	while ((skb = __skb_dequeue(list)) != NULL){
		kfree_skb_reason(skb, reason);
	}
}

static inline void skb_queue_purge(struct sk_buff_head *list){
	skb_queue_purge_reason(list, SKB_DROP_REASON_QUEUE_PURGE);
}

static inline struct ubuf_info *skb_zcopy(struct sk_buff *skb){
	bool is_zcopy = skb && skb_shinfo(skb)->flags & SKBFL_ZEROCOPY_ENABLE;
	return is_zcopy ? skb_uarg(skb) : NULL;
}

static inline bool skb_zcopy_pure(const struct sk_buff *skb){
	return skb_shinfo(skb)->flags & SKBFL_PURE_ZEROCOPY;
}

static inline bool skb_zcopy_managed(const struct sk_buff *skb){
	return skb_shinfo(skb)->flags & SKBFL_MANAGED_FRAG_REFS;
}

static inline bool skb_pure_zcopy_same(const struct sk_buff *skb1, const struct sk_buff *skb2){
	return skb_zcopy_pure(skb1) == skb_zcopy_pure(skb2);
}

static inline void skb_zcopy_downgrade_managed(struct sk_buff *skb){
	if (unlikely(skb_zcopy_managed(skb))){
		__skb_zcopy_downgrade_managed(skb);
	}
}

static inline void net_zcopy_get(struct ubuf_info *uarg){
	refcount_inc(&uarg->refcnt);
}

static inline void skb_zcopy_init(struct sk_buff *skb, struct ubuf_info *uarg){
	skb_shinfo(skb)->destructor_arg = uarg;
	skb_shinfo(skb)->flags |= uarg->flags;
}

static inline void skb_zcopy_set(struct sk_buff *skb, struct ubuf_info *uarg, bool *have_ref){
	if (skb && uarg && !skb_zcopy(skb)) {
		if (unlikely(have_ref && *have_ref)){
			*have_ref = false;
		}else{
			net_zcopy_get(uarg);
		}
		skb_zcopy_init(skb, uarg);
	}
}

static int skb_zerocopy_clone(struct sk_buff *nskb, struct sk_buff *orig, int gfp_mask){
	int ret = 0;
	if (skb_zcopy(orig)) {
		if (skb_zcopy(nskb)) {
			if (skb_uarg(nskb) == skb_uarg(orig)){
				return ret;
			}
			if (skb_copy_ubufs(nskb, GFP_ATOMIC)){
				ret = -EIO;
				return ret;
			}
		}
		skb_zcopy_set(nskb, skb_uarg(orig), NULL);
	}
	return 0;
}

/* Release a reference on a zerocopy structure */
static inline void skb_zcopy_clear(struct sk_buff *skb, bool zerocopy_success){
	struct ubuf_info *uarg = NULL;

	uarg = skb_zcopy(skb);
	if (uarg) {
		skb_shinfo(skb)->flags &= ~SKBFL_ALL_ZEROCOPY;
	}
}

/**
 *	skb_orphan - orphan a buffer
 *	@skb: buffer to orphan
 *
 *	If a buffer currently has an owner then we call the owner's
 *	destructor function and make the @skb unowned. The buffer continues
 *	to exist but is no longer charged to its former owner.
 */
static inline void skb_orphan(struct sk_buff *skb){
	void (*destructor)(struct sk_buff *skb) = NULL;

	if (skb->destructor) {
		destructor = skb->destructor;
		destructor(skb);
		skb->destructor = NULL;
		skb->sk		= NULL;
	}
}

/**
 *	skb_orphan_frags - orphan the frags contained in a buffer
 *	@skb: buffer to orphan frags from
 *	@gfp_mask: allocation mask for replacement pages
 *
 *	For each frag in the SKB which needs a destructor (i.e. has an
 *	owner) create a copy of that frag and release the original
 *	page by calling the destructor.
 */
static inline int skb_orphan_frags(struct sk_buff *skb, int gfp_mask){
	if (likely(!skb_zcopy(skb))){
		return 0;
	}
	if (skb_shinfo(skb)->flags & SKBFL_DONT_ORPHAN){
		return 0;
	}
	return skb_copy_ubufs(skb, gfp_mask);
}

/* Frags must be orphaned, even if refcounted, if skb might loop to rx path */
static inline int skb_orphan_frags_rx(struct sk_buff *skb, int gfp_mask){
	if (likely(!skb_zcopy(skb))){
		return 0;
	}
	return skb_copy_ubufs(skb, gfp_mask);
}

/**
 * skb_frag_page - retrieve the page referred to by a paged fragment
 * @frag: the paged fragment
 *
 * Returns the &struct page associated with @frag.
 */
static inline void *skb_frag_page(const skb_frag_t *frag){
	return frag->netmem;
}

/**
 * skb_frag_netmem - retrieve the netmem referred to by a fragment
 * @frag: the fragment
 *
 * Return: the &netmem_ref associated with @frag.
 */
static inline netmem_ref skb_frag_netmem(const skb_frag_t *frag){
	return frag->netmem;
}

static inline void skb_fill_netmem_desc(struct sk_buff *skb, int i, netmem_ref netmem, int off, int size){
	//__skb_fill_netmem_desc(skb, i, netmem, off, size);
	skb_shinfo(skb)->nr_frags = i + 1;
}

/**
 * __skb_frag_ref - take an addition reference on a paged fragment.
 * @frag: the paged fragment
 *
 * Takes an additional reference on the paged fragment @frag.
 */
static inline void __skb_frag_ref(skb_frag_t *frag){
	get_netmem(skb_frag_netmem(frag));
}

/**
 * skb_frag_ref - take an addition reference on a paged fragment of an skb.
 * @skb: the buffer
 * @f: the fragment offset.
 *
 * Takes an additional reference on the @f'th paged fragment of @skb.
 */
static inline void skb_frag_ref(struct sk_buff *skb, int f){
	__skb_frag_ref(&skb_shinfo(skb)->frags[f]);
}

static inline void *__slab_build_skb(void *data, unsigned int *size){
	void *resized = NULL;

	if(data == NULL || size == 0){
		return NULL;
	}
	/* krealloc() will immediately return "data" when
	 * "ksize(data)" is requested: it is the existing upper
	 * bounds. As a result, GFP_ATOMIC will be ignored. Note
	 * that this "new" pointer needs to be passed back to the
	 * caller for use so the __alloc_size hinting will be
	 * tracked correctly.
	 */
	resized = realloc(data, *size);
	return resized;
}

static inline void __finalize_skb_around(struct sk_buff *skb, void *data, unsigned int size){
	struct skb_shared_info *shinfo = NULL;

	size -= SKB_DATA_ALIGN(sizeof(struct skb_shared_info));

	/* Assumes caller memset cleared SKB */
	skb->truesize = SKB_TRUESIZE(size);
	refcount_set(&skb->users, 1);
	skb->head = data;
	skb->data = data;
	skb_reset_tail_pointer(skb);
	skb_set_end_offset(skb, size);
	skb->mac_header = 0;
	skb->transport_header = 0;
	skb->alloc_cpu = 0;
	/* make sure we initialize shinfo sequentially */
	shinfo = skb_shinfo(skb);
	memset(shinfo, 0, sizeof(struct skb_shared_info));
	__atomic_store_n(&shinfo->dataref, 1, __ATOMIC_RELEASE);
}

/* Caller must provide SKB that is memset cleared */
static void __build_skb_around(struct sk_buff *skb, void *data, unsigned int frag_size){
	unsigned int size = frag_size;

	/* frag_size == 0 is considered deprecated now. Callers
	 * using slab buffer should use slab_build_skb() instead.
	 */
	if (size == 0){
		return;
	}
	data = __slab_build_skb(data, &size);
	__finalize_skb_around(skb, data, size);
}

static struct sk_buff *__napi_build_skb(void *data, unsigned int frag_size){
	struct sk_buff *skb = NULL;

	skb = (struct sk_buff *)malloc(SK_BUFF_STRUCT_SIZE);
	if (unlikely(!skb)){
		return NULL;
	}
	memset(skb, 0, SK_BUFF_STRUCT_SIZE);
	__build_skb_around(skb, data, frag_size);

	return skb;
}

/* udp and checksum related functions */

static inline __sum16 udp_v6_check(int len, struct in6_addr *saddr, struct in6_addr *daddr, __wsum base){
	return csum_ipv6_magic(saddr, daddr, len, IPPROTO_UDP, base);
}

static __always_inline __wsum csum_unfold(__sum16 n){
	return (__force __wsum)n;
}

static inline unsigned short csum_from32to16(unsigned int sum){
	sum += (sum >> 16) | (sum << 16);
	return (unsigned short)(sum >> 16);
}

static unsigned int do_csum(const unsigned char *buff, int len){
	int odd = 0;
	unsigned int result = 0;

	if (len <= 0){
		goto out;
	}
	odd = 1 & (unsigned long) buff;
	if (odd) {
#if __BYTE_ORDER == __LITTLE_ENDIAN
		result += (*buff << 8);
#else
		result = *buff;
#endif
		len--;
		buff++;
	}
	if (len >= 2) {
		if (2 & (unsigned long) buff) {
			result += *(unsigned short *) buff;
			len -= 2;
			buff += 2;
		}
		if (len >= 4) {
			const unsigned char *end = buff + ((unsigned)len & ~3);
			unsigned int carry = 0;
			do {
				unsigned int w = *(unsigned int *) buff;
				buff += 4;
				result += carry;
				result += w;
				carry = (w > result);
			} while (buff < end);
			result += carry;
			result = (result & 0xffff) + (result >> 16);
		}
		if (len & 2) {
			result += *(unsigned short *) buff;
			buff += 2;
		}
	}
	if (len & 1)
#if __BYTE_ORDER == __LITTLE_ENDIAN
		result += *buff;
#else
		result += (*buff << 8);
#endif
	result = csum_from32to16(result);
	if (odd)
		result = ((result >> 8) & 0xff) | ((result & 0xff) << 8);
out:
	return result;
}

/* Local Checksum Offload.
 * Compute outer checksum based on the assumption that the
 * inner checksum will be offloaded later.
 * See Documentation/networking/checksum-offloads.rst for
 * explanation of how this works.
 * Fill in outer checksum adjustment (e.g. with sum of outer
 * pseudo-header) before calling.
 * Also ensure that inner checksum is in linear data area.
 */
static inline __wsum lco_csum(struct sk_buff *skb){
	unsigned char *csum_start = skb_checksum_start(skb);
	unsigned char *l4_hdr = skb_transport_header(skb);
	__wsum partial = 0;

	/* Start with complement of inner checksum adjustment */
	partial = ~csum_unfold(*(__force __sum16 *)(csum_start +
						    skb->csum_offset));

	/* Add in checksum of our headers (incl. outer checksum
	 * adjustment filled in by caller) and return result.
	 */
	return csum_partial(l4_hdr, csum_start - l4_hdr, partial);
}

/* place here also the 'sock' functions */
static inline void sock_put(struct sock *sk){
        if (refcount_dec_and_test(&sk->sk_refcnt)){
                sk_free(sk);
        }
}
#endif
