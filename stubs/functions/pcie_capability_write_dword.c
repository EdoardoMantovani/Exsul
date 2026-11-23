int pcie_capability_write_dword(struct pci_dev *pci_dev, int pos, u32 val){
	int ret = 0;

	#ifdef EXSUL_PCI_SUPPORT
	if (pos & 3){
		ret = PCIBIOS_BAD_REGISTER_NUMBER;
		return ret;
	}
	ret = pcie_capability_reg_implemented(pci_dev, pos);
	if(ret == 0){
		return ret;
	}
	ret = pci_write_config_dword(pci_dev, pci_pcie_cap(pci_dev) + pos, val);
	#endif
	return ret;
}
