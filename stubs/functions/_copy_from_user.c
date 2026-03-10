unsigned long _copy_from_user(void *to, const void __user *from, unsigned long n){
	return _inline_copy_from_user(to, from, n);
}
