int printk(const char *fmt, ...){
	int ret = 0;

        if(fmt[1] == '0' || fmt[1] == '1' || fmt[1] == '2' || fmt[1] == '3' || fmt[1] == '4' || fmt[1] == '5' || fmt[1] == '6' || fmt[1] == '7'){
                fmt += 2;
        }

	va_list ap;
	va_start(ap, fmt);
	ret = vprintf(fmt, ap);
	va_end(ap);
	return ret;
}
