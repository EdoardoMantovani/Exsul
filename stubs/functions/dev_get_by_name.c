void *dev_get_by_name(void *net, const char *name){
	struct net_device *dev = director.net_device;

	if(dev == NULL || name == NULL){
		return NULL;
	}
	if(strncmp((char *)&dev->name, name, IFNAMSIZ) == 0){
		return dev;
	}
	return NULL;
}

