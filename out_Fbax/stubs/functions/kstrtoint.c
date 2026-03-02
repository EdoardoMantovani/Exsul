int kstrtoint(const char *s, unsigned int base, int *res){
	*res = strtol(s, NULL, base);
	return 0;
}
