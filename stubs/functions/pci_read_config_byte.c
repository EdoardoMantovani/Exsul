int pci_read_config_byte(const struct pci_dev *pci_dev, int where, u8 *value){
	int ret = 0;

	#ifdef EXSUL_PCI_SUPPORT
	ret = pci_config_read(where, value, sizeof(*value));
	#endif
	return ret;
}
