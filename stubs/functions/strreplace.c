char *strreplace(char *str, char old, char new){
	char *s = str;

	for (; *s; ++s){
		if (*s == old){
			*s = new;
		}
	}
	return str;
}
