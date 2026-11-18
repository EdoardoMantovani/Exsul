int kstrtobool_from_user(const char __user *s, int count, bool *res){
	int ret = 0;

	ret = kstrtobool(s, res);
	return ret;
}
