ssize_t memory_read_from_buffer(void *to, size_t count, loff_t *ppos, const void *from, size_t available){
	int ret = 0;
	loff_t pos = *ppos;

	if (pos < 0){
		ret = -EINVAL;
		return ret;
	}
	if (pos >= available){
		return ret;
	}
	if (count > available - pos){
		count = available - pos;
	}

	memcpy(to, from + pos, count);
	*ppos = pos + count;

	return count;
}
