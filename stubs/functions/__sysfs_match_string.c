int __sysfs_match_string(const char * const *array, size_t n, const char *str){
	int ret = 0;
	int index = 0;
	const char *item = NULL;

	for (index = 0; index < n; index++) {
		item = array[index];
		if (!item){
			break;
		}
		if (sysfs_streq(item, str)){
			return index;
		}
	}

	ret = -EINVAL;
	return ret;
}
