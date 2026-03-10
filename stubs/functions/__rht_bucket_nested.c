struct rhash_lock_head __rcu **__rht_bucket_nested(struct bucket_table *tbl, unsigned int hash){
	const unsigned int shift = PAGE_SHIFT - ilog2(sizeof(void *));
	unsigned int index = hash & ((1 << tbl->nest) - 1);
	unsigned int size = tbl->size >> tbl->nest;
	unsigned int subhash = hash;
	union nested_table *ntbl = NULL;

	ntbl = nested_table_top(tbl);
	ntbl = rht_dereference_bucket_rcu(ntbl[index].table, tbl, hash);
	subhash >>= tbl->nest;

	while (ntbl && size > (1 << shift)) {
		index = subhash & ((1 << shift) - 1);
		ntbl = rht_dereference_bucket_rcu(ntbl[index].table, tbl, hash);
		size >>= shift;
		subhash >>= shift;
	}

	if (ntbl == NULL){
		return NULL;
	}

	return &ntbl[subhash].bucket;

}
