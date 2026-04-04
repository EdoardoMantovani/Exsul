int kernel_write(struct file_layer *file, const void *buf, size_t count, u32 *pos){
	int ret = 0;

	if(file == NULL || file->actual_fd <= 0){
		ret = -EINVAL;
		return ret;
	}

	lseek(file->actual_fd, *pos, SEEK_SET);
	ret = write(file->actual_fd, buf, count);
	return ret;
}
