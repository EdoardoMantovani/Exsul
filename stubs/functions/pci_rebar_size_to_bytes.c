resource_size_t pci_rebar_size_to_bytes(int size){
	#ifdef EXSUL_PCI_SUPPORT
	return 1ULL << (size + ilog2(PCI_REBAR_MIN_SIZE));
	#else
	return 0;
	#endif
}
