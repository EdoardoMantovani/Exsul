int __sysfs_match_string(const char * const *array, size_t n, const char *str){
	const char *item = NULL;
	int index = 0;

	for (index = 0; index < n; index++) {
		item = array[index];
		if (!item){
			break;
		}
		if (sysfs_streq(item, str)){
			return index;
		}
	}
	return -EINVAL;
}
