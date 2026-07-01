void baswap(char *dst, const char *src){
	unsigned char *d = NULL;
	const unsigned char *s = NULL;

	s = (const unsigned char *)src;
	d = (unsigned char *)dst;

	for (u8 i = 0; i < 6; i++){
		d[i] = s[5 - i];
	}
}
