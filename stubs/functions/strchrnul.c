char *strchrnul(const char *s, int c){
	while (*s && *s != (char)c){
		s++;
	}
	return (char *)s;
}
