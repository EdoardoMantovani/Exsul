int pci_read_config_dword(const struct pci_dev *pci_dev, int where, u32 *val){
	int ret = 0;

	#ifdef EXSUL_PCI_SUPPORT
	ret = pci_config_read(where, val, sizeof(*val));
	#endif
	return ret;
}
