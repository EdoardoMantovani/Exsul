int pcie_capability_read_dword(struct pci_dev *pci_dev, int pos, u32 *val){
	int ret = 0;

	#ifdef EXSUL_PCI_SUPPORT
	*val = 0;
	if (pos & 3){
		ret = PCIBIOS_BAD_REGISTER_NUMBER;
		return ret;
	}
	ret = pcie_capability_reg_implemented(pci_dev, pos);
	if(ret) {
		ret = pci_read_config_dword(pci_dev, pci_pcie_cap(pci_dev) + pos, val);
		/*
		 * Reset *val to 0 if pci_read_config_dword() fails; it may
		 * have been written as 0xFFFFFFFF (PCI_ERROR_RESPONSE) if
		 * the config read failed on PCI.
		 */
		if (ret){
			*val = 0;
		}
		return ret;
	}

	if (pci_is_pcie(pci_dev) && pcie_downstream_port(pci_dev) && pos == PCI_EXP_SLTSTA){
		*val = PCI_EXP_SLTSTA_PDS;
	}
	#endif
	return ret;
}
