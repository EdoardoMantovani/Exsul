size_t strnlen(const char *s, size_t count){
	const char *sc = NULL;

	for (sc = s; count-- && *sc != '\0'; ++sc){
		/* nothing */;
	}
	return sc - s;
}
