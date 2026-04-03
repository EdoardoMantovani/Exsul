void *memmove(void *dest, const void *src, size_t count){
	char *tmp = NULL;
	const char *s = NULL;

	if (dest <= src) {
		tmp = dest;
		s = src;
		while (count--){
			*tmp++ = *s++;
		}
	}else{
		tmp = dest;
		tmp += count;
		s = src;
		s += count;
		while (count--){
			*--tmp = *--s;
		}
	}
	return dest;
}
