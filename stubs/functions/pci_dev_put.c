void pci_dev_put(struct pci_dev *pci_dev){
	#ifdef EXSUL_PCI_SUPPORT
	if (pci_dev != NULL){
		put_device(&pci_dev->dev);
	}
	#endif
}
