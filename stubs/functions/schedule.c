void schedule(void){
	struct task_struct *current_task = NULL;
	current_task = current;

	if(current_task == NULL){
		goto out;
	}

	pthread_mutex_lock(&current_task->lock);

	while (atomic_load(&current_task->__state) != TASK_RUNNING && !atomic_load(&current_task->should_stop)) {
		pthread_cond_wait(&current_task->cond, &current_task->lock);
	}

	pthread_mutex_unlock(&current_task->lock);
	out:
}
