struct device *get_device(struct device *dev){
	return dev; // ? kobj_to_dev(kobject_get(&dev->kobj)) : NULL;
}
