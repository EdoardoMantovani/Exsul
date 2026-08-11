u32 crc32_be(u32 crc, const void *p, size_t len){
	return crc32_be_arch(crc, p, len);
}
