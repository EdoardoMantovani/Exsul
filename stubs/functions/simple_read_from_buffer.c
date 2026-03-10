int simple_read_from_buffer(void __user *to, size_t count, loff_t *ppos, const void *from, size_t available){
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
	ret = copy_to_user(to, from + pos, count);
	if (ret == count){
		ret = -EFAULT;
		return ret;
	}
	count -= ret;
	*ppos = pos + count;
	return count;
}
