struct file_layer *filp_open(const char *filename, int flags, u16 mode){
	int fd = 0;
	struct file_layer *ptr_file_table = NULL;
	struct file_layer *current_file_layer = NULL;

	fd = open(filename, flags, mode);
	if(fd < 0){
		printf("[!] failed to open file: %s\n", filename);
		return NULL;
	}

	/* associate 'current_file_layer' with a file ptr in the director */
	for(u8 current_file_idx = 0; current_file_idx < MAX_FILE_IDX; current_file_idx++){
		ptr_file_table = &director.file_table[current_file_idx];
		if(ptr_file_table->actual_fd != 0xFF){
			current_file_layer = ptr_file_table;
			current_file_layer->actual_fd = fd;
			break;
		}
	}

	return current_file_layer;
}
