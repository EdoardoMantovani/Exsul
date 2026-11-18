int kernel_read(struct file *file, void *buf, size_t count, loff_t *pos){
	int ret = 0;

	if(file == NULL || pos == NULL){
		ret = -EINVAL;
		return ret;
	}
	/* retrieve the 'fd' by using the fake 'f_flags' field */
	if(lseek(file->f_flags, *pos, SEEK_SET) < 0){
		ret = -EIO;
		return ret;
	}
	ret = read(file->f_flags, buf, count);
	if(ret > 0){
		*pos += ret;
	}
	return ret;
}
