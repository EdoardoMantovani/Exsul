u16 pci_find_next_ext_capability(struct pci_dev *pci_dev, u16 start, int cap){
	int ret = 0;

	#ifdef EXSUL_PCI_SUPPORT
	if (pci_dev->cfg_size <= PCI_CFG_SPACE_SIZE){
		return ret;
	}
	ret = PCI_FIND_NEXT_EXT_CAP(pci_bus_read_config, start, cap, NULL, pci_dev->bus, pci_dev->devfn);
	#endif
	return ret;
}
