void pci_disable_pri(struct pci_dev *pci_dev){
	int pri = 0;
	u16 control = 0;

	#ifdef EXSUL_PCI_SUPPORT
	pri = pci_dev->pri_cap;
	/* VFs share the PF PRI */
	if (pci_dev->is_virtfn == true){
		return;
	}
	if (pci_dev->pri_enabled == false){
		return;
	}
	if (pri == 0){
		return;
	}
	pci_read_config_word(pci_dev, pri + PCI_PRI_CTRL, &control);
	control &= ~PCI_PRI_CTRL_ENABLE;
	pci_write_config_word(pci_dev, pri + PCI_PRI_CTRL, control);

	pci_dev->pri_enabled = 0;
	#endif
}
