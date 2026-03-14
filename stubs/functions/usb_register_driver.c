int usb_register_driver(struct usb_driver *new_driver, struct module *owner, const char *mod_name){
	int ret = 0;

	if(new_driver == NULL){
		ret = -EINVAL;
		return ret;
	}

	/*
	 * Steps for running the usb driver:
	 * 1) read the symbol '__mod_device_table__usb__' and get the usb pid/vid table
	 * 2) wait for the usb device to be plugged && detect the usb endpoints
	 * 3) call the ->probe() function from the main driver loaded in memory
	 */

	ret = parse_module_usb_table();
	if(ret){
		printf("[!] failed to parse the usb vid/pid table, aborting...\n");
		ret = -EINVAL;
		return ret;
	}

	new_driver->probe(NULL, NULL);
	return ret;
}
