int usb_register_driver(struct usb_driver *new_driver, struct module *owner, const char *mod_name){
	int ret = 0;
	unsigned char *probe_name = NULL;
	struct usb_device_id *id = NULL;
	struct usb_interface *intf = NULL;
	const struct usb_device_id *matched_id = NULL;

	if(new_driver == NULL){
		ret = -EINVAL;
		return ret;
	}

	if(new_driver->probe == NULL){
		ret = -EINVAL;
		return ret;
	}

	ret = fast_get_name(new_driver->probe, &probe_name);
	if(ret == 0){
		printf("[!] function has size 0!\n");
		ret = -EINVAL;
		return ret;
	}else{
		printf("[*] probe address: 0x%08x probe name: %s module_name: %s\n", new_driver->probe, probe_name, mod_name);
	}

	/*
	 * Steps for running the usb driver:
	 * 1) read the symbol '__mod_device_table__usb__' and get the usb pid/vid table
	 * 2) wait for the usb device to be plugged && detect the usb endpoints
	 * 3) call the ->probe() function from the main driver loaded in memory
	 */
	ret = parse_module_usb_table(&id);
	if(ret || id == NULL){
		printf("[!] failed to parse the usb vid/pid table, aborting...\n");
		ret = -EINVAL;
		return ret;
	}else{
		director.usb_device_id = id;
	}

	usb_interface:
	/* update: add a specific "matched_id" which refers to the 'usb_device_id' linked with the current attached usb dongle */
	ret = detect_usb_device(id, &intf, &matched_id);
	if(ret || intf == NULL || matched_id == NULL){
		printf("[!] failed to detect the usb dongle! aborting...\n");
		ret = -EIO;
		return ret;
	}else{
		director.usb_interface = intf;
	}
	end:
	/* finally, invoke the probe callback */
	ret = invoke_usb_probe(new_driver, matched_id, intf);
	return ret;
}
