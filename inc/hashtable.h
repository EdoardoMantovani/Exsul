/* contains everything related to the hashtable manipulation */

#define HASH_DEFAULT_SIZE	64UL
#define HASH_MIN_SIZE		4U

#define rhltable_init		rhltable_init_noprof

/**
 * rht_for_each_rcu_from - iterate over rcu hash chain from given head
 * @pos:	the &struct rhash_head to use as a loop cursor.
 * @head:	the &struct rhash_head to start from
 * @tbl:	the &struct bucket_table
 * @hash:	the hash value / bucket index
 *
 * This hash chain list-traversal primitive may safely run concurrently with
 * the _rcu mutation primitives such as rhashtable_insert() as long as the
 * traversal is guarded by rcu_read_lock().
 */
#define rht_for_each_rcu_from(pos, head, tbl, hash)			\
	for (({barrier(); }),						\
	     pos = head;						\
	     !rht_is_a_nulls(pos);					\
	     pos = rcu_dereference_all(pos->next))

/**
 * rht_for_each_rcu - iterate over rcu hash chain
 * @pos:	the &struct rhash_head to use as a loop cursor.
 * @tbl:	the &struct bucket_table
 * @hash:	the hash value / bucket index
 *
 * This hash chain list-traversal primitive may safely run concurrently with
 * the _rcu mutation primitives such as rhashtable_insert() as long as the
 * traversal is guarded by rcu_read_lock().
 */
#define rht_for_each_rcu(pos, tbl, hash)			\
	for (({barrier(); }),					\
	     pos = rht_ptr_rcu(rht_bucket(tbl, hash));		\
	     !rht_is_a_nulls(pos);				\
	     pos = rcu_dereference_all(pos->next))

/**
 * rht_for_each_entry_rcu_from - iterated over rcu hash chain from given head
 * @tpos:	the type * to use as a loop cursor.
 * @pos:	the &struct rhash_head to use as a loop cursor.
 * @head:	the &struct rhash_head to start from
 * @tbl:	the &struct bucket_table
 * @hash:	the hash value / bucket index
 * @member:	name of the &struct rhash_head within the hashable struct.
 *
 * This hash chain list-traversal primitive may safely run concurrently with
 * the _rcu mutation primitives such as rhashtable_insert() as long as the
 * traversal is guarded by rcu_read_lock().
 */
#define rht_for_each_entry_rcu_from(tpos, pos, head, tbl, hash, member) \
	for (({barrier(); }),						    \
	     pos = head;						    \
	     (!rht_is_a_nulls(pos)) && rht_entry(tpos, pos, member);	    \
	     pos = rht_dereference_bucket_rcu(pos->next, tbl, hash))

/**
 * rht_for_each_entry_rcu - iterate over rcu hash chain of given type
 * @tpos:	the type * to use as a loop cursor.
 * @pos:	the &struct rhash_head to use as a loop cursor.
 * @tbl:	the &struct bucket_table
 * @hash:	the hash value / bucket index
 * @member:	name of the &struct rhash_head within the hashable struct.
 *
 * This hash chain list-traversal primitive may safely run concurrently with
 * the _rcu mutation primitives such as rhashtable_insert() as long as the
 * traversal is guarded by rcu_read_lock().
 */
#define rht_for_each_entry_rcu(tpos, pos, tbl, hash, member)		   \
	rht_for_each_entry_rcu_from(tpos, pos,				   \
				    rht_ptr_rcu(rht_bucket(tbl, hash)),	   \
				    tbl, hash, member)

/**
 * rcu_dereference_all_check() - rcu_dereference_all with debug checking
 * @p: The pointer to read, prior to dereferencing
 * @c: The conditions under which the dereference will take place
 *
 * This is similar to rcu_dereference_check(), but allows protection
 * by all forms of vanilla RCU readers, including preemption disabled,
 * bh-disabled, and interrupt-disabled regions of code.  Note that "vanilla
 * RCU" excludes SRCU and the various Tasks RCU flavors.  Please note
 * that this macro should not be backported to any Linux-kernel version
 * preceding v5.0 due to changes in synchronize_rcu() semantics prior
 * to that version.
 */
