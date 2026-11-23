int pcie_capability_clear_and_set_dword(struct pci_dev *pci_dev, int pos, u32 clear, u32 set){
	int ret = 0;
	u32 val = 0;

	#ifdef EXSUL_PCI_SUPPORT
	ret = pcie_capability_read_dword(pci_dev, pos, &val);
	if (ret){
		return ret;
	}
	val &= ~clear;
	val |= set;
	ret = pcie_capability_write_dword(pci_dev, pos, val);
	#endif
	return ret;
}
