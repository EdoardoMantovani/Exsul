bool hci_discovery_active(struct hci_dev *hdev){
	struct discovery_state *discov = NULL;

	discov = &hdev->discovery;
	switch (discov->state) {
		case DISCOVERY_FINDING:
		case DISCOVERY_RESOLVING:
			return true;
		default:
			return false;
	}
}
