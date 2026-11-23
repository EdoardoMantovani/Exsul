int pci_set_pcie_reset_state(struct pci_dev *pci_dev, int state){
	int ret = 0;

	/* 'pcibios_set_pcie_reset_state' isn't implemented in x86, arm64 ecc.. */
	ret = -EINVAL;
	return ret;
}
