char *strim(char *s){
	size_t size = 0;
	char *end = NULL;

	size = strlen(s);
	if (!size){
		return s;
	}

	end = s + size - 1;
	while (end >= s && isspace(*end)){
		end--;
	}
	*(end + 1) = '\0';

	return skip_spaces(s);
}
