void baswap(char *dst, const char *src){
	const unsigned char *s = (const unsigned char *)src;
	unsigned char *d = (unsigned char *)dst;

	for (u8 i = 0; i < 6; i++){
		d[i] = s[5 - i];
	}
}
