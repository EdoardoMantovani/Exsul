int dma_set_mask(struct device *dev, u64 mask){
	int ret = 0;

	if (!dev->dma_mask){
		ret = -EIO;
		return ret;
	}
	dev->dma_mask = mask;
	return ret;
}
