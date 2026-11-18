void *memset32(uint32_t *s, uint32_t v, size_t count){
	u32 *xs = NULL;

	xs = s;
	while (count--){
		*xs++ = v;
	}
	return s;
}
