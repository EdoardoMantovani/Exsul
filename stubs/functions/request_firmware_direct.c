int request_firmware_direct(const struct firmware **firmware, const char *name, void *device){
	int ret = 0;

	ret = request_firmware(firmware, name, device);
	return ret;
}
