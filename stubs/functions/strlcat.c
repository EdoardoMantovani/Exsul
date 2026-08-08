size_t strlcat(char *dest, const char *src, size_t count){
	int res = 0;
	int len = 0;
	int dsize = 0;

	len = strlen(src);
	dsize = strlen(dest);
	res = dsize + len;
	dest += dsize;
	count -= dsize;
	if (len >= count){
		len = count-1;
	}
	memcpy(dest, src, len);
	dest[len] = 0;
	return res;
}
