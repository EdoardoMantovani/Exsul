void tasklet_setup(struct tasklet_struct *t, void (*callback)(struct tasklet_struct *)){
	t->next = NULL;
	t->state = 0;
	#ifdef __clang__
	__c11_atomic_store(&t->count, false, __ATOMIC_RELEASE);
	#else
	__atomic_store_n(&t->count, false, __ATOMIC_RELEASE);
	#endif
	t->callback = callback;
	t->use_callback = true;
	t->data = 0;
}
