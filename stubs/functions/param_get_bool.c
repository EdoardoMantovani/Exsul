int param_get_bool(char *buffer, const struct kernel_param *kp){
	/* Y and N chosen as being relatively non-coder friendly */
	return sprintf(buffer, "%c\n", *(bool *)kp->arg ? 'Y' : 'N');
}
