void tasklet_setup(struct tasklet_struct *tasklet, void (*callback)(struct tasklet_struct *)){
	struct tasklet_runtime_state *state = NULL;

	if(callback == NULL || tasklet == NULL){
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
	tasklet->callback = callback;
	tasklet->use_callback = true;
	tasklet->data = 0;
	end:
}
