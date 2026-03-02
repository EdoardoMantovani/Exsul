void tasklet_setup(struct tasklet_struct *t, void (*callback)(struct tasklet_struct *)){
	t->next = NULL;
	t->state = 0;
	__atomic_store_n(&t->count, false, __ATOMIC_RELEASE);
	t->callback = callback;
	t->use_callback = true;
	t->data = 0;
}
