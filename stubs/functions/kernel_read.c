int kernel_read(struct file *file, void *buf, size_t count, int *pos){
	int ret = 0;

	if(file == NULL || file->f_flags <= 0){
		ret = -EINVAL;
		return ret;
	}

	/* retrieve the 'fd' by using the fake 'f_flags' field */
	lseek(file->f_flags, *pos, SEEK_SET);
	ret = read(file->f_flags, buf, count);
	return ret;
}