#define rcu_dereference_all_check(p, c)	__rcu_dereference_check((p), __UNIQUE_ID(rcu), (c) || 0, __rcu)

enum rht_lookup_freq {
        RHT_LOOKUP_NORMAL,
        RHT_LOOKUP_LIKELY,
};

struct rhash_head {
	struct rhash_head __rcu		*next;
};

struct rhlist_head {
	struct rhash_head		rhead;
	struct rhlist_head __rcu	*next;
};

/*
 * Objects in an rhashtable have an embedded struct rhash_head
 * which is linked into as hash chain from the hash table - or one
 * of two or more hash tables when the rhashtable is being resized.
 * The end of the chain is marked with a special nulls marks which has
 * the least significant bit set but otherwise stores the address of
 * the hash bucket.  This allows us to be sure we've found the end
 * of the right list.
 * The value stored in the hash bucket has BIT(0) used as a lock bit.
 * This bit must be atomically set before any changes are made to
 * the chain.  To avoid dereferencing this pointer without clearing
 * the bit first, we use an opaque 'struct rhash_lock_head *' for the
 * pointer stored in the bucket.  This struct needs to be defined so
 * that rcu_dereference() works on it, but it has no content so a
 * cast is needed for it to be useful.  This ensures it isn't
 * used by mistake with clearing the lock bit first.
 */
struct rhash_lock_head {};

/**
 * struct bucket_table - Table of hash buckets
 * @size: Number of hash buckets
 * @nest: Number of bits of first-level nested table.
 * @rehash: Current bucket being rehashed
 * @hash_rnd: Random seed to fold into hash
 * @walkers: List of active walkers
 * @rcu: RCU structure for freeing the table
 * @future_tbl: Table under construction during rehashing
 * @ntbl: Nested table used when out of memory.
 * @buckets: size * hash buckets
 */
struct bucket_table {
	unsigned int		size;
	unsigned int		nest;
	u32			hash_rnd;
	struct list_head	walkers;
	struct rcu_head		rcu;

	struct bucket_table __rcu *future_tbl;
	struct rhash_lock_head __rcu *buckets[];
};

/**
 * struct rhashtable_compare_arg - Key for the function rhashtable_compare
 * @ht: Hash table
 * @key: Key to compare against
 */
struct rhashtable_compare_arg {
	struct rhashtable *ht;
	const void *key;
};

typedef u32 (*rht_hashfn_t)(void *data, u32 len, u32 seed);
typedef u32 (*rht_obj_hashfn_t)(void *data, u32 len, u32 seed);
typedef int (*rht_obj_cmpfn_t)(struct rhashtable_compare_arg *arg, void *obj);

/**
 * struct rhashtable_params - Hash table construction parameters
 * @nelem_hint: Hint on number of elements, should be 75% of desired size
 * @key_len: Length of key
 * @key_offset: Offset of key in struct to be hashed
 * @head_offset: Offset of rhash_head in struct to be hashed
 * @max_size: Maximum size while expanding
 * @min_size: Minimum size while shrinking
 * @automatic_shrinking: Enable automatic shrinking of tables
 * @hashfn: Hash function (default: jhash2 if !(key_len % 4), or jhash)
 * @obj_hashfn: Function to hash object
 * @obj_cmpfn: Function to compare key with object
 */
struct rhashtable_params {
	u16			nelem_hint;
	u16			key_len;
	u16			key_offset;
	u16			head_offset;
	unsigned int		max_size;
	u16			min_size;
	bool			automatic_shrinking;
	rht_hashfn_t		hashfn;
	rht_obj_hashfn_t	obj_hashfn;
	rht_obj_cmpfn_t		obj_cmpfn;
};

/**
 * struct rhashtable - Hash table handle
 * @tbl: Bucket table
 * @key_len: Key length for hashfn
 * @max_elems: Maximum number of elements in table
 * @p: Configuration parameters
 * @rhlist: True if this is an rhltable
 * @run_work: Deferred worker to expand/shrink asynchronously
 * @mutex: Mutex to protect current/future table swapping
 * @lock: Spin lock to protect walker list
 * @nelems: Number of elements in table
 */
