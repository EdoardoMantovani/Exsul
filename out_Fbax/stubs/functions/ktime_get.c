int64_t ktime_get(void){
	return (int64_t)nsec_now_monotonic();
}
