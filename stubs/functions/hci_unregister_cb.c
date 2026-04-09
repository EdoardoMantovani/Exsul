int hci_unregister_cb(struct hci_cb *cb){
	printf("%p name %s", cb, cb->name);

	list_del(&cb->list);

	return 0;
}
