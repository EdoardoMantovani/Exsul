ktime_t ktime_get_with_offset(enum tk_offsets offs){
	return (ktime_t)nsec_now_monotonic();
}
