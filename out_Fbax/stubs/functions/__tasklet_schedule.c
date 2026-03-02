void __tasklet_schedule(struct tasklet_struct *t){
	struct tasklet_head *head = NULL;

	head = &tasklet_global_queue;
	t->next = NULL;
	*head->tail = t;
	head->tail = &(t->next);

}

