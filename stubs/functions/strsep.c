char *strsep(char **s, const char *ct){
	char *end = NULL;
	char *sbegin = NULL;

	sbegin = *s;
	if (sbegin == NULL){
		return NULL;
	}
	end = strpbrk(sbegin, ct);
	if (end){
		*end++ = '\0';
	}
	*s = end;
	return sbegin;
}
