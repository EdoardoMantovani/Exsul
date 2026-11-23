int pcie_capability_clear_and_set_word_locked(struct pci_dev *pci_dev, int pos, u16 clear, u16 set){
	int ret = 0;

	#ifdef EXSUL_PCI_SUPPORT
	/* using spinlocks here isn't necessary */
	ret = pcie_capability_clear_and_set_word_unlocked(pci_dev, pos, clear, set);
	#endif
	return ret;
}
