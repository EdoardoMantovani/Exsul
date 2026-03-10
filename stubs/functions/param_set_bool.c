int param_set_bool(const char *val, const struct kernel_param *kp){
	/* No equals means "set"... */
	if (!val){
		val = "1";
	}
	/* One of =[yYnN01] */
	return kstrtobool(val, kp->arg);
}
