void netdev_err(const struct net_device *dev, const char *format, ...){
	int ret = 0;

	va_list ap;
	va_start(ap, format);
	ret = vprintf(format, ap);
	va_end(ap);
	return ret;
}
