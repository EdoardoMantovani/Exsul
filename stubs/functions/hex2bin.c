int hex2bin(u8 *dst, const char *src, size_t count){
	int ret = 0;
	int hi = 0;
	int lo = 0;

	while (count--) {
		hi = hex_to_bin(*src++);
		lo = hex_to_bin(*src++);

		if ((hi < 0) || (lo < 0)){
			ret = -EINVAL;
			return ret;
		}
		*dst++ = (hi << 4) | lo;
	}
	return ret;
}
