int param_get_invbool(char *buffer, const struct kernel_param *kp){
	return sprintf(buffer, "%c\n", (*(bool *)kp->arg) ? 'N' : 'Y');
}
