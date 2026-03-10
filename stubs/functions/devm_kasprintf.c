char *devm_kasprintf(struct device *dev, int gfp, const char *fmt, ...){
	va_list ap;
	char *p;

	va_start(ap, fmt);
	p = devm_kvasprintf(dev, gfp, fmt, ap);
	va_end(ap);

	return p;
}
