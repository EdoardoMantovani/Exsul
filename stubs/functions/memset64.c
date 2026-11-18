void *memset64(uint64_t *s, uint64_t v, size_t count){
	u64 *xs = NULL;

	xs = s;
	while (count--){
		*xs++ = v;
	}
	return s;
}
