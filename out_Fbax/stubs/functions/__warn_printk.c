int __warn_printk(const char *fmt, ...){
	int ret = 0;
	va_list ap;
	va_start(ap, fmt);
	ret = vprintf(fmt, ap);
	va_end(ap);
	return ret;
}
