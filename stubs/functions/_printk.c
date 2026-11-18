int _printk(const char *fmt, ...){
	int ret = 0;
	va_list ap;

	if(fmt != NULL && fmt[0] == '\001' && fmt[1] >= '0' && fmt[1] <= '7'){
		fmt += 2;
	}
	va_start(ap, fmt);
	ret = vprintf(fmt, ap);
	va_end(ap);
	return ret;
}

