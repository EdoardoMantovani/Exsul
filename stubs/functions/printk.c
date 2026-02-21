int printk(const char *fmt, ...){
	int ret = 0;
	if(fmt[0] == "\001"){
		fmt += 2;
	}

	va_list ap;
	va_start(ap, fmt);
	ret = vprintf(fmt, ap);
	va_end(ap);
	return ret;
}
