void pci_pme_active(struct pci_dev *pci_dev, bool enable){

	#ifdef EXSUL_PCI_SUPPORT
	__pci_pme_active(pci_dev, enable);
	/*
	 * PCI (as opposed to PCIe) PME requires that the device have
	 * its PME# line hooked up correctly. Not all hardware vendors
	 * do this, so the PME never gets delivered and the device
	 * remains asleep. The easiest way around this is to
	 * periodically walk the list of suspended devices and check
	 * whether any have their PME flag set. The assumption is that
	 * we'll wake up often enough anyway that this won't be a huge
	 * hit, and the power savings from the devices will still be a
	 * win.
	 *
	 * Although PCIe uses in-band PME message instead of PME# line
	 * to report PME, PME does not work for some PCIe devices in
	 * reality.  For example, there are devices that set their PME
	 * status bits, but don't really bother to send a PME message;
	 * there are PCI Express Root Ports that don't bother to
	 * trigger interrupts when they receive PME messages from the
	 * devices below.  So PME poll is used for PCIe devices too.
	 */

	/* disable this part for now, need to work more on the global pci_pme variable */
	#if 0
	if (pci_dev->pme_poll == true) {
		struct pci_pme_device *pme_dev;
		if (enable == true) {
			pme_dev = calloc(1, sizeof(struct pci_pme_device));
			if (pme_dev == NULL) {
				return;
			}
			pme_dev->dev = pci_dev;
			list_add(&pme_dev->list, &pci_pme_list);
			if (list_is_singular(&pci_pme_list)){
				queue_delayed_work(system_freezable_wq,
						   &pci_pme_work,
						   msecs_to_jiffies(PME_TIMEOUT));
			}
		}else{
			list_for_each_entry(pme_dev, &pci_pme_list, list) {
				if (pme_dev->dev == pci_dev) {
					list_del(&pme_dev->list);
					kfree(pme_dev);
					break;
				}
			}
		}
	}
	#endif
	#endif
}
