void tasklet_init(struct tasklet_struct *t, void (*func)(unsigned long), unsigned long data){
	t->next = NULL;
	t->state = 0;
	#ifdef __clang__
	__c11_atomic_store(&t->count, false, __ATOMIC_RELEASE);
	#else
	__atomic_store_n(&t->count, false, __ATOMIC_RELEASE);
	#endif

	t->func = func;
	t->use_callback = false;
	t->data = data;
}
