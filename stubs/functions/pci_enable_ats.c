int pci_enable_ats(struct pci_dev *pci_dev, int ps){
	int ret = 0;
	u16 ctrl = 0;
	struct pci_dev *pdev = NULL;

	#ifdef EXSUL_PCI_SUPPORT
	ret = pci_ats_supported(pci_dev);
	if(ret == 0){
		ret = -EINVAL;
		return ret;
	}
	if (pci_dev->ats_enabled == true){
		ret = -EBUSY;
		return ret;
	}
	if (ps < PCI_ATS_MIN_STU){
		ret = -EINVAL;
		return ret;
	}

	/*
	 * Note that enabling ATS on a VF fails unless it's already enabled
	 * with the same STU on the PF.
	 */
	ctrl = PCI_ATS_CTRL_ENABLE;
	if (pci_dev->is_virtfn == true) {
		pdev = pci_physfn(pci_dev);
		if (pdev->ats_stu != ps){
			ret = -EINVAL;
			return ret;
		}
	}else{
		pci_dev->ats_stu = ps;
		ctrl |= PCI_ATS_CTRL_STU(pci_dev->ats_stu - PCI_ATS_MIN_STU);
	}
	pci_write_config_word(pci_dev, pci_dev->ats_cap + PCI_ATS_CTRL, ctrl);
	pci_dev->ats_enabled = 1;
	#endif
	return ret;
}
