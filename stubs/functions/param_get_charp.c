int param_get_charp(char *buffer, const struct kernel_param *kp){
	int ret = 0;

	ret = scnprintf(buffer, PAGE_SIZE, "%s\n", *((char **)kp->arg));
	return ret;
}
