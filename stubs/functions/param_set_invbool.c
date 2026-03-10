int param_set_invbool(const char *val, const struct kernel_param *kp){
	int ret = 0;
	bool boolval = false;
	struct kernel_param dummy = {};

	dummy.arg = &boolval;
	ret = param_set_bool(val, &dummy);
	if (ret == 0){
		*(bool *)kp->arg = !boolval;
	}
	return ret;
}
