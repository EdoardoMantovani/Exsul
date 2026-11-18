int wake_up_process(struct __task_struct *task_struct){
	int ret = 0;
	bool process_woke_up = false;

	if (task_struct == NULL){
		ret = -EINVAL;
		return ret;
	}
	pthread_mutex_lock(&task_struct->lock);
	if (atomic_load(&task_struct->started) == false) {
		atomic_store(&task_struct->started, true);
		process_woke_up = true;
	}
	if (atomic_load(&task_struct->__state) != TASK_RUNNING) {
		atomic_store(&task_struct->__state, TASK_RUNNING);
		process_woke_up = true;
	}
	pthread_cond_broadcast(&task_struct->cond);
	pthread_mutex_unlock(&task_struct->lock);
	return process_woke_up;
}
