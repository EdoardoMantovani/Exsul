void rhashtable_free_and_destroy(struct rhashtable *ht, void (*free_fn)(void *ptr, void *arg), void *arg){
	struct bucket_table *tbl = NULL;
	struct bucket_table *next_tbl = NULL;

	cancel_work_sync(&ht->run_work);
	mutex_lock(&ht->mutex);
	tbl = rht_dereference(ht->tbl, ht);

	restart:
	if (free_fn) {
		for (int i = 0; i < tbl->size; i++) {
			struct rhash_head *pos, *next;

			for (pos = rht_ptr_exclusive(rht_bucket(tbl, i)),
			     next = !rht_is_a_nulls(pos) ? rht_dereference(pos->next, ht) : NULL;
			     !rht_is_a_nulls(pos);
			     pos = next,
			     next = !rht_is_a_nulls(pos) ? rht_dereference(pos->next, ht) : NULL)
				rhashtable_free_one(ht, pos, free_fn, arg);
		}
	}

	next_tbl = rht_dereference(tbl->future_tbl, ht);
	bucket_table_free(tbl);
	if (next_tbl) {
		tbl = next_tbl;
		goto restart;
	}
	mutex_unlock(&ht->mutex);
}
