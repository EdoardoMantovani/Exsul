void *memset16(uint16_t *s, uint16_t v, size_t count){
	u16 *xs = s;

	while (count--){
		*xs++ = v;
	}
	return s;
}
