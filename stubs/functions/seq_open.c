int seq_open(void *file, const struct seq_operations *op){
	int ret = 0;
	struct seq_file *p = NULL;

	//p = kmem_cache_zalloc(sizeof(struct );
	if (p == NULL){
		ret = -ENOMEM;
		return ret;
	}


	return ret;
}