struct rhashtable {
	struct bucket_table __rcu	*tbl;
	unsigned int			key_len;
	unsigned int			max_elems;
	struct rhashtable_params	p;
	bool				rhlist;
	struct work_struct		run_work;
	struct mutex                    mutex;
	u32				lock;
	atomic_int			nelems;
#ifdef CONFIG_MEM_ALLOC_PROFILING
	struct alloc_tag		*alloc_tag;
#endif
};

/**
 * struct rhltable - Hash table with duplicate objects in a list
 * @ht: Underlying rhtable
 */
struct rhltable {
	struct rhashtable ht;
};

union nested_table {
	union nested_table __rcu *table;
	struct rhash_lock_head __rcu *bucket;
};

/*
 * declare the prototype of 'rht_bucket_nested_insert' before everything else
 */
static struct rhash_lock_head __rcu **rht_bucket_nested_insert(struct rhashtable *ht, struct bucket_table *tbl, unsigned int hash);

static inline bool rht_is_a_nulls(struct rhash_head *ptr){
	return ((unsigned long) ptr & 1);
}

static inline void *rht_obj(struct rhashtable *ht, struct rhash_head *he){
	return (char *)he - ht->p.head_offset;
}

static inline unsigned int rht_bucket_index(struct bucket_table *tbl, unsigned int hash){
	return hash & (tbl->size - 1);
}

static inline int rhashtable_compare(struct rhashtable_compare_arg *arg, void *obj){
	struct rhashtable *ht = arg->ht;
	const char *ptr = obj;

	return memcmp(ptr + ht->p.key_offset, arg->key, ht->p.key_len);
}

static inline struct rhash_lock_head __rcu *const *rht_bucket(struct bucket_table *tbl, unsigned int hash){
	return unlikely(tbl->nest) ? rht_bucket_nested(tbl, hash) : &tbl->buckets[hash];
}

static inline struct rhash_lock_head __rcu **rht_bucket_var(struct bucket_table *tbl, unsigned int hash){
	return unlikely(tbl->nest) ? __rht_bucket_nested(tbl, hash) : &tbl->buckets[hash];
}

static inline struct rhash_lock_head __rcu **rht_bucket_insert(struct rhashtable *ht, struct bucket_table *tbl, unsigned int hash){
	return unlikely(tbl->nest) ? rht_bucket_nested_insert(ht, tbl, hash) : &tbl->buckets[hash];
}

static inline union nested_table *nested_table_top(struct bucket_table *tbl){
	/* The top-level bucket entry does not need RCU protection
	 * because it's set at the same time as tbl->nest.
	 */
	return (void *)rcu_dereference_protected(tbl->buckets[0], 1);
}


static inline struct rhash_head *__rht_ptr(struct rhash_lock_head *p, struct rhash_lock_head __rcu *const *bkt, enum rht_lookup_freq freq){
	unsigned long p_val = (unsigned long)p & ~BIT(0);

	if (freq == RHT_LOOKUP_LIKELY){
		return (struct rhash_head *)(likely(p_val) ? p_val : (unsigned long)RHT_NULLS_MARKER(bkt));
	}else{
		return (struct rhash_head *) (p_val ?: (unsigned long)RHT_NULLS_MARKER(bkt));
	}
}

/*
 * Where 'bkt' is a bucket and might be locked:
 *   rht_ptr_rcu() dereferences that pointer and clears the lock bit.
 *   rht_ptr() dereferences in a context where the bucket is locked.
 *   rht_ptr_exclusive() dereferences in a context where exclusive
 *            access is guaranteed, such as when destroying the table.
 */
static inline struct rhash_head *__rht_ptr_rcu(struct rhash_lock_head __rcu *const *bkt, enum rht_lookup_freq freq){
	return __rht_ptr(rcu_dereference_all(*bkt), bkt, freq);
}

