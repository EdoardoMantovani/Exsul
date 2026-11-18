int filp_close(struct file *file, void *id){
	int ret = 0;
	struct inode *inode = NULL;

	if(file != NULL){
		inode = file->f_inode;
		ret = close(file->f_flags);
		free(inode);
		free(file);
		return ret;
	}else{
		ret = -EINVAL;
		return ret;
	}
}
