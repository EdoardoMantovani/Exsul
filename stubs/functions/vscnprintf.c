int vscnprintf(char *buf, size_t size, const char *fmt, va_list args){
	int ret = 0;
	int i = 0;

	if (unlikely(!size)){
		return ret;
	}
	i = vsnprintf(buf, size, fmt, args);
	if (likely(i < size)){
		return i;
	}
	return size - 1;
}
