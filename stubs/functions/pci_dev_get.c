struct pci_dev *pci_dev_get(struct pci_dev *pci_dev){
	#ifdef EXSUL_PCI_SUPPORT
	if (pci_dev != NULL){
		get_device(&pci_dev->dev);
	}
	return pci_dev;
	#else
	return NULL;
	#endif
}
