int send_sig(int sig, struct task_struct *task_struct, int priv){
	int ret = 0;
	struct __task_struct *task = NULL;

	if(task_struct == NULL){
		ret = -EINVAL;
		return ret;
	}
	task = (struct __task_struct *)task_struct;
	if(sig == SIGTERM || sig == SIGKILL || sig == SIGINT){
		atomic_store(&task->should_stop, true);
		wake_up_process(task);
	}
	return ret;
}
