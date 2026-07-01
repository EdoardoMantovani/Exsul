void kthread_complete_and_exit(void *completion, long code){
	pthread_exit((void *)(intptr_t)code);
}
