int param_set_charp(const char *val, const struct kernel_param *kp){
	int ret = 0;
	size_t len = 0;
	size_t maxlen = 1024;

	len = strnlen(val, maxlen + 1);
	if (len == maxlen + 1) {
		pr_err("%s: string parameter too long\n", kp->name);
		ret = -EINVAL;
		return ret;
	}

	/*
	 * This is a hack. We can't kmalloc() in early boot, and we
	 * don't need to; this mangled commandline is preserved.
	 */
	*(const char **)kp->arg = val;

	return ret;
}
