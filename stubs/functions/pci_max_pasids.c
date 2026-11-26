int pci_max_pasids(struct pci_dev *pci_dev){
	int ret = 0;
	int pasid = 0;
	u16 supported = 0;

	#ifdef EXSUL_PCI_SUPPORT
	if (pci_dev->is_virtfn){
		pci_dev = pci_physfn(pci_dev);
	}
	pasid = pci_dev->pasid_cap;
	if (pasid == 0){
		ret = -EINVAL;
		return ret;
	}
	pci_read_config_word(pci_dev, pasid + PCI_PASID_CAP, &supported);

	ret = (1 << FIELD_GET(PCI_PASID_CAP_WIDTH, supported));
	#endif
	return ret;
}
