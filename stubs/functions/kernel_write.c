int kernel_write(struct file *file, const void *buf, size_t count, u32 *pos){
	int ret = 0;

	if(file == NULL || file->f_flags <= 0){
		ret = -EINVAL;
		return ret;
	}

	/* retrieve the 'fd' by using the fake 'f_flags' field */
	lseek(file->f_flags, *pos, SEEK_SET);
	ret = write(file->f_flags, buf, count);
	return ret;
}
