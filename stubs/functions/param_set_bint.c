int param_set_bint(const char *val, const struct kernel_param *kp){
	int ret = 0;
	bool v = false;
	/* Match bool exactly, by re-using it. */
	struct kernel_param boolkp = *kp;

	boolkp.arg = &v;

	ret = param_set_bool(val, &boolkp);
	if (ret == 0){
		*(int *)kp->arg = v;
	}
	return ret;
}
