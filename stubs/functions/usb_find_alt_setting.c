struct usb_host_interface *usb_find_alt_setting(struct usb_host_config *config, unsigned int iface_num, unsigned int alt_num){
	int ret = 0;
	struct usb_interface *intf = NULL;

	if (config == NULL){
		return NULL;
	}
	for (int i = 0; i < config->desc.bNumInterfaces; i++) {
		if (config->interface[i] != NULL && config->interface[i]->altsetting[0].desc.bInterfaceNumber == iface_num) {
			intf = config->interface[i];
			break;
		}
	}
	if (intf == NULL){
		return NULL;
	}
	for (int i = 0; i < intf->num_altsetting; i++){
		if (intf->altsetting[i].desc.bAlternateSetting == alt_num){
			return &intf->altsetting[i];
		}
	}
	#ifdef ENABLE_DBG_PRINT
	printf("[!] alt setting %u for intf %u not found!\n", alt_num, iface_num);
	#endif
	return NULL;
}
