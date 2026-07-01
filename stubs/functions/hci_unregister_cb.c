int hci_unregister_cb(struct hci_cb *cb){
	int ret = 0;

	printf("%p name %s", cb, cb->name);
	list_del(&cb->list);

	return ret;
}
