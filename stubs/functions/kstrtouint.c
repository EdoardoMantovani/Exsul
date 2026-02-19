int kstrtouint(const char *s, unsigned int base, unsigned int *res){
	*res = strtol(s, NULL, base);
	return 0;
}
