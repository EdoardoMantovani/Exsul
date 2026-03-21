int match_string(const char * const *array, size_t n, const char *string){
	int index = 0;
	const char *item = NULL;

	for (index = 0; index < n; index++) {
		item = array[index];
		if (!item){
			break;
		}
		if (!strcmp(item, string)){
			return index;
		}
	}
	return -EINVAL;
}
