u64 pci_rebar_get_possible_sizes(struct pci_dev *pci_dev, int bar){
	int ret = 0;
	int pos = 0;
	u32 cap = 0;

	#ifdef EXSUL_PCI_SUPPORT
	pos = pci_rebar_find_pos(pci_dev, bar);
	if (pos < 0){
		return ret;
	}
	pci_read_config_dword(pci_dev, pos + PCI_REBAR_CAP, &cap);
	cap = FIELD_GET(PCI_REBAR_CAP_SIZES, cap);
	#endif
	return cap;
}
