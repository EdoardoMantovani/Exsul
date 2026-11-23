int pci_rebar_bytes_to_size(u64 bytes){
	int ret = 0;
	int rebar_minsize = 0;

	#ifdef EXSUL_PCI_SUPPORT
	rebar_minsize = ilog2(PCI_REBAR_MIN_SIZE);
	bytes = roundup_pow_of_two(bytes);
	ret = max(ilog2(bytes), rebar_minsize) - rebar_minsize;
	#endif
	return ret;
}
