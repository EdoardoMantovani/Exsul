bool pci_pme_capable(struct pci_dev *pci_dev, int state){
	#ifdef EXSUL_PCI_SUPPORT
	if (!pci_dev->pm_cap){
		return false;
	}
	return !!(pci_dev->pme_support & (1 << state));
	#else
	return false;
	#endif
}
