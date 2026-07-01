int seq_open(void *file, const struct seq_operations *op){
	int ret = 0;
	struct seq_file *p = NULL;

	if (p == NULL){
		ret = -ENOMEM;
		return ret;
	}

	return ret;
}
