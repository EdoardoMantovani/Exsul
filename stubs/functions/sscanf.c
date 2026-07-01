int sscanf(const char *buf, const char *fmt, ...){
	int i = 0;
	va_list args;

	va_start(args, fmt);
	i = vsscanf(buf, fmt, args);
	va_end(args);

	return i;
}
