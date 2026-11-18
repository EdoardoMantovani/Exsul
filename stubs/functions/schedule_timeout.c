long schedule_timeout(long timeout){
	int ret = 0;
	int wait_ret = 0;
	u64 deadline_ns = 0;
	u64 now_ns = 0;
	u64 remaining_ns = 0;
	struct timespec deadline = {};
	struct __task_struct *task = NULL;

	if(timeout <= 0){
		return ret;
	}
	task = current;
	if(task == NULL){
		return ret;
	}
	now_ns = nsec_now_monotonic();
	deadline_ns = now_ns + ((u64)timeout * NSEC_PER_SEC + HZ - 1) / HZ;
	deadline.tv_sec = deadline_ns / NSEC_PER_SEC;
	deadline.tv_nsec = deadline_ns % NSEC_PER_SEC;

	pthread_mutex_lock(&task->lock);
	while(atomic_load(&task->__state) != TASK_RUNNING && !atomic_load(&task->should_stop)){
		wait_ret = pthread_cond_timedwait(&task->cond, &task->lock, &deadline);
		if(wait_ret == ETIMEDOUT){
			break;
		}
	}
	pthread_mutex_unlock(&task->lock);
	if(wait_ret == ETIMEDOUT){
		return ret;
	}
	now_ns = nsec_now_monotonic();
	if(now_ns >= deadline_ns){
		return ret;
	}
	remaining_ns = deadline_ns - now_ns;
	ret = (remaining_ns * HZ + NSEC_PER_SEC - 1) / NSEC_PER_SEC;
	if(ret == 0){
		ret = 1;
	}
	return ret;
}
