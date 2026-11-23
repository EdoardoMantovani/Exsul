struct pci_dev *pci_get_subsys(unsigned int vendor, unsigned int device, unsigned int ss_vendor, unsigned int ss_device, struct pci_dev *from){
	#ifdef EXSUL_PCI_SUPPORT
	return __pci_get_subsys(vendor, device, ss_vendor, ss_device, from, PCI_SEARCH_FORWARD);
	#else
	return NULL;
	#endif
}
