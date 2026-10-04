int sscanf(const char *buf, const char *fmt, ...){
	int ret = 0;
	va_list args;

	va_start(args, fmt);
	ret = vsscanf(buf, fmt, args);
	va_end(args);
	return ret;
}
