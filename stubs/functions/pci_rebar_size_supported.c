bool pci_rebar_size_supported(struct pci_dev *pci_dev, int bar, int size){
	u64 sizes = 0;

	#ifdef EXSUL_PCI_SUPPORT
	sizes = pci_rebar_get_possible_sizes(pci_dev, bar);
	if (size < 0 || size > ilog2(SZ_128T) - ilog2(PCI_REBAR_MIN_SIZE)){
		return false;
	}
	return BIT(size) & sizes;
	#else
	return false;
	#endif
}
