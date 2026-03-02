int request_firmware(struct firmware **firmware_p, const char *name, void *device){
	int ret = 0;
	unsigned int size = 0;
	struct stat fs = {};

	ret = open(name, O_RDONLY);
	if(ret < 0){
		ret = -EINVAL;
		return ret;
	}
	fstat(&fs, ret);
	size = fs.st_size;
	memset(&fs, 0, sizeof(struct stat));
	(*firmware_p)->size = size;
	(*firmware_p)->data = malloc(size);

	return ret;
}

