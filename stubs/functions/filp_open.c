struct file *filp_open(const char *filename, int flags, u16 mode){
	int fd = 0;
	struct file *file = NULL;

	fd = open(filename, flags, mode);
	if(fd < 0){
		printf("[!] failed to open file: %s\n", filename);
		file = (void *)-ENOENT;
		return file;
	}

	file = calloc(1, director.struct_sizes.file);
	if(file == NULL){
		close(fd);
		file = (void *)-ENOENT;
		return file;
	}

	/* due to the fact that 'struct file' doesn't have the place for the fd, use the 'f_flags' field */
	file->f_flags = fd;
	return file;
}