static inline unsigned int rht_key_get_hash(struct rhashtable *ht, void *key, struct rhashtable_params params, unsigned int hash_rnd){
	unsigned int hash = 0;
	unsigned int key_len = 0;

	/* params must be equal to ht->p if it isn't constant. */
	if (!__builtin_constant_p(params.key_len)){
		hash = ht->p.hashfn(key, ht->key_len, hash_rnd);
	}else if (params.key_len) {
		unsigned int key_len = params.key_len;

		if (params.hashfn){
			hash = params.hashfn(key, key_len, hash_rnd);
		}else if (key_len & (sizeof(u32) - 1)){
			hash = jhash(key, key_len, hash_rnd);
		}else{
			hash = jhash2(key, key_len / sizeof(u32), hash_rnd);
		}
	}else{
		key_len = ht->p.key_len;

		if (params.hashfn){
			hash = params.hashfn(key, key_len, hash_rnd);
		}else{
			hash = jhash(key, key_len, hash_rnd);
		}
	}

	return hash;
}

static inline unsigned int rht_key_hashfn(struct rhashtable *ht, struct bucket_table *tbl, void *key, struct rhashtable_params params){
	unsigned int hash = rht_key_get_hash(ht, key, params, tbl->hash_rnd);

	return rht_bucket_index(tbl, hash);
}

/* Internal function, do not use. */
static inline struct rhash_head *__rhashtable_lookup(struct rhashtable *ht, const void *key, struct rhashtable_params params, enum rht_lookup_freq freq){
	unsigned int hash = 0;
	struct rhashtable_compare_arg arg = {
		.ht = ht,
		.key = key,
	};
	struct rhash_lock_head __rcu *const *bkt = NULL;
	struct bucket_table *tbl = NULL;
	struct rhash_head *he = NULL;

	tbl = rht_dereference_rcu(ht->tbl, ht);
restart:
	hash = rht_key_hashfn(ht, tbl, key, params);
	bkt = rht_bucket(tbl, hash);
	do {
		rht_for_each_rcu_from(he, __rht_ptr_rcu(bkt, freq), tbl, hash) {
			if (params.obj_cmpfn ?
			    params.obj_cmpfn(&arg, rht_obj(ht, he)) :
			    rhashtable_compare(&arg, rht_obj(ht, he)))
				continue;
			return he;
		}
		/* An object might have been moved to a different hash chain,
		 * while we walk along it - better check and retry.
		 */
	} while (he != RHT_NULLS_MARKER(bkt));

	tbl = rht_dereference_rcu(tbl->future_tbl, ht);
	if (unlikely(tbl)){
		goto restart;
	}

	return NULL;
}

/**
 * rhashtable_lookup - search hash table
 * @ht:		hash table
 * @key:	the pointer to the key
 * @params:	hash table parameters
 *
 * Computes the hash value for the key and traverses the bucket chain looking
 * for an entry with an identical key. The first matching entry is returned.
 *
 * This must only be called under the RCU read lock.
 *
 * Returns the first entry on which the compare function returned true.
 */
static inline void *rhashtable_lookup(struct rhashtable *ht, const void *key, const struct rhashtable_params params){
	struct rhash_head *he = __rhashtable_lookup(ht, key, params, RHT_LOOKUP_NORMAL);

	return he ? rht_obj(ht, he) : NULL;
}

static inline void *rhashtable_lookup_likely(struct rhashtable *ht, const void *key, const struct rhashtable_params params){
	struct rhash_head *he = __rhashtable_lookup(ht, key, params, RHT_LOOKUP_LIKELY);

	return likely(he) ? rht_obj(ht, he) : NULL;
}

/**
 * rhltable_lookup - search hash list table
 * @hlt:	hash table
 * @key:	the pointer to the key
 * @params:	hash table parameters
 *
 * Computes the hash value for the key and traverses the bucket chain looking
 * for an entry with an identical key.  All matching entries are returned
 * in a list.
 *
 * This must only be called under the RCU read lock.
 *
 * Returns the list of entries that match the given key.
 */
static inline struct rhlist_head *rhltable_lookup(struct rhltable *hlt, void *key, struct rhashtable_params params){
	struct rhash_head *he = __rhashtable_lookup(&hlt->ht, key, params, RHT_LOOKUP_NORMAL);
	return he ? container_of(he, struct rhlist_head, rhead) : NULL;
}

