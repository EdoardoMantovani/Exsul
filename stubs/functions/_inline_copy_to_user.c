unsigned long _inline_copy_to_user(void *to, void *from, unsigned long n){
	int ret = 0;

	if(n < sizeof(director.driver_to_userspace_buffer)){
		memcpy(&director.driver_to_userspace_buffer, from, n);
		return ret;
	}else{
		memset(&director.driver_to_userspace_buffer, 0xFF, sizeof(director.driver_to_userspace_buffer));
		ret = -EINVAL;
		return ret;
	}
}
