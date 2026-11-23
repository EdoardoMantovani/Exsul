int pci_rebar_get_max_size(struct pci_dev *pci_dev, int bar){
	int ret = 0;
	u64 sizes = 0;

	#ifdef EXSUL_PCI_SUPPORT
	sizes = pci_rebar_get_possible_sizes(pci_dev, bar);
	if (sizes == 0){
		ret = -ENOENT;
		return ret;
	}
	ret = __fls(sizes);
	#endif
	return ret;
}
