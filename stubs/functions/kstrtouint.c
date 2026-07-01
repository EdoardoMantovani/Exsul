int kstrtouint(const char *s, unsigned int base, unsigned int *res){
	int ret = 0;

	*res = strtol(s, NULL, base);
	return ret;
}
