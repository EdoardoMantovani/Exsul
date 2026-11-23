int pci_try_set_mwi(struct pci_dev *pci_dev){
	int ret = 0;

	#ifdef EXSUL_PCI_SUPPORT
	#ifdef PCI_DISABLE_MWI
	return ret;
	#else
	ret = pci_set_mwi(pci_dev);
	return ret;
	#endif
	#endif
}
