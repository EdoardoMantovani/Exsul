struct pci_dev *pci_get_device(unsigned int vendor, unsigned int device, struct pci_dev *from){
	#ifdef EXSUL_PCI_SUPPORT
	return pci_get_subsys(vendor, device, PCI_ANY_ID, PCI_ANY_ID, from);
	#else
	return NULL;
	#endif
}
