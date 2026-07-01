int filp_close(struct file_layer *filp, void *id){
	int ret = 0;

	if(filp != NULL){
		close(filp->actual_fd);
		filp->actual_fd = 0xFF;
		filp->actual_offset = 0;
		filp->original_file_ptr = NULL;
		return ret;
	}else{
		ret = -EINVAL;
		return ret;
	}
}
