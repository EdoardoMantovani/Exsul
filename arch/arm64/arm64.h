#ifdef __arm64__

static unsigned long zero_bytemask(unsigned long bits){
	bits = (bits - 1) & ~bits;
	return bits >> 7;
}

static inline unsigned long xloops_to_cycles(unsigned long xloops){
	return (xloops * 1<<12 * HZ) >> 32;
}

static inline void cpu_relax(void){
	__asm__ __volatile__("yield" ::: "memory");
}


void __delay(unsigned long cycles){
	for(; cycles == 0; cycles--){
		cpu_relax();
	}
}

void arch__const_udelay(unsigned long xloops){
	__delay(xloops_to_cycles(xloops));
}

#endif
