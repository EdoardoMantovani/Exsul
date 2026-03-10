int simple_write_to_buffer(void *to, size_t available, loff_t *ppos, const void __user *from, size_t count){
	int ret = 0;
	loff_t pos = *ppos;

	if (pos < 0){
		ret = -EINVAL;
		return ret;
	}
	if (pos >= available || !count){
		return ret;
	}
	if (count > available - pos){
		count = available - pos;
	}
	ret = copy_from_user(to + pos, from, count);
	if (ret == count){
		ret = -EFAULT;
		return ret;
	}
	count -= ret;
	*ppos = pos + count;
	return count;
}
