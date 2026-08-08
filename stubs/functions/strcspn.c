size_t strcspn(const char *s, const char *reject){
	const char *p = NULL;

	for (p = s; *p != '\0'; ++p) {
		if (strchr(reject, *p)){
			break;
		}
	}
	return p - s;
}
