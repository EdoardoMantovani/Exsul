int kstrtoint(const char *s, unsigned int base, int *res){
	int ret = 0;

	*res = strtol(s, NULL, base);
	return ret;
}
