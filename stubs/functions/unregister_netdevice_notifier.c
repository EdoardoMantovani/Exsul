int unregister_netdevice_notifier(struct notifier_block *nb){
	int ret = 0;

	if(nb == NULL){
		ret = -EINVAL;
		return ret;
	}
	for(int i = 0; i < MAX_NETDEVICE_NOTIFIERS; i++){
		if(netdevice_notifiers[i] == nb){
			netdevice_notifiers[i] = NULL;
			return ret;
		}
	}
	return ret;
}
