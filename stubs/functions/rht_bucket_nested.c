struct rhash_lock_head __rcu **rht_bucket_nested(struct bucket_table *tbl, unsigned int hash){
	static struct rhash_lock_head __rcu *rhnull = NULL;

	if (rhnull == NULL){
		INIT_RHT_NULLS_HEAD(rhnull);
	}
	return __rht_bucket_nested(tbl, hash) ?: &rhnull;
}
