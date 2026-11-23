void __iomem *pci_iomap(struct pci_dev *pci_dev, int bar, unsigned long maxlen){
	#ifdef EXSUL_PCI_SUPPORT
	return pci_iomap_range(pci_dev, bar, 0, maxlen);
	#else
	return NULL;
	#endif
}
