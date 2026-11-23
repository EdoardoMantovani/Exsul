void pci_clear_mwi(struct pci_dev *pci_dev){
	#ifndef PCI_DISABLE_MWI
	u16 cmd = 0;

	#ifdef EXSUL_PCI_SUPPORT
	pci_read_config_word(pci_dev, PCI_COMMAND, &cmd);
	if (cmd & PCI_COMMAND_INVALIDATE) {
		cmd &= ~PCI_COMMAND_INVALIDATE;
		pci_write_config_word(pci_dev, PCI_COMMAND, cmd);
	}
	#endif
	#endif
}