static inline struct rhlist_head *rhltable_lookup_likely(struct rhltable *hlt, void *key, struct rhashtable_params params){
	struct rhash_head *he = __rhashtable_lookup(&hlt->ht, key, params, RHT_LOOKUP_LIKELY);
	return likely(he) ? container_of(he, struct rhlist_head, rhead) : NULL;
}

/**
 * rhashtable_lookup_fast - search hash table, without RCU read lock
 * @ht:		hash table
 * @key:	the pointer to the key
 * @params:	hash table parameters
 *
 * Computes the hash value for the key and traverses the bucket chain looking
 * for an entry with an identical key. The first matching entry is returned.
 *
 * Only use this function when you have other mechanisms guaranteeing
 * that the object won't go away after the RCU read lock is released.
 *
 * Returns the first entry on which the compare function returned true.
 */
static inline void *rhashtable_lookup_fast(struct rhashtable *ht, const void *key, const struct rhashtable_params params){
	void *obj = NULL;

	pthread_mutex_lock(&director.rhashtable_lock);
	obj = rhashtable_lookup(ht, key, params);
	pthread_mutex_unlock(&director.rhashtable_lock);

	return obj;
}

static inline struct rhash_head *rht_ptr_exclusive(struct rhash_lock_head __rcu *const *bkt){
	return __rht_ptr(rcu_dereference_protected(*bkt, 1), bkt, RHT_LOOKUP_NORMAL);
}

static inline int rhltable_insert_simple(struct rhltable *hlt, struct rhlist_head *node, const struct rhashtable_params params){
	int ret = 0;
	struct rhashtable *ht = NULL;
	struct bucket_table *tbl = NULL;
	struct rhash_lock_head **bucket = NULL;
	struct rhash_head *old = NULL;
	void *object = NULL;
	void *key = NULL;
	unsigned int hash = 0;

	if (hlt == NULL || node == NULL){
		ret = -EINVAL;
		return ret;
	}

	ht = &hlt->ht;
	tbl = rcu_dereference_protected(ht->tbl, 1);
	if (tbl == NULL){
		ret = -EINVAL;
		return ret;
	}

	object = (char *)node - params.head_offset;
	key = (char *)object + params.key_offset;
	hash = rht_key_hashfn(ht, tbl, key, params);
	bucket = rht_bucket_insert(ht, tbl, hash);
	if (bucket == NULL){
		ret = -ENOMEM;
		return ret;
	}

	pthread_mutex_lock(&director.rhashtable_lock);
	old = rht_ptr_exclusive(bucket);
	node->next = NULL;
	node->rhead.next = old;
	rcu_assign_pointer(*bucket, (struct rhash_lock_head *)&node->rhead);
	atomic_fetch_add_explicit(&ht->nelems, 1, memory_order_relaxed);
	pthread_mutex_unlock(&director.rhashtable_lock);

	return ret;
}


static void rhashtable_free_one(struct rhashtable *ht, struct rhash_head *obj, void (*free_fn)(void *ptr, void *arg), void *arg){
	struct rhlist_head *list = NULL;

	if (!ht->rhlist) {
		free_fn(rht_obj(ht, obj), arg);
		return;
	}

	list = container_of(obj, struct rhlist_head, rhead);
	do {
		obj = &list->rhead;
		list = rht_dereference(list->next, ht);
		free_fn(rht_obj(ht, obj), arg);
	} while (list);
}

static void nested_table_free(union nested_table *ntbl, unsigned int size){
	const unsigned int shift = PAGE_SHIFT - ilog2(sizeof(void *));
	const unsigned int len = 1 << shift;

	ntbl = rcu_dereference_protected(ntbl->table, 1);
	if (ntbl == NULL){
		return;
	}
	if (size > len) {
		size >>= shift;
		for (unsigned int i = 0; i < len; i++){
			nested_table_free(ntbl + i, size);
		}
	}

	kfree(ntbl);
}

