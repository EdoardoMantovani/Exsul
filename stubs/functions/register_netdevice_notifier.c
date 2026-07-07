int register_netdevice_notifier(struct notifier_block *nb){
	int ret = 0;

	if(nb == NULL){
		ret = -EINVAL;
		return ret;
	}

	for(int i = 0; i < MAX_NETDEVICE_NOTIFIERS; i++){
		if(netdevice_notifiers[i] == nb){
			return ret;
		}
	}

	for(int i = 0; i < MAX_NETDEVICE_NOTIFIERS; i++){
		if(netdevice_notifiers[i] == NULL){
			netdevice_notifiers[i] = nb;
			return ret;
		}
	}

	ret = -ENOMEM;
	return ret;
}
