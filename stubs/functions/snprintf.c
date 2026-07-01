int snprintf(char *buf, size_t size, const char *fmt, ...){
	int i = 0;
	va_list args;

	va_start(args, fmt);
	i = vsnprintf(buf, size, fmt, args);
	va_end(args);

	return i;
}
