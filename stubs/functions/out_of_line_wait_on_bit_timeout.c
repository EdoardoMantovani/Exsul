int out_of_line_wait_on_bit_timeout(unsigned long *word, int bit, wait_bit_action_f *action, unsigned int mode, unsigned long timeout){
	int ret = 0;
	u64 nsec = 0;
	struct timespec deadline = {};

	if(word == NULL || bit < 0){
		ret = -EINVAL;
		return ret;
	}
	clock_gettime(CLOCK_REALTIME, &deadline);
	nsec = deadline.tv_nsec + (u64)jiffies_to_msecs(timeout) * 1000000ULL;
	deadline.tv_sec += nsec / NSEC_PER_SEC;
	deadline.tv_nsec = nsec % NSEC_PER_SEC;
	pthread_mutex_lock(&director.bit_wait_lock);
	while(test_bit(bit, word)){
		if(timeout == 0){
			ret = -EAGAIN;
			break;
		}
		ret = pthread_cond_timedwait(&director.bit_wait_cond, &director.bit_wait_lock, &deadline);
		if(ret == ETIMEDOUT){
			ret = -EAGAIN;
			break;
		}
	}
	pthread_mutex_unlock(&director.bit_wait_lock);
	return ret;
}
