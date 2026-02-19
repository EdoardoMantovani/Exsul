void release_firmware(const struct firmware *fw){
	if (fw != NULL) {
		free(fw);
	}
}
