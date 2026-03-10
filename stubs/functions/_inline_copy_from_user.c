unsigned long _inline_copy_from_user(void *to, const void *from, unsigned long n){
	memcpy(to, from, n);
}
