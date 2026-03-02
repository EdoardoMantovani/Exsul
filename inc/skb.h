#ifndef __SKB_H__
#define __SKB_H__

#if BITS_PER_LONG > 32
#define NET_SKBUFF_DATA_USES_OFFSET 1
#endif

#ifdef NET_SKBUFF_DATA_USES_OFFSET
typedef unsigned int sk_buff_data_t;
#else
typedef unsigned char *sk_buff_data_t;
#endif

#define ENCAP_TYPE_ETHER	0
#define ENCAP_TYPE_IPPROTO	1

/* Don't change this without changing skb_csum_unnecessary! */
#define CHECKSUM_NONE		0
#define CHECKSUM_UNNECESSARY	1
#define CHECKSUM_COMPLETE	2
#define CHECKSUM_PARTIAL	3

#define CHECK_SKB_FIELD(x)

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

#define skb_shinfo(SKB)	((struct skb_shared_info *)(skb_end_pointer(SKB)))

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

static inline unsigned char *skb_end_pointer(const struct sk_buff *skb){
	#if BITS_PER_LONG > 32
		return *(unsigned char *)skb->head + *(sk_buff_data_t *)skb->end;
	#else
		return *(sk_buff_data_t *)skb->end;
	#endif
}

static void skb_reset_tail_pointer(struct sk_buff *skb){
	#if BITS_PER_LONG > 32
	*(sk_buff_data_t *)skb->tail = *(unsigned char *)skb->data - *(unsigned char *)skb->head;
	#else
	*(sk_buff_data_t *)skb->tail = *(unsigned char *)skb->data;
	#endif
}

static inline void skb_set_tail_pointer(struct sk_buff *skb, const int offset){
	#if BITS_PER_LONG > 32
	skb_reset_tail_pointer(skb);
	*(sk_buff_data_t *)skb->tail += offset;
	#else
	*(sk_buff_data_t *)skb->tail = *(unsigned char *)skb->data + offset;
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

static inline void __skb_insert(struct sk_buff *skb,
				struct sk_buff *prev, struct sk_buff *next,
				struct sk_buff_head *list){
	/* See skb_queue_empty_lockless() and skb_peek_tail()
	 * for the opposite READ_ONCE()
	 */
	//WRITE_ONCE(skb->next, next);
	//WRITE_ONCE(skb->prev, prev);
	//WRITE_ONCE(((struct sk_buff_list *)next)->prev, skb);
	//WRITE_ONCE(((struct sk_buff_list *)prev)->next, skb);
	//WRITE_ONCE(list->qlen, list->qlen + 1);
}

static inline bool skb_frags_readable(const struct sk_buff *skb){
	return !skb->unreadable;
}

static inline void skb_mark_not_on_list(struct sk_buff *skb){
	*(void **)skb->next = NULL;
}

static inline bool skb_is_nonlinear(const struct sk_buff *skb){
        return skb->data_len;
}

static inline unsigned int skb_headlen(const struct sk_buff *skb){
        return skb->len - skb->data_len;
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
//	return skb->end - skb->tail - skb->reserved_tailroom;
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
	*(unsigned char *)skb->data += len;
	*(sk_buff_data_t *)skb->tail += len;
}

static inline void __skb_queue_before(struct sk_buff_head *list,
				      struct sk_buff *next,
				      struct sk_buff *newsk){
	__skb_insert(newsk, ((struct sk_buff_list *)next)->prev, next, list);
}

static inline void __skb_queue_tail(struct sk_buff_head *list, struct sk_buff *newsk){
	__skb_queue_before(list, (struct sk_buff *)list, newsk);
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
	skb_panic(skb, sz, addr, __func__);
}

static inline bool skb_unref(struct sk_buff *skb){
	int ret = 0;
	if (unlikely(skb == NULL)){
		ret = false;
		return ret;
	}

	if (likely(!refcount_dec_and_test(skb->users))){
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
	*(int *)skb->len -= len;
	if (unlikely(skb->len < skb->data_len)) {
		/* do nothing for now */
	}
	return *(unsigned char *)skb->data += len;
}

static inline void *skb_pull_inline(struct sk_buff *skb, unsigned int len){
	return unlikely(len > *(int *)skb->len) ? NULL : __skb_pull(skb, len);
}

static inline void __skb_set_length(struct sk_buff *skb, unsigned int len){
	if (skb_is_nonlinear(skb) == true){
		return;
	}
	*(int *)skb->len = len;
	skb_set_tail_pointer(skb, len);
}

static inline void __skb_unlink(struct sk_buff *skb, struct sk_buff_head *list){
	struct sk_buff *next = NULL;
	struct sk_buff *prev = NULL;

	WRITE_ONCE(list->qlen, list->qlen - 1);
	next	   = *(void **)skb->next;
	prev	   = *(void **)skb->prev;
	*(void **)skb->next  = NULL;
	*(void **)skb->prev  = NULL;
//	WRITE_ONCE(next->prev, prev);
//	WRITE_ONCE(prev->next, next);
}

static void __copy_skb_header(struct sk_buff *new, const struct sk_buff *old){
	new->tstamp		= old->tstamp;
	/* We do not copy old->sk */
	new->dev		= old->dev;
	memcpy(new->cb, old->cb, sizeof(old->cb));
	skb_dst_copy(new, old);
	__skb_ext_copy(new, old);
	__nf_copy(new, old, false);

	/* Note : this field could be in the headers group.
	 * It is not yet because we do not want to have a 16 bit hole
	 */
	new->queue_mapping = old->queue_mapping;

	memcpy(&new->headers, &old->headers, sizeof(new->headers));
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

static inline void __skb_insert(struct sk_buff *newsk, struct sk_buff *prev, struct sk_buff *next, struct sk_buff_head *list){
	/* See skb_queue_empty_lockless() and skb_peek_tail()
	 * for the opposite READ_ONCE()
	 */
	WRITE_ONCE(newsk->next, next);
	WRITE_ONCE(newsk->prev, prev);
	WRITE_ONCE(((struct sk_buff_list *)next)->prev, newsk);
	WRITE_ONCE(((struct sk_buff_list *)prev)->next, newsk);
	WRITE_ONCE(list->qlen, list->qlen + 1);
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

#endif
