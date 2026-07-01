int param_set_bool(const char *val, const struct kernel_param *kp){
	int ret = 0;

	/* No equals means "set"... */
	if (!val){
		val = "1";
	}
	/* One of =[yYnN01] */
	ret = kstrtobool(val, kp->arg);
	return ret;
}
