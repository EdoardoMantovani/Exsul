char *strpbrk(const char *cs, const char *ct){
	const char *sc = NULL;

	for (sc = cs; *sc != '\0'; ++sc) {
		if (strchr(ct, *sc)){
			return (char *)sc;
		}
	}
	return NULL;
}
