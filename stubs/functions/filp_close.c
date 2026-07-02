int filp_close(struct file *file, void *id){
	int ret = 0;

	if(file != NULL){
		close(file->f_flags);
		return ret;
	}else{
		ret = -EINVAL;
		return ret;
	}
}
