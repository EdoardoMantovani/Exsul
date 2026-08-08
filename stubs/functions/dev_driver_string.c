const char *dev_driver_string(const struct device *dev){
	struct device_driver *drv = NULL;

	/* dev->driver can change to NULL underneath us because of unbinding,
	 * so be careful about accessing it.  dev->bus and dev->class should
	 * never change once they are set, so they don't need special care.
	 */
	drv = dev->driver;
	return drv ? drv->name : "generic-usb";
}

