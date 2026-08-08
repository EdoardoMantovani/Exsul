char *strcpy(char *dest, const char *src){
	char *tmp = NULL;

	tmp = dest;
	while ((*dest++ = *src++) != '\0'){
		/* nothing */;
	}
	return tmp;
}
