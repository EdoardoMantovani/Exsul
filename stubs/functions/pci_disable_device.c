void pci_disable_device(struct pci_dev *pci_dev){
	int ret = 0;
	u16 command = 0;

	#ifdef EXSUL_PCI_SUPPORT
	ret = pci_read_config_word(pci_dev, PCI_COMMAND, &command);
	if(ret == 0){
		command &= ~(PCI_COMMAND_IO | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER);
		pci_write_config_word(pci_dev, PCI_COMMAND, command);
	}
	#endif
}
