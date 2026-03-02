void __dynamic_pr_debug(void *descriptor, const char *fmt, ...){
	va_list ap;

	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}
