void release_firmware(const struct firmware *fw){
	if(fw != NULL){
		if(fw->data != NULL){
			free((void *)fw->data);
		}
		free((void *)fw);
	}
}
