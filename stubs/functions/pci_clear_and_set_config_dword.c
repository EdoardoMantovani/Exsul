void pci_clear_and_set_config_dword(const struct pci_dev *pci_dev, int pos, u32 clear, u32 set){
	u32 val = 0;

	#ifdef EXSUL_PCI_SUPPORT
	pci_read_config_dword(pci_dev, pos, &val);
	val &= ~clear;
	val |= set;
	pci_write_config_dword(pci_dev, pos, val);
	#endif
}
