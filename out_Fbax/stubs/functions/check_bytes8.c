void *check_bytes8(const u8 *start, u8 value, unsigned int bytes){
	while (bytes) {
		if (*start != value)
			return (void *)start;
		start++;
		bytes--;
	}
	return NULL;
}
