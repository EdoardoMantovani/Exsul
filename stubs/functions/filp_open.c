struct file *filp_open(const char *filename, int flags, u16 mode){
	int ret = 0;
	int fd = 0;
	int used_fallback = 0;
	struct file *file = NULL;
	struct inode *inode = NULL;
	struct runtime_struct_layout *layout = NULL;
	struct stat st = {};

	for(int i = 0; director.runtime_generated_structs != NULL && director.runtime_generated_structs->structs[i].struct_name != NULL; i++){
		layout = &director.runtime_generated_structs->structs[i];
		if(strcmp(layout->struct_name, "file") == 0){
			if(layout->struct_size < 0x78){
				layout->struct_size = 0x78;
				used_fallback = 1;
			}
			for(int k = 0; k < layout->struct_field_num; k++){
				if(strcmp(layout->fields[k].field_name, "f_inode") == 0 && layout->fields[k].field_offset == 0){
					layout->fields[k].field_offset = 0x20;
					used_fallback = 1;
				}
				if(strcmp(layout->fields[k].field_name, "f_pos") == 0 && layout->fields[k].field_offset == 0){
					layout->fields[k].field_offset = 0x70;
					used_fallback = 1;
				}
			}
			director.struct_sizes.file = layout->struct_size;
		}
		if(strcmp(layout->struct_name, "inode") == 0){
			if(layout->struct_size < 0x58){
				layout->struct_size = 0x58;
				used_fallback = 1;
			}
			for(int k = 0; k < layout->struct_field_num; k++){
				if(strcmp(layout->fields[k].field_name, "i_size") == 0 && layout->fields[k].field_offset == 0){
					layout->fields[k].field_offset = 0x50;
					used_fallback = 1;
				}
			}
			director.struct_sizes.inode = layout->struct_size;
		}
	}
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
	inode = calloc(1, director.struct_sizes.inode);
	if(inode == NULL){
		close(fd);
		free(file);
		file = (void *)-ENOMEM;
		return file;
	}
	ret = fstat(fd, &st);
	if(ret < 0){
		close(fd);
		free(inode);
		free(file);
		file = (void *)-EIO;
		return file;
	}

	/* due to the fact that 'struct file' doesn't have the place for the fd, use the 'f_flags' field */
	file->f_flags = fd;
	file->f_inode = inode;
	file->f_pos = 0;
	inode->i_size = st.st_size;
	return file;
}
