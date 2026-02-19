void _dev_warn(const struct device *dev, const char *fmt, ...){
	fmt += 2;
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}
