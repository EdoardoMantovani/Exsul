int kthread_stop(struct __task_struct *kthread){
	int ret = 0;

	if (kthread == NULL){
		ret = -EINVAL;
		return ret;
	}
	atomic_store(&kthread->should_stop, true);
	wake_up_process(kthread);
	ret = pthread_join(kthread->tid, NULL);
	if(ret != 0){
		return -ret;
	}
	ret = kthread->exit_code;
	pthread_cond_destroy(&kthread->cond);
	pthread_mutex_destroy(&kthread->lock);
	free(kthread);
	return ret;
}
