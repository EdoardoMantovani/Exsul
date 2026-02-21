int kthread_stop(struct task_struct *kthread){
	int ret = 0;

	if (kthread == NULL){
		ret = -EINVAL;
		return ret;
	}

	atomic_store(&kthread->should_stop, true);
	wake_up_process(kthread);

	pthread_join(kthread->tid, NULL);
	ret = kthread->exit_code;
	return ret;
}
