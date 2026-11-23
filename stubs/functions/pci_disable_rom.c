void pci_disable_rom(struct pci_dev *pci_dev){
	u32 rom_addr = 0;
	struct resource *res = NULL;

	#ifdef EXSUL_PCI_SUPPORT
	res = &pci_dev->resource[PCI_ROM_RESOURCE];
	if (res->flags & IORESOURCE_ROM_SHADOW){
		return;
	}
	pci_read_config_dword(pci_dev, pci_dev->rom_base_reg, &rom_addr);
	rom_addr &= ~PCI_ROM_ADDRESS_ENABLE;
	pci_write_config_dword(pci_dev, pci_dev->rom_base_reg, rom_addr);
	#endif
}
