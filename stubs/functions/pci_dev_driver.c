struct pci_driver *pci_dev_driver(const struct pci_dev *pci_dev){
	#ifdef EXSUL_PCI_SUPPORT
	if (pci_dev->driver != NULL){
		return pci_dev->driver;
	}
	#endif
	return NULL;
}
