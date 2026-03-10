int _printk(const char *fmt, ...){
	int ret = 0;
	va_list ap;

	if(fmt[0] == '0' || fmt[0] == '1' || fmt[0] == '2' || fmt[0] == '3' || fmt[0] == '4' || fmt[0] == '5' || fmt[0] == '6' || fmt[0] == '7' || fmt[1] == '0' ||
	   fmt[1] == '1' || fmt[1] == '2' || fmt[1] == '3' || fmt[1] == '4' || fmt[1] == '5' || fmt[1] == '6' || fmt[1] == '7'){
		fmt += 2;
	}

	va_start(ap, fmt);
	ret = vprintf(fmt, ap);
	va_end(ap);
	return ret;
}

