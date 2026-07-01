int bcmp(const void *a, const void *b, size_t len){
	int ret = 0;

	ret = memcmp(a, b, len);
	return ret;
}
