int pcie_capability_write_word(struct pci_dev *pci_dev, int pos, u16 val){
	int ret = 0;

	#ifdef EXSUL_PCI_SUPPORT
	if (pos & 1){
		ret = PCIBIOS_BAD_REGISTER_NUMBER;
		return ret;
	}
	ret = pcie_capability_reg_implemented(pci_dev, pos);
	if(ret == 0){
		return ret;
	}
	ret = pci_write_config_word(pci_dev, pci_pcie_cap(pci_dev) + pos, val);
	#endif
	return ret;
}
