struct kernel_param_ops param_ops_charp = {
	.set = param_set_charp,
	.get = param_get_charp,
	.free = param_free_charp,
};
