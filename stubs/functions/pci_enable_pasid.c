int pci_enable_pasid(struct pci_dev *pci_dev, int features){
	int ret = 0;
	int pasid = 0;
	u16 control = 0;
	u16 supported = 0;

	#ifdef EXSUL_PCI_SUPPORT
	pasid = pci_dev->pasid_cap;
	/*
	 * VFs must not implement the PASID Capability, but if a PF
	 * supports PASID, its VFs share the PF PASID configuration.
	 */
	if (pci_dev->is_virtfn == true) {
		if (pci_dev->pasid_enabled == true){
			return ret;
		}
		ret = -EINVAL;
		return ret;
	}

	if (pci_dev->pasid_enabled == true){
		ret = -EBUSY;
		return ret;
	}
	if (pci_dev->eetlp_prefix_max == false && pci_dev->pasid_no_tlp == false){
		ret = -EINVAL;
		return ret;
	}
	if (pasid == 0){
		ret = -EINVAL;
		return ret;
	}
	#if 0
	/* disable for now, it will be handled after the hci patch */
	ret = pci_acs_path_enabled(pci_dev, NULL, PCI_ACS_RR | PCI_ACS_UF);
	if(ret == 0){
		ret = -EINVAL;
		return ret;
	}
	#endif
	pci_read_config_word(pci_dev, pasid + PCI_PASID_CAP, &supported);
	supported &= PCI_PASID_CAP_EXEC | PCI_PASID_CAP_PRIV;

	/* User wants to enable anything unsupported? */
	if ((supported & features) != features){
		ret = -EINVAL;
		return ret;
	}
	control = PCI_PASID_CTRL_ENABLE | features;
	pci_dev->pasid_features = features;
	pci_write_config_word(pci_dev, pasid + PCI_PASID_CTRL, control);
	pci_dev->pasid_enabled = 1;
	#endif
	return ret;
}
