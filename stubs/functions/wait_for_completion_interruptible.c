int wait_for_completion_interruptible(struct completion *x){
	int ret = 0;
	int cur = 0;
	struct __task_struct *task = NULL;
	struct timespec timeout = {
		.tv_sec = 0,
		.tv_nsec = 100000000,
	};

	if(x == NULL){
		ret = -EINVAL;
		return ret;
	}
	task = current;
	for(;;){
		if(try_consume_completion(x)){
			return ret;
		}
		if(task != NULL && atomic_load(&task->should_stop)){
			ret = -ERESTARTSYS;
			return ret;
		}
		cur = atomic_load_explicit(&x->done, memory_order_acquire);
		if(cur == 0){
			futex_wait_timeout(&x->done, 0, &timeout);
		}
	}
}
