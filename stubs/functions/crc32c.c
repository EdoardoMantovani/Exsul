u32 crc32c(u32 crc, const void *p, size_t len){
	return crc32c_arch(crc, p, len);
}
