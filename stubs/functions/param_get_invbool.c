int param_get_invbool(char *buffer, const struct kernel_param *kp){
	int ret = 0;

	ret = sprintf(buffer, "%c\n", (*(bool *)kp->arg) ? 'N' : 'Y');
	return ret;
}
