unsigned long hweight_long(unsigned long w){
	return sizeof(w) == 4 ? __sw_hweight32(w) : __sw_hweight64((__u64)w);
}

