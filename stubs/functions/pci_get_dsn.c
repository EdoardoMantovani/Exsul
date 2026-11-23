u64 pci_get_dsn(struct pci_dev *pci_dev){
	int ret = 0;
	int pos = 0;
	u64 dsn = 0;
	u32 dword = 0;

	#ifdef EXSUL_PCI_SUPPORT
	pos = pci_find_ext_capability(pci_dev, PCI_EXT_CAP_ID_DSN);
	if (pos == 0){
		return ret;
	}
	/*
	 * The Device Serial Number is two dwords offset 4 bytes from the
	 * capability position. The specification says that the first dword is
	 * the lower half, and the second dword is the upper half.
	 */
	pos += 4;
	pci_read_config_dword(pci_dev, pos, &dword);
	dsn = (u64)dword;
	pci_read_config_dword(pci_dev, pos + 4, &dword);
	dsn |= ((u64)dword) << 32;
	ret = dsn;
	#endif
	return ret;
}
