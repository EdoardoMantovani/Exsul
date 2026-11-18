unsigned int jiffies_to_usecs(const unsigned long j){
	int ret = 0;

	#if !(USEC_PER_SEC % HZ)
	ret = (USEC_PER_SEC / HZ) * j;
	return ret;
	#else
	#if BITS_PER_LONG == 32
	ret = (HZ_TO_USEC_MUL32 * j) >> HZ_TO_USEC_SHR32;
	return ret;
	#else
	ret = (j * HZ_TO_USEC_NUM) / HZ_TO_USEC_DEN;
	return ret;
	#endif
	#endif
}
