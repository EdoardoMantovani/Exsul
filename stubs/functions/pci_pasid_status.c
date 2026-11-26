int pci_pasid_status(struct pci_dev *pci_dev){
	int ret = 0;
	int pasid = 0;
	u16 ctrl = 0;

	#ifdef EXSUL_PCI_SUPPORT
	if (pci_dev->is_virtfn == true){
		pci_dev = pci_physfn(pci_dev);
	}
	pasid = pci_dev->pasid_cap;
	if (pasid == 0){
		ret = -EINVAL;
		return ret;
	}
	pci_read_config_word(pci_dev, pasid + PCI_PASID_CTRL, &ctrl);
	ctrl &= PCI_PASID_CTRL_ENABLE | PCI_PASID_CTRL_EXEC | PCI_PASID_CTRL_PRIV;
	#endif
	return ctrl;
}
