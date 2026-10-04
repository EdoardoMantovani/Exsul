int pcie_capability_clear_and_set_word_unlocked(struct pci_dev *dev, int pos, u16 clear, u16 set){
	int ret = 0;
	u16 val = 0;

	ret = pcie_capability_read_word(dev, pos, &val);
	if (ret){
		return ret;
	}
	val &= ~clear;
	val |= set;
	return pcie_capability_write_word(dev, pos, val);
}
