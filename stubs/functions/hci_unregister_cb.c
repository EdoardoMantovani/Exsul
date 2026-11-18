int hci_unregister_cb(struct hci_cb *cb){
	int ret = 0;

	list_del(&cb->list);
	return ret;
}
