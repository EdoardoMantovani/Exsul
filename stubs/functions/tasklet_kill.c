void tasklet_kill(struct tasklet_struct *tasklet){
	struct tasklet_runtime_state *state = NULL;

	if (tasklet == NULL){
		goto end;
	}

	state = tasklet_runtime_get(tasklet, true);
	tasklet_set_state(tasklet, TASKLET_STATE_KILL);
	tasklet_sync_init();
	pthread_mutex_lock(&director.tasklet_lock);
	tasklet_head_remove(&director.tasklet_high_priority, tasklet);
	tasklet_head_remove(&director.tasklet_normal_priority, tasklet);
	if (state != NULL) {
		atomic_store_explicit(&state->queued, false, memory_order_release);
	}
	tasklet_clear_state(tasklet, TASKLET_STATE_SCHED);
	pthread_mutex_unlock(&director.tasklet_lock);

	if (state != NULL) {
		pthread_mutex_lock(&state->wait_lock);
		while (tasklet_test_state(tasklet, TASKLET_STATE_RUN) || atomic_load_explicit(&state->dispatch_refs, memory_order_acquire) != 0){
			pthread_cond_wait(&state->wait_cond, &state->wait_lock);
		}
		pthread_mutex_unlock(&state->wait_lock);
		tasklet_runtime_remove(tasklet);
	}
	end:
}
