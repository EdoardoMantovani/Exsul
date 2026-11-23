void pci_intx(struct pci_dev *pci_dev, int enable){
	u16 new = 0;
	u16 pci_command = 0;

	#ifdef EXSUL_PCI_SUPPORT
	pci_read_config_word(pci_dev, PCI_COMMAND, &pci_command);
	if (enable == true){
		new = pci_command & ~PCI_COMMAND_INTX_DISABLE;
	}else{
		new = pci_command | PCI_COMMAND_INTX_DISABLE;
	}
	if (new == pci_command){
		return;
	}
	pci_write_config_word(pci_dev, PCI_COMMAND, new);
	#endif
}
