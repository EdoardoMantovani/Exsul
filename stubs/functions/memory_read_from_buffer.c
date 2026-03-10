ssize_t memory_read_from_buffer(void *to, size_t count, loff_t *ppos, const void *from, size_t available){
	loff_t pos = *ppos;

	if (pos < 0){
		return -EINVAL;
	}
	if (pos >= available){
		return 0;
	}
	if (count > available - pos){
		count = available - pos;
	}

	memcpy(to, from + pos, count);
	*ppos = pos + count;

	return count;
}
