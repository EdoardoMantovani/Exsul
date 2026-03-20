We load the kernel module in memory and we can run it, but what if instead of loading the module from an external platform, we can also
create a converted elf/pe executable for being used in userspace only context ?
It isn't such a complex extension, think that that once you can execute a kernel module in userspace, you can also dump the entire address space, the challenge
arises in how put the dump in the right form in order to have an executable which can be used as a standalone driver.
The final elf file should be something like:
1) minimal stub which does the usb device plug detection by reading the entries of 'usb_device_id'
2) call the probe function of the usb device

