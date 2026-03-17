int rhashtable_init_noprof(struct rhashtable *ht, const struct rhashtable_params *params){
	int ret = 0;
	int size = 0;
	struct bucket_table *tbl = NULL;

	if ((!params->key_len && !params->obj_hashfn) || (params->obj_hashfn && !params->obj_cmpfn)){
		ret = -EINVAL;
		return ret;
	}

	memset(ht, 0, sizeof(*ht));
	memcpy(&ht->p, params, sizeof(*params));

	if (params->min_size){
		ht->p.min_size = roundup_pow_of_two(params->min_size);
	}

	/* Cap total entries at 2^31 to avoid nelems overflow. */
	ht->max_elems = 1u << 31;

	if (params->max_size) {
		ht->p.max_size = rounddown_pow_of_two(params->max_size);
		if (ht->p.max_size < ht->max_elems / 2){
			ht->max_elems = ht->p.max_size * 2;
		}
	}

	ht->p.min_size = max_t(u16, ht->p.min_size, HASH_MIN_SIZE);

	size = rounded_hashtable_size(&ht->p);

	ht->key_len = ht->p.key_len;
	if (!params->hashfn) {
		ht->p.hashfn = jhash;

		if (!(ht->key_len & (sizeof(u32) - 1))) {
			ht->key_len /= sizeof(u32);
			ht->p.hashfn = rhashtable_jhash2;
		}
	}

	/*
	 * This is api initialization and thus we need to guarantee the
	 * initial rhashtable allocation. Upon failure, retry with the
	 * smallest possible size with __GFP_NOFAIL semantics.
	 */
	tbl = bucket_table_alloc(ht, size, GFP_KERNEL);
	if (unlikely(tbl == NULL)) {
		size = max_t(u16, ht->p.min_size, HASH_MIN_SIZE);
		tbl = bucket_table_alloc(ht, size, GFP_KERNEL);
	}

	ht->nelems = 0;

	RCU_INIT_POINTER(ht->tbl, tbl);

	return 0;
}
