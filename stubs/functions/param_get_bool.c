int param_get_bool(char *buffer, const struct kernel_param *kp){
	int ret = 0;

	/* Y and N chosen as being relatively non-coder friendly */
	ret = sprintf(buffer, "%c\n", *(bool *)kp->arg ? 'Y' : 'N');
	return ret;
}
