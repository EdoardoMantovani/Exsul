void dev_addr_mod(void *dev, unsigned int offset, const void *addr, size_t len){
	int free_slot = 0;
	u8 *dev_addr = NULL;
	struct net_device *netdev = NULL;

	netdev = (struct net_device *)dev;

	if(netdev == NULL || addr == NULL){
		goto end;
	}

	if(offset >= MAX_ADDR_LEN){
		goto end;
	}
	if(len > MAX_ADDR_LEN - offset){
		len = MAX_ADDR_LEN - offset;
	}

	free_slot = -1;
	for(int i = 0; i < 16; i++){
		if(netdev_addr_slots[i].dev == netdev){
			dev_addr = netdev_addr_slots[i].addr;
			goto setup_dev_addr;
		}
		if(netdev_addr_slots[i].dev == NULL && free_slot < 0){
			free_slot = i;
                }
	}

	if(free_slot < 0){
		free_slot = 0;
	}

	netdev_addr_slots[free_slot].dev = dev;
	memset(netdev_addr_slots[free_slot].addr, 0, MAX_ADDR_LEN);
	dev_addr = netdev_addr_slots[free_slot].addr;

	setup_dev_addr:
	if(dev_addr == NULL){
		goto end;
	}

	memcpy(dev_addr + offset, addr, len);
	netdev->dev_addr = dev_addr;
	end:
}
