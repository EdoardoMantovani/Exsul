int pci_set_mwi(struct pci_dev *pci_dev){
	int ret = 0;

	#ifndef EXSUL_PCI_SUPPORT
	return ret;
	#else
	u16 cmd = 0;

	ret = pci_set_cacheline_size(pci_dev);
	if (ret){
		return ret;
	}
	pci_read_config_word(pci_dev, PCI_COMMAND, &cmd);
	if (!(cmd & PCI_COMMAND_INVALIDATE)) {
		cmd |= PCI_COMMAND_INVALIDATE;
		pci_write_config_word(pci_dev, PCI_COMMAND, cmd);
	}
	return ret;
	#endif
}
