int _printk(const char *fmt, ...){
	int ret = 0;
	va_list ap;

	fmt += 2;
	va_start(ap, fmt);
	ret = vprintf(fmt, ap);
	va_end(ap);
	return ret;
}

