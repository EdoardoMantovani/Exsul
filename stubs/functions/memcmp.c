int memcmp(const void *cs, const void *ct, size_t count){
	int res = 0;
	const unsigned char *su1 = cs;
	const unsigned char *su2 = ct;

	while (count-- > 0) {
		if ((res = (int)*su1++ - (int)*su2++) != 0)
			break;
	}
	return res;
}
