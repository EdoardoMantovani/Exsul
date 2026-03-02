int vscnprintf(char *buf, size_t size, const char *fmt, va_list args){
	int i;

	if (unlikely(!size))
		return 0;

	i = vsnprintf(buf, size, fmt, args);

	if (likely(i < size))
		return i;

	return size - 1;
}