static void nested_bucket_table_free(const struct bucket_table *tbl){
	unsigned int size = tbl->size >> tbl->nest;
	unsigned int len = 1 << tbl->nest;
	union nested_table *ntbl = NULL;

	ntbl = nested_table_top(tbl);

	for (int i = 0; i < len; i++){
		nested_table_free(ntbl + i, size);
	}
	kfree(ntbl);
}

static void bucket_table_free(const struct bucket_table *tbl){
	if (tbl->nest){
		nested_bucket_table_free(tbl);
	}

	kvfree(tbl);
}

/**
 * rhltable_free_and_destroy - free elements and destroy hash list table
 * @hlt:	the hash list table to destroy
 * @free_fn:	callback to release resources of element
 * @arg:	pointer passed to free_fn
 *
 * See documentation for rhashtable_free_and_destroy.
 */
static inline void rhltable_free_and_destroy(struct rhltable *hlt, void (*free_fn)(void *ptr, void *arg), void *arg){
	rhashtable_free_and_destroy(&hlt->ht, free_fn, arg);
}

static inline void rhltable_destroy(struct rhltable *hlt){
	rhltable_free_and_destroy(hlt, NULL, NULL);
}

static u32 rhashtable_jhash2(const void *key, u32 length, u32 seed){
	return jhash2(key, length, seed);
}

static size_t rounded_hashtable_size(const struct rhashtable_params *params){
	size_t retsize = 0;

	if (params->nelem_hint){
		retsize = max(roundup_pow_of_two(params->nelem_hint * 4 / 3),
			      (unsigned long)params->min_size);
	}else{
		retsize = max(HASH_DEFAULT_SIZE,
			      (unsigned long)params->min_size);
	}
	return retsize;
}

static struct bucket_table *bucket_table_alloc(struct rhashtable *ht, int nbuckets, int gfp){
	int size = 0;
	struct bucket_table *tbl = NULL;

	tbl = calloc(1, sizeof(struct bucket_table) + (sizeof(struct rhash_lock_head *) * nbuckets));

	size = nbuckets;

	if (tbl == NULL){
		return NULL;
	}

	tbl->size = size;

	INIT_LIST_HEAD(&tbl->walkers);

	tbl->hash_rnd = get_random_u32();

	for (int i = 0; i < nbuckets; i++){
		INIT_RHT_NULLS_HEAD(tbl->buckets[i]);
	}

	return tbl;
}

static union nested_table *nested_table_alloc(struct rhashtable *ht, union nested_table __rcu **prev, bool leaf){
	union nested_table *ntbl = NULL;

	ntbl = rcu_dereference(*prev);
	if (ntbl != NULL){
		return ntbl;
	}
	ntbl = malloc(PAGE_SIZE);

	if (ntbl && leaf) {
		for (int i = 0; i < PAGE_SIZE / sizeof(ntbl[0]); i++){
			INIT_RHT_NULLS_HEAD(ntbl[i].bucket);
		}
	}

	/* for simplicity, swap 'cmpxchg' with '__sync_val_compare_and_swap' which seems to be a gcc builtin */
	if (__sync_val_compare_and_swap((union nested_table **)prev, NULL, ntbl)== NULL){
		return ntbl;
	}
	/* Raced with another thread. */
	free(ntbl);
	return rcu_dereference(*prev);
}

static struct rhash_lock_head __rcu **rht_bucket_nested_insert(struct rhashtable *ht, struct bucket_table *tbl, unsigned int hash){
	const unsigned int shift = PAGE_SHIFT - ilog2(sizeof(void *));
	unsigned int index = hash & ((1 << tbl->nest) - 1);
	unsigned int size = tbl->size >> tbl->nest;
	union nested_table *ntbl = NULL;

	ntbl = nested_table_top(tbl);
	hash >>= tbl->nest;
	ntbl = nested_table_alloc(ht, &ntbl[index].table, size <= (1 << shift));

	while (ntbl && size > (1 << shift)) {
		index = hash & ((1 << shift) - 1);
		size >>= shift;
		hash >>= shift;
		ntbl = nested_table_alloc(ht, &ntbl[index].table, size <= (1 << shift));
	}

	if (ntbl == NULL){
		return NULL;
	}
	return &ntbl[hash].bucket;
}
