int usb_register_driver(struct usb_driver *new_driver, struct module *owner, const char *mod_name){
	int ret = 0;
	unsigned char *probe_name = NULL;
	struct usb_device_id *id  = NULL;
	struct usb_interface *intf = NULL;
	const struct usb_device_id *matched_id = NULL;

	if(new_driver == NULL){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] \'usb_driver\' is invalid!\n");
		#endif
		ret = -EINVAL;
		return ret;
	}

	if(new_driver->probe == NULL){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] %s: the \'probe\' callback is invalid!\n", new_driver->name);
		#endif
		ret = -EINVAL;
		return ret;
	}

	director.bound_usb_driver = new_driver;

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
