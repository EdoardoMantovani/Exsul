bool kthread_should_stop(void){
	bool ret = false;
	struct task_struct *t = NULL;

	t = current;
	if (t == NULL){
		return ret;
	}
	ret = atomic_load(&t->should_stop);
	return ret;
}
