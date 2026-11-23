int pcie_capability_read_word(struct pci_dev *pci_dev, int pos, u16 *val){
	int ret = 0;

	#ifdef EXSUL_PCI_SUPPORT
	*val = 0;
	if (pos & 1){
		ret = PCIBIOS_BAD_REGISTER_NUMBER;
		return ret;
	}
	ret = pcie_capability_reg_implemented(pci_dev, pos);
	if(ret) {
		ret = pci_read_config_word(pci_dev, pci_pcie_cap(pci_dev) + pos, val);
		/*
		 * Reset *val to 0 if pci_read_config_word() fails; it may
		 * have been written as 0xFFFF (PCI_ERROR_RESPONSE) if the
		 * config read failed on PCI.
		 */
		if (ret){
			*val = 0;
		}
		return ret;
	}

	/*
	 * For Functions that do not implement the Slot Capabilities,
	 * Slot Status, and Slot Control registers, these spaces must
	 * be hardwired to 0b, with the exception of the Presence Detect
	 * State bit in the Slot Status register of Downstream Ports,
	 * which must be hardwired to 1b.  (PCIe Base Spec 3.0, sec 7.8)
	 */
	if (pci_is_pcie(pci_dev) && pcie_downstream_port(pci_dev) && pos == PCI_EXP_SLTSTA){
		*val = PCI_EXP_SLTSTA_PDS;
	}
	#endif
	return ret;
}
