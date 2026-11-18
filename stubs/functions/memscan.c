void *memscan(void *addr, int c, size_t size){
	unsigned char *p = NULL;

	p = addr;
	while (size) {
		if (*p == (unsigned char)c){
			return (void *)p;
		}
		p++;
		size--;
	}
	return (void *)p;
}
