void __udelay(unsigned long usecs){
	__const_udelay(usecs * 0x10C7UL); /* 2**32 / 1000000 (rounded up) */
}

