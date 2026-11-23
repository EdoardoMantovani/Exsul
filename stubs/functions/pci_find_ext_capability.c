u16 pci_find_ext_capability(struct pci_dev *pci_dev, int cap){
	#ifdef EXSUL_PCI_SUPPORT
	return pci_find_next_ext_capability(pci_dev, 0, cap);
	#else
	return 0;
	#endif
}
