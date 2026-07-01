int strncmp(const char *cs, const char *ct, size_t count){
	int ret = 0;
	unsigned char c1 = 0;
	unsigned char c2 = 0;

	while (count) {
		c1 = *cs++;
		c2 = *ct++;
		if (c1 != c2){
			return c1 < c2 ? -1 : 1;
		}
		if (!c1){
			break;
		}
		count--;
	}
	return ret;
}
