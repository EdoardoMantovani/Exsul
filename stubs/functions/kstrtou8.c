int kstrtou8(const char *s, unsigned int base, u8 *res){
	int ret = 0;

	*res = strtoul_with_range(s, NULL, base, 256);
	return ret;
}
