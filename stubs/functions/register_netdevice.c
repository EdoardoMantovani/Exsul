int register_netdevice(struct net_device *dev){
	int ret = 0;
	struct net_device_ops *ops = NULL;
	int (*ndo_init)(struct net_device *dev) = NULL;

	if(dev == NULL){
		ret = -EINVAL;
		return ret;
	}

	director.net_device = dev;
	if(((char *)&dev->name)[0] == '\0'){
		dev_alloc_name(dev, "wlan%d");
	}else if(strchr((char *)&dev->name, '%') != NULL){
		dev_alloc_name(dev, (char *)&dev->name);
	}

	ops = dev->netdev_ops;
	if(ops != NULL){
		ndo_init = ops->ndo_init;
	}
	if(ndo_init != NULL){
		ret = ndo_init(dev);
		if(ret != 0){
			return ret;
		}
	}

	set_bit(__LINK_STATE_PRESENT, &dev->state);
	dev->reg_state = NETREG_REGISTERED;
	call_netdevice_notifiers(NETDEV_POST_INIT, dev);
	call_netdevice_notifiers(NETDEV_REGISTER, dev);
	return ret;
}
