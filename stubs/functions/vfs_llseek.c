int vfs_llseek(struct file_layer *file, u32 offset, int whence){
	int ret = 0;
	if(file == NULL || file->actual_fd <= 0){
		ret = -EINVAL;
		return ret;
	}

	ret = lseek(file->actual_fd, offset, whence);
	return ret;
}
