void kthread_complete_and_exit(void *completion, long code){
	if(tls_current != NULL){
		tls_current->exit_code = code;
	}
	pthread_exit((void *)(intptr_t)code);
}
