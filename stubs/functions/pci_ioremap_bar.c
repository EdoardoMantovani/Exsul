void *pci_ioremap_bar(struct pci_dev *pci_dev, int bar){
	#ifdef EXSUL_PCI_SUPPORT
	return vfio_map_bar(bar);
	#else
	return NULL;
	#endif
}
