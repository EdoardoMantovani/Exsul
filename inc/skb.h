#ifndef __SKB_H__
#define __SKB_H__

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

static inline void __skb_insert(struct sk_buff *newsk,
				struct sk_buff *prev, struct sk_buff *next,
				struct sk_buff_head *list){
	/* See skb_queue_empty_lockless() and skb_peek_tail()
	 * for the opposite READ_ONCE()
	 */
	WRITE_ONCE(newsk->next, next);
	WRITE_ONCE(newsk->prev, prev);
	WRITE_ONCE(((struct sk_buff_list *)next)->prev, newsk);
	WRITE_ONCE(((struct sk_buff_list *)prev)->next, newsk);
	WRITE_ONCE(list->qlen, list->qlen + 1);
}

static inline void __skb_queue_before(struct sk_buff_head *list,
				      struct sk_buff *next,
				      struct sk_buff *newsk){
	__skb_insert(newsk, ((struct sk_buff_list *)next)->prev, next, list);
}

static inline void __skb_queue_tail(struct sk_buff_head *list, struct sk_buff *newsk){
	__skb_queue_before(list, (struct sk_buff *)list, newsk);
}

static void skb_queue_tail(struct sk_buff_head *list, struct sk_buff *newsk){
	__skb_queue_tail(list, newsk);
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
		skb->dev ? skb->dev->name : "<NULL>"
	);
}


static void skb_over_panic(struct sk_buff *skb, unsigned int sz, void *addr){
	skb_panic(skb, sz, addr, __func__);
}

static void skb_under_panic(struct sk_buff *skb, unsigned int sz, void *addr){
	skb_panic(skb, sz, addr, __func__);
}

#endif
