int snprintf(char *buf, size_t size, const char *fmt, ...){
	int ret = 0;
	va_list args;

	va_start(args, fmt);
	ret = vsnprintf(buf, size, fmt, args);
	va_end(args);

	return ret;
}
