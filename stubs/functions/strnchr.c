char *strnchr(const char *s, size_t count, int c){
	while (count--) {
		if (*s == (char)c){
			return (char *)s;
		}
		if (*s++ == '\0'){
			break;
		}
	}
	return NULL;
}
