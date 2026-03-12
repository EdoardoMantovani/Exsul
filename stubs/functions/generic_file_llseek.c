int generic_file_llseek(struct file_layer *file, int offset, int whence){
	int ret = 0;

	if(file == NULL || file->actual_fd <= 0){
		ret = -EINVAL;
		return ret;
	}

	ret = lseek(file->actual_fd, offset, whence);
	return ret;
}

