struct task_struct *kthread_create_on_node(int (*kthread_function)(void *data), void *data, int node, unsigned char *thread_name){
	int ret = 0;
	pthread_condattr_t current_cond_attr = {};
	struct __task_struct *current_thread = NULL;

	current_thread = (struct __task_struct *)calloc(1, sizeof(struct __task_struct));

	if (current_thread == NULL){
		return ERR_PTR(-ENOMEM);
	}
	pthread_mutex_init(&current_thread->lock, NULL);
	pthread_condattr_init(&current_cond_attr);
	pthread_condattr_setclock(&current_cond_attr, CLOCK_MONOTONIC);
	pthread_cond_init(&current_thread->cond, &current_cond_attr);
	pthread_condattr_destroy(&current_cond_attr);

	atomic_store(&current_thread->__state, TASK_RUNNING);
	atomic_store(&current_thread->started, false);
	atomic_store(&current_thread->should_stop, false);

	current_thread->threadfn = kthread_function;
	current_thread->data = data;

	if(thread_name != NULL){
		snprintf(current_thread->name, sizeof(current_thread->name), "%s", thread_name);
	}

	ret = pthread_create(&current_thread->tid, NULL, kthread_trampoline, current_thread);
	if(ret != 0){
		pthread_cond_destroy(&current_thread->cond);
		pthread_mutex_destroy(&current_thread->lock);
		memset(current_thread, 0, sizeof(struct __task_struct));
		free(current_thread);
        	return ERR_PTR(-EINVAL);
	}
	return (struct task_struct *)current_thread;
}
