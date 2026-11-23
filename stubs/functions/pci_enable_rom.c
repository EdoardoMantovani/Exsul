int pci_enable_rom(struct pci_dev *pci_dev){
	int ret = 0;
	u32 rom_addr = 0;
	struct resource *res = NULL;
	#ifdef EXSUL_PCI_SUPPORT
	struct pci_bus_region region = {};

	res = &pci_dev->resource[PCI_ROM_RESOURCE];
	if (!res->flags){
		ret = -1;
		return ret;
	}
	/* Nothing to enable if we're using a shadow copy in RAM */
	if (res->flags & IORESOURCE_ROM_SHADOW){
		return ret;
	}
	/*
	 * Ideally pci_update_resource() would update the ROM BAR address,
	 * and we would only set the enable bit here.  But apparently some
	 * devices have buggy ROM BARs that read as zero when disabled.
	 */
	pcibios_resource_to_bus(pci_dev->bus, &region, res);
	pci_read_config_dword(pci_dev, pci_dev->rom_base_reg, &rom_addr);
	rom_addr &= ~PCI_ROM_ADDRESS_MASK;
	rom_addr |= region.start | PCI_ROM_ADDRESS_ENABLE;
	pci_write_config_dword(pci_dev, pci_dev->rom_base_reg, rom_addr);
	#endif
	return ret;
}
