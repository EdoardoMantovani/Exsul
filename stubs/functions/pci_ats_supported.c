bool pci_ats_supported(struct pci_dev *pci_dev){
	#ifdef EXSUL_PCI_SUPPORT
	if (!pci_dev->ats_cap || pci_dev->untrusted){
		return false;
	}
	if (pci_dev->is_virtfn){
		return pci_ats_supported(pci_physfn(pci_dev));
	}
	#endif
	return true;
}
