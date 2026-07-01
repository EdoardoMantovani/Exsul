int hci_register_cb(struct hci_cb *cb){
	int ret = 0;

	printf("%p name %s", cb, cb->name);

	list_add_tail(&cb->list, &director.hci_cb_list);

	return ret;
}
