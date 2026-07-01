ssize_t sized_strscpy(char *dest, const char *src, size_t count){
	int ret = 0;
	size_t len = 0;

	if(count == 0){
		ret = -E2BIG;
		return ret;
	}
	len = strnlen(src, count);
	if(len == count){
		memcpy(dest, src, count - 1);
		dest[count - 1] = 0;
		ret = -E2BIG;
		return ret;
	}
	memcpy(dest, src, len + 1);
	return (ssize_t)len;
}
