void _dev_info(const struct device *dev, const char *fmt, ...){
	va_list ap;
	char buf[2048] = {};
	int len = 0;

	va_start(ap, fmt);
	len = vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	if(len < 0){
		return;
	}

	if(len >= sizeof(buf)){
		len = sizeof(buf) - 1;
	}

	if(len >= 2 && buf[len - 2] == '\\' && buf[len - 1] == 'n'){
		buf[len - 2] = '\n';
		buf[len - 1] = '\0';
	}

	printf("%s", buf);
}
