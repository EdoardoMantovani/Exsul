const char *dev_driver_string(const struct device *dev){
	struct device_driver *drv = NULL;

	drv = dev->driver;
	return drv ? drv->name : "generic-usb";
}

