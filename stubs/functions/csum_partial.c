__wsum csum_partial(const void *buff, int len, __wsum wsum){
	unsigned int sum = (__force unsigned int)wsum;
	unsigned int result = do_csum(buff, len);

	/* add in old sum, and carry.. */
	result += sum;
	if (sum > result){
		result += 1;
	}
	return (__force __wsum)result;
}
