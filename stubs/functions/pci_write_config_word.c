int pci_write_config_word(const struct pci_dev *pci_dev, int where, u16 val){
	int ret = 0;

	#ifdef EXSUL_PCI_SUPPORT
	ret = pci_config_write(where, &val, sizeof(val));
	#endif
	return ret;
}
