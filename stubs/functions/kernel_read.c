int kernel_read(struct file_layer *file, void *buf, size_t count, int *pos){
	int ret = 0;

	if(file == NULL || file->actual_fd <= 0){
		ret = -EINVAL;
		return ret;
	}

	lseek(file->actual_fd, *pos, SEEK_SET);
	ret = read(file->actual_fd, buf, count);
	return ret;
}
