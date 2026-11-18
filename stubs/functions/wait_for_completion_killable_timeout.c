long wait_for_completion_killable_timeout(struct completion *x, unsigned long timeout){
	int ret = 0;
	struct __task_struct *task = NULL;

	if(x == NULL){
		ret = -EINVAL;
		return ret;
	}
	task = current;
	if(task != NULL && atomic_load(&task->should_stop)){
		ret = -ERESTARTSYS;
		return ret;
	}
	ret = wait_for_completion_timeout(x, timeout);
	return ret;
}
