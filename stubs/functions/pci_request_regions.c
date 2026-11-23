int pci_request_regions(struct pci_dev *pci_dev, const char *name){
	int ret = 0;

	/* this function won't be used in the practice because the regions are already in the PCIe context */
	return ret;
}
