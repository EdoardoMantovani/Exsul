u8 pci_find_next_ht_capability(struct pci_dev *pci_dev, u8 pos, int ht_cap){
	#ifdef EXSUL_PCI_SUPPORT
	return __pci_find_next_ht_cap(pci_dev, pos + PCI_CAP_LIST_NEXT, ht_cap);
	#else
	return 0;
	#endif
}
