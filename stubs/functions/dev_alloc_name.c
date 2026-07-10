int dev_alloc_name(void *dev, const char *name){
	int ret = 0;
	char tmpl[IFNAMSIZ];
	struct net_device *netdev = dev;

	if(netdev == NULL || name == NULL){
		ret = -EINVAL;
		return ret;
	}

	strncpy(tmpl, name, sizeof(tmpl) - 1);
	tmpl[sizeof(tmpl) - 1] = '\0';
	if(strchr(tmpl, '%') != NULL){
		snprintf((char *)&netdev->name, IFNAMSIZ, tmpl, 0);
	}else{
		strncpy((char *)&netdev->name, tmpl, IFNAMSIZ - 1);
		((char *)&netdev->name)[IFNAMSIZ - 1] = '\0';
	}
	return ret;
}
