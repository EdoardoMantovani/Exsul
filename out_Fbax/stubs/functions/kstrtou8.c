int kstrtou8(const char *s, unsigned int base, u8 *res){
	*res = strtoul_with_range(s, NULL, base, 256);
	return 0;
}
