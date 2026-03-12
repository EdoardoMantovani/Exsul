void put_device(struct device *dev){
	if (dev != NULL){
		kobject_put(&dev->kobj);
	}
}
