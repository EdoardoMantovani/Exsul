int vfs_llseek(struct file *file, u32 offset, int whence){
	int ret = 0;

	if(file == NULL){
		ret = -EINVAL;
		return ret;
	}
	if(file->f_flags <= 0){
		ret = -EIO;
		return ret;
	}
	ret = lseek(file->f_flags, offset, whence);
	return ret;
}
