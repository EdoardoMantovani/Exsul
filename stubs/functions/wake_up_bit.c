void wake_up_bit(unsigned long *word, int bit){
	int ret = 0;

	if(word == NULL || bit < 0 || director.hci_state_initialized == false){
		return;
	}
	pthread_mutex_lock(&director.bit_wait_lock);
	pthread_cond_broadcast(&director.bit_wait_cond);
	pthread_mutex_unlock(&director.bit_wait_lock);
}
