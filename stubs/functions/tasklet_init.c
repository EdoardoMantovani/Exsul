void tasklet_init(struct tasklet_struct *tasklet, void (*func)(unsigned long), unsigned long data){
	struct tasklet_runtime_state *state = NULL;

	if(func == NULL || tasklet == NULL){
		goto end;
	}
	tasklet->next = NULL;
	tasklet->state = 0;
	tasklet_count_set(tasklet, 0);

	state = tasklet_runtime_get(tasklet, true);
	if (state != NULL) {
		atomic_store_explicit(&state->queued, false, memory_order_release);
		atomic_store_explicit(&state->dispatch_refs, 0, memory_order_release);
		atomic_store_explicit(&state->priority, TASKLET_PRIO_NORMAL, memory_order_release);
	}

	tasklet->func = func;
	tasklet->use_callback = false;
	tasklet->data = data;
	end:
}
