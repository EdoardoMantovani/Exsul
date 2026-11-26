u64 ktime_get_mono_fast_ns(void){
	int ret = 0;
	struct timespec ts = {};

	ret = clock_gettime(CLOCK_MONOTONIC, &ts);
	if(ret != 0){
		return ret;
	}
	ret = (u64)ts.tv_sec * 1000000000ULL + (u64)ts.tv_nsec;
	return ret;
}
