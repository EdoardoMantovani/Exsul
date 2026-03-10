int param_get_charp(char *buffer, const struct kernel_param *kp){
	return scnprintf(buffer, PAGE_SIZE, "%s\n", *((char **)kp->arg));
}
