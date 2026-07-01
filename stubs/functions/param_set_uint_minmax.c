int param_set_uint_minmax(const char *val, const struct kernel_param *kp, unsigned int min, unsigned int max){
	int ret = 0;
	unsigned int num = 0;

	if (!val){
		ret = -EINVAL;
		return ret;
	}
	ret = kstrtouint(val, 0, &num);
	if (ret){
		return ret;
	}
	if (num < min || num > max){
		ret = -EINVAL;
		return ret;
	}
	*((unsigned int *)kp->arg) = num;
	return ret;
}
