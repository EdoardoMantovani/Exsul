u32 crc32_le(u32 crc, const void *p, size_t len){
	return crc32_le_arch(crc, p, len);
}
