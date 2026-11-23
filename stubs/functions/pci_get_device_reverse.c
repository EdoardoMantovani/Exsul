struct pci_dev *pci_get_device_reverse(unsigned int vendor, unsigned int device, struct pci_dev *from){
	#ifdef EXSUL_PCI_SUPPORT
	return __pci_get_subsys(vendor, device, PCI_ANY_ID, PCI_ANY_ID, from, PCI_SEARCH_REVERSE);
	#else
	return NULL;
	#endif
}
