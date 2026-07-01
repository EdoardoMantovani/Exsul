void *proc_mkdir_data(const char *name, unsigned short mode, void *parent, void *data){
	static unsigned char proc_dir;

	return &proc_dir;
}
