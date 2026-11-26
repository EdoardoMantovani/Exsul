void pcim_pin_device(struct pci_dev *pci_dev){
	#ifdef EXSUL_PCI_SUPPORT
	pci_dev->pinned = true;
	#endif
}
