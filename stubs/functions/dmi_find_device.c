const struct dmi_device *dmi_find_device(int type, const char *name, const struct dmi_device *from){
	struct list_head *d = NULL;
	const struct list_head *head = NULL;
	const struct dmi_device *dmi_device = NULL;

	if(from != NULL){
		head = &from->list;
	}else{
		/*
		 * for now, toggle this line 'head = &dmi_devices;', this because
		 * 'dmi_devices' is a list that CAN be retrieved during the build part of Exsul, but for now
		 * it isn't necessary to implement */
		return NULL;
	}
	for (d = head->next; d != NULL /*&dmi_devices*/; d = d->next) {
		dmi_device = list_entry(d, struct dmi_device, list);

		if (((type == DMI_DEV_TYPE_ANY) || (dmi_device->type == type)) && ((name == NULL) || (strcmp(dmi_device->name, name) == 0))){
			return dmi_device;
		}
	}

	return NULL;
}
