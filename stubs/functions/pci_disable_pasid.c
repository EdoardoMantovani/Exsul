void pci_disable_pasid(struct pci_dev *pci_dev){
	int pasid = 0;
	u16 control = 0;

	#ifdef EXSUL_PCI_SUPPORT
	pasid = pci_dev->pasid_cap;
	/* VFs share the PF PASID configuration */
	if (pci_dev->is_virtfn == true){
		return;
	}
	if (pci_dev->pasid_enabled == false){
		return;
	}
	if (pasid == 0){
		return;
	}
	pci_write_config_word(pci_dev, pasid + PCI_PASID_CTRL, control);
	pci_dev->pasid_enabled = 0;
	#endif
}
