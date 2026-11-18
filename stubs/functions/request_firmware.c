int request_firmware(const struct firmware **firmware_p, const char *name, void *device){
	int ret = 0;
	int fd = 0;
	int nread = 0;
	int total = 0;
	struct stat st = {};
	struct firmware *fw = NULL;
	unsigned char *buf = NULL;
	unsigned char fw_path[48];

        if (firmware_p == NULL || name == NULL){
		ret = -EINVAL;
		return ret;
	}
	*firmware_p = NULL;
	/* for now this is a temp fix, seems that the standard snprintf crash, so I had to reimplement it in my src/main.c */
	ret = snprintf(fw_path, sizeof(fw_path), "%s%s", DEFAULT_FW_PATH, name);
	fd = open(fw_path, O_RDONLY);
	if (fd < 0){
		ret = -errno;
		return ret;
	}
	if (fstat(fd, &st) < 0) {
		ret = -errno;
		close(fd);
		return ret;
	}

	if (st.st_size <= 0) {
		close(fd);
		ret = -EINVAL;
		return ret;
	}

	fw = calloc(1, sizeof(*fw));
	if (fw == NULL) {
		close(fd);
		ret = -ENOMEM;
		return ret;
	}

	buf = malloc((size_t)st.st_size);
	if (buf == NULL) {
		free(fw);
		close(fd);
		ret = -ENOMEM;
		return ret;
	}

	while (total < st.st_size) {
		nread = read(fd, buf + total, (size_t)st.st_size - total);
		if (nread < 0) {
			ret = -errno;
			free(buf);
			free(fw);
			close(fd);
			return ret;
		}
		if (nread == 0) {
			free(buf);
			free(fw);
			close(fd);
			ret = -EIO;
			return ret;
		}
		total += (size_t)nread;
	}

        close(fd);

	fw->size = (size_t)st.st_size;
	fw->data = buf;
	fw->priv = NULL;

	director.firmware_ptr = fw;
	director.firmware_ptr->data = fw->data;
	*firmware_p = fw;
	ret = 0;
	return ret;
}
