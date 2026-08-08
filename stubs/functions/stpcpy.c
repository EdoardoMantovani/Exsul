char *stpcpy(char *dest, const char *src){
	while ((*dest++ = *src++) != '\0'){
		/* nothing */;
	}
	return --dest;
}
