size_t strlen(const char *s){
	const char *sc = s;

	while (*sc != '\0')
		++sc;
	return sc - s;
}
