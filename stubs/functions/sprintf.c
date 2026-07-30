int sprintf(char *buf, const char *fmt, ...){
	int i = 0;
	va_list args;

	va_start(args, fmt);
	i = vsnprintf(buf, (size_t)-1, fmt, args);
	va_end(args);

	return i;
}
