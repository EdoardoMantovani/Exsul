/* Made by Edoardo Mantovani, 2026
 * this header contains pci-related stuff (not enabled by default) */
#ifdef EXSUL_PCI_SUPPORT
#include <linux/vfio.h>

#define PCI_ANY_ID (~0)
/*
 * DMA_ATTR_MMIO - Indicates memory-mapped I/O (MMIO) region for DMA mapping
 *
 * This attribute indicates the physical address is not normal system
 * memory. It may not be used with kmap*()/phys_to_virt()/phys_to_page()
 * functions, it may not be cacheable, and access using CPU load/store
 * instructions may not be allowed.
 *
 * Usually this will be used to describe MMIO addresses, or other non-cacheable
 * register addresses. When DMA mapping this sort of address we call
 * the operation Peer to Peer as a one device is DMA'ing to another device.
 * For PCI devices the p2pdma APIs must be used to determine if DMA_ATTR_MMIO
 * is appropriate.
 *
 * For architectures that require cache flushing for DMA coherence
 * DMA_ATTR_MMIO will not perform any cache flushing. The address
 * provided must never be mapped cacheable into the CPU.
 */
#define DMA_ATTR_MMIO		(1UL << 10)

/*
 * DMA_ATTR_DEBUGGING_IGNORE_CACHELINES: Indicates the CPU cache line can be
 * overlapped. All mappings sharing a cacheline must have this attribute for
 * this to be considered safe.
 */
#define DMA_ATTR_DEBUGGING_IGNORE_CACHELINES	(1UL << 11)

/*
 * DMA_ATTR_REQUIRE_COHERENT: Indicates that DMA coherency is required.
 * All mappings that carry this attribute can't work with SWIOTLB and cache
 * flushing.
 */
#define DMA_ATTR_REQUIRE_COHERENT	(1UL << 12)
/*
 * DMA_ATTR_CC_SHARED: Indicates the DMA mapping is shared (decrypted) for
 * confidential computing guests. For normal system memory the caller must have
 * called set_memory_decrypted(), and pgprot_decrypted must be used when
 * creating CPU PTEs for the mapping. The same shared semantic may be passed
 * to the vIOMMU when it sets up the IOPTE. For MMIO use together with
 * DMA_ATTR_MMIO to indicate shared MMIO. Unless DMA_ATTR_MMIO is provided
 * a struct page is required.
 */
#define DMA_ATTR_CC_SHARED	(1UL << 13)

/*
 * __DMA_ATTR_ALLOC_CC_SHARED: Internal DMA-mapping attribute used by
 * allocation paths that create shared (decrypted) backing pages for
 * confidential computing guests. Drivers must not pass this attribute to
 * dma_alloc_attrs().
 */
#define __DMA_ATTR_ALLOC_CC_SHARED	(1UL << 14)

/*
 * A dma_addr_t can hold any valid DMA or bus address for the platform.  It can
 * be given to a device to use as a DMA source or target.  It is specific to a
 * given device and there may be a translation between the CPU physical address
 * space and the bus address space.
 *
 * DMA_MAPPING_ERROR is the magic error code if a mapping failed.  It should not
 * be used directly in drivers, but checked for using dma_mapping_error()
 * instead.
 */
#define DMA_MAPPING_ERROR		(~(dma_addr_t)0)

/* definitions from the kernel side */
#define PCI_IRQ_INTX            (1 << 0) /* Allow INTx interrupts */
#define PCI_IRQ_MSI             (1 << 1) /* Allow MSI interrupts */
#define PCI_IRQ_MSIX            (1 << 2) /* Allow MSI-X interrupts */
#define PCI_IRQ_AFFINITY        (1 << 3) /* Auto-assign affinity */

#define PCI_IRQ_LEGACY          PCI_IRQ_INTX /* Deprecated! Use PCI_IRQ_INTX */

/* Error values that may be returned by PCI functions */
#define PCIBIOS_SUCCESSFUL		0x00
#define PCIBIOS_FUNC_NOT_SUPPORTED	0x81
#define PCIBIOS_BAD_VENDOR_ID		0x83
#define PCIBIOS_DEVICE_NOT_FOUND	0x86
#define PCIBIOS_BAD_REGISTER_NUMBER	0x87
#define PCIBIOS_SET_FAILED		0x88
#define PCIBIOS_BUFFER_TOO_SMALL	0x89

/*
 * Wrappers for all PCI configuration access functions.  They just check
 * alignment, do locking and call the low-level functions pointed to
 * by pci_dev->ops.
 */
#define PCI_byte_BAD 0
#define PCI_word_BAD (pos & 1)
#define PCI_dword_BAD (pos & 3)

/*
 * Reading from a device that doesn't respond typically returns ~0.  A
 * successful read from a device may also return ~0, so you need additional
 * information to reliably identify errors.
 */
#define PCI_ERROR_RESPONSE		(~0ULL)
#define PCI_SET_ERROR_RESPONSE(val)	(*(val) = ((typeof(*(val))) PCI_ERROR_RESPONSE))
#define PCI_POSSIBLE_ERROR(val)		((val) == ((typeof(val)) PCI_ERROR_RESPONSE))

#define __bf_shf __builtin_ctzll
#define READ_PCI_VALUE(_name, _value)                                      	\
	do {                                                                 	\
		snprintf(path, sizeof(path),					\
			"/sys/bus/pci/devices/%s/" _name, bdf);                	\
		ret = read_hex_file(path, _value);	                        \
		if(ret){                                                       	\
			return ret;                                             \
		}                                                              	\
	} while(0)

/* Standard Capability finder */
/*
 * PCI_FIND_NEXT_CAP - Find a PCI standard capability
 * @read_cfg: Function pointer for reading PCI config space
 * @start: Starting position to begin search
 * @cap: Capability ID to find
 * @prev_ptr: Pointer to store position of preceding capability (optional)
 * @args: Arguments to pass to read_cfg function
 *
 * Search the capability list in PCI config space to find @cap. If
 * found, update *prev_ptr with the position of the preceding capability
 * (if prev_ptr != NULL)
 * Implements TTL (time-to-live) protection against infinite loops.
 *
 * Return: Position of the capability if found, 0 otherwise.
 */
#define PCI_FIND_NEXT_CAP(read_cfg, start, cap, prev_ptr, args...)	\
({									\
	int __ttl = PCI_FIND_CAP_TTL;					\
	u8 __id,  __found_pos = 0;					\
	u8 __prev_pos = (start);					\
	u8 __pos = (start);						\
	u16 __ent;							\
									\
	read_cfg##_byte(args, __pos, &__pos);				\
									\
	while (__ttl--) {						\
		if (__pos < PCI_STD_HEADER_SIZEOF)			\
			break;						\
									\
		__pos = ALIGN_DOWN(__pos, 4);				\
		read_cfg##_word(args, __pos, &__ent);			\
									\
		__id = FIELD_GET(PCI_CAP_ID_MASK, __ent);		\
		if (__id == 0xff)					\
			break;						\
									\
		if (__id == (cap)) {					\
			__found_pos = __pos;				\
			if (prev_ptr != NULL)				\
				*(u8 *)prev_ptr = __prev_pos;		\
			break;						\
		}							\
									\
		__prev_pos = __pos;					\
		__pos = FIELD_GET(PCI_CAP_LIST_NEXT_MASK, __ent);	\
	}								\
	__found_pos;							\
})

/* Extended Capability finder */
/*
 * PCI_FIND_NEXT_EXT_CAP - Find a PCI extended capability
 * @read_cfg: Function pointer for reading PCI config space
 * @start: Starting position to begin search (0 for initial search)
 * @cap: Extended capability ID to find
 * @prev_ptr: Pointer to store position of preceding capability (optional)
 * @args: Arguments to pass to read_cfg function
 *
 * Search the extended capability list in PCI config space to find @cap.
 * If found, update *prev_ptr with the position of the preceding capability
 * (if prev_ptr != NULL)
 * Implements TTL protection against infinite loops using a calculated
 * maximum search count.
 *
 * Return: Position of the capability if found, 0 otherwise.
 */
#define PCI_FIND_NEXT_EXT_CAP(read_cfg, start, cap, prev_ptr, args...)	\
({									\
	u16 __pos = (start) ?: PCI_CFG_SPACE_SIZE;			\
	u16 __found_pos = 0;						\
	u16 __prev_pos;							\
	int __ttl, __ret;						\
	u32 __header;							\
									\
	__prev_pos = __pos;						\
	__ttl = (PCI_CFG_SPACE_EXP_SIZE - PCI_CFG_SPACE_SIZE) / 8;	\
	while (__ttl-- > 0 && __pos >= PCI_CFG_SPACE_SIZE) {		\
		__ret = read_cfg##_dword(args, __pos, &__header);	\
		if (__ret != PCIBIOS_SUCCESSFUL)			\
			break;						\
									\
		if (__header == 0)					\
			break;						\
									\
		if (PCI_EXT_CAP_ID(__header) == (cap) && __pos != start) {\
			__found_pos = __pos;				\
			if (prev_ptr != NULL)				\
				*(u16 *)prev_ptr = __prev_pos;		\
			break;						\
		}							\
									\
		__prev_pos = __pos;					\
		__pos = PCI_EXT_CAP_NEXT(__header);			\
	}								\
	__found_pos;							\
})

#define PCI_OP_READ(size, type, len) \
static int pci_bus_read_config_##size(struct pci_bus *bus, unsigned int devfn, int pos, type *value){	\
	int ret = 0;											\
	u32 data = 0;											\
	struct pci_ops *pci_ops = NULL;									\
	int (*read)(struct pci_bus *bus, unsigned int devfn, int where, int size, u32 *val) = NULL;	\
													\
	if (PCI_##size##_BAD){										\
		return PCIBIOS_BAD_REGISTER_NUMBER;							\
	}												\
	pci_ops = bus->ops;										\
	read = pci_ops->read;										\
	ret = read(bus, devfn, pos, len, &data);							\
	if (ret){											\
		PCI_SET_ERROR_RESPONSE(value);								\
	}else{												\
		*value = (type)data;									\
	}												\
	return ret;											\
}

#define PCI_OP_WRITE(size, type, len) \
static int pci_bus_write_config_##size(struct pci_bus *bus, unsigned int devfn, int pos, type value){	\
	int ret = 0;											\
	struct pci_ops *pci_ops = NULL;									\
	int (*write)(struct pci_bus *bus, unsigned int devfn, int where, int size, u32 val) = NULL;	\
													\
	if (PCI_##size##_BAD){										\
		return PCIBIOS_BAD_REGISTER_NUMBER;							\
	}												\
	pci_ops = bus->ops;										\
	write = pci_ops->write;										\
	ret = write(bus, devfn, pos, len, value);							\
	return ret;											\
}

#define PCI_CAP_ID_MASK		0x00ff	/* Capability ID mask */
#define PCI_CAP_LIST_NEXT_MASK	0xff00	/* Next Capability Pointer mask */

/* port here the header 'pci_regs.h' */
#ifndef LINUX_PCI_REGS_H
#define LINUX_PCI_REGS_H

/*
 * Conventional PCI and PCI-X Mode 1 devices have 256 bytes of
 * configuration space.  PCI-X Mode 2 and PCIe devices have 4096 bytes of
 * configuration space.
 */
#define PCI_CFG_SPACE_SIZE	256
#define PCI_CFG_SPACE_EXP_SIZE	4096

/*
 * Under PCI, each device has 256 bytes of configuration address space,
 * of which the first 64 bytes are standardized as follows:
 */
#define PCI_STD_HEADER_SIZEOF	64
#define PCI_STD_NUM_BARS	6	/* Number of standard BARs */
#define PCI_VENDOR_ID		0x00	/* 16 bits */
#define PCI_DEVICE_ID		0x02	/* 16 bits */
#define PCI_COMMAND		0x04	/* 16 bits */
#define  PCI_COMMAND_IO		0x1	/* Enable response in I/O space */
#define  PCI_COMMAND_MEMORY	0x2	/* Enable response in Memory space */
#define  PCI_COMMAND_MASTER	0x4	/* Enable bus mastering */
#define  PCI_COMMAND_SPECIAL	0x8	/* Enable response to special cycles */
#define  PCI_COMMAND_INVALIDATE	0x10	/* Use memory write and invalidate */
#define  PCI_COMMAND_VGA_PALETTE 0x20	/* Enable palette snooping */
#define  PCI_COMMAND_PARITY	0x40	/* Enable parity checking */
#define  PCI_COMMAND_WAIT	0x80	/* Enable address/data stepping */
#define  PCI_COMMAND_SERR	0x100	/* Enable SERR */
#define  PCI_COMMAND_FAST_BACK	0x200	/* Enable back-to-back writes */
#define  PCI_COMMAND_INTX_DISABLE 0x400 /* INTx Emulation Disable */

#define PCI_STATUS		0x06	/* 16 bits */
#define  PCI_STATUS_IMM_READY	0x01	/* Immediate Readiness */
#define  PCI_STATUS_INTERRUPT	0x08	/* Interrupt status */
#define  PCI_STATUS_CAP_LIST	0x10	/* Support Capability List */
#define  PCI_STATUS_66MHZ	0x20	/* Support 66 MHz PCI 2.1 bus */
#define  PCI_STATUS_UDF		0x40	/* Support User Definable Features [obsolete] */
#define  PCI_STATUS_FAST_BACK	0x80	/* Accept fast-back to back */
#define  PCI_STATUS_PARITY	0x100	/* Detected parity error */
#define  PCI_STATUS_DEVSEL_MASK	0x600	/* DEVSEL timing */
#define  PCI_STATUS_DEVSEL_FAST		0x000
#define  PCI_STATUS_DEVSEL_MEDIUM	0x200
#define  PCI_STATUS_DEVSEL_SLOW		0x400
#define  PCI_STATUS_SIG_TARGET_ABORT	0x800 /* Set on target abort */
#define  PCI_STATUS_REC_TARGET_ABORT	0x1000 /* Master ack of " */
#define  PCI_STATUS_REC_MASTER_ABORT	0x2000 /* Set on master abort */
#define  PCI_STATUS_SIG_SYSTEM_ERROR	0x4000 /* Set when we drive SERR */
#define  PCI_STATUS_DETECTED_PARITY	0x8000 /* Set on parity error */

#define PCI_CLASS_REVISION	0x08	/* High 24 bits are class, low 8 revision */
#define PCI_REVISION_ID		0x08	/* Revision ID */
#define PCI_CLASS_PROG		0x09	/* Reg. Level Programming Interface */
#define PCI_CLASS_DEVICE	0x0a	/* Device class */

#define PCI_CACHE_LINE_SIZE	0x0c	/* 8 bits */
#define PCI_LATENCY_TIMER	0x0d	/* 8 bits */
#define PCI_HEADER_TYPE		0x0e	/* 8 bits */
#define  PCI_HEADER_TYPE_MASK		0x7f
#define  PCI_HEADER_TYPE_NORMAL		0
#define  PCI_HEADER_TYPE_BRIDGE		1
#define  PCI_HEADER_TYPE_CARDBUS	2
#define  PCI_HEADER_TYPE_MFD		0x80	/* Multi-Function Device (possible) */

#define PCI_BIST		0x0f	/* 8 bits */
#define  PCI_BIST_CODE_MASK	0x0f	/* Return result */
#define  PCI_BIST_START		0x40	/* 1 to start BIST, 2 secs or less */
#define  PCI_BIST_CAPABLE	0x80	/* 1 if BIST capable */

/*
 * Base addresses specify locations in memory or I/O space.
 * Decoded size can be determined by writing a value of
 * 0xffffffff to the register, and reading it back.  Only
 * 1 bits are decoded.
 */
#define PCI_BASE_ADDRESS_0	0x10	/* 32 bits */
#define PCI_BASE_ADDRESS_1	0x14	/* 32 bits [htype 0,1 only] */
#define PCI_BASE_ADDRESS_2	0x18	/* 32 bits [htype 0 only] */
#define PCI_BASE_ADDRESS_3	0x1c	/* 32 bits */
#define PCI_BASE_ADDRESS_4	0x20	/* 32 bits */
#define PCI_BASE_ADDRESS_5	0x24	/* 32 bits */
#define  PCI_BASE_ADDRESS_SPACE		0x01	/* 0 = memory, 1 = I/O */
#define  PCI_BASE_ADDRESS_SPACE_IO	0x01
#define  PCI_BASE_ADDRESS_SPACE_MEMORY	0x00
#define  PCI_BASE_ADDRESS_MEM_TYPE_MASK	0x06
#define  PCI_BASE_ADDRESS_MEM_TYPE_32	0x00	/* 32 bit address */
#define  PCI_BASE_ADDRESS_MEM_TYPE_1M	0x02	/* Below 1M [obsolete] */
#define  PCI_BASE_ADDRESS_MEM_TYPE_64	0x04	/* 64 bit address */
#define  PCI_BASE_ADDRESS_MEM_PREFETCH	0x08	/* prefetchable? */
#define  PCI_BASE_ADDRESS_MEM_MASK	(~0x0fUL)
#define  PCI_BASE_ADDRESS_IO_MASK	(~0x03UL)
/* bit 1 is reserved if address_space = 1 */

/* Header type 0 (normal devices) */
#define PCI_CARDBUS_CIS		0x28
#define PCI_SUBSYSTEM_VENDOR_ID	0x2c
#define PCI_SUBSYSTEM_ID	0x2e
#define PCI_ROM_ADDRESS		0x30	/* Bits 31..11 are address, 10..1 reserved */
#define  PCI_ROM_ADDRESS_ENABLE	0x01
#define PCI_ROM_ADDRESS_MASK	(~0x7ffU)

#define PCI_CAPABILITY_LIST	0x34	/* Offset of first capability list entry */

/* 0x35-0x3b are reserved */
#define PCI_INTERRUPT_LINE	0x3c	/* 8 bits */
#define PCI_INTERRUPT_PIN	0x3d	/* 8 bits */
#define PCI_MIN_GNT		0x3e	/* 8 bits */
#define PCI_MAX_LAT		0x3f	/* 8 bits */

/* Header type 1 (PCI-to-PCI bridges) */
#define PCI_PRIMARY_BUS		0x18	/* Primary bus number */
#define PCI_SECONDARY_BUS	0x19	/* Secondary bus number */
#define PCI_SUBORDINATE_BUS	0x1a	/* Highest bus number behind the bridge */
#define PCI_SEC_LATENCY_TIMER	0x1b	/* Latency timer for secondary interface */
#define PCI_IO_BASE		0x1c	/* I/O range behind the bridge */
#define PCI_IO_LIMIT		0x1d
#define  PCI_IO_RANGE_TYPE_MASK	0x0fUL	/* I/O bridging type */
#define  PCI_IO_RANGE_TYPE_16	0x00
#define  PCI_IO_RANGE_TYPE_32	0x01
#define  PCI_IO_RANGE_MASK	(~0x0fUL) /* Standard 4K I/O windows */
#define  PCI_IO_1K_RANGE_MASK	(~0x03UL) /* Intel 1K I/O windows */
#define PCI_SEC_STATUS		0x1e	/* Secondary status register, only bit 14 used */
#define PCI_MEMORY_BASE		0x20	/* Memory range behind */
#define PCI_MEMORY_LIMIT	0x22
#define  PCI_MEMORY_RANGE_TYPE_MASK 0x0fUL
#define  PCI_MEMORY_RANGE_MASK	(~0x0fUL)
#define PCI_PREF_MEMORY_BASE	0x24	/* Prefetchable memory range behind */
#define PCI_PREF_MEMORY_LIMIT	0x26
#define  PCI_PREF_RANGE_TYPE_MASK 0x0fUL
#define  PCI_PREF_RANGE_TYPE_32	0x00
#define  PCI_PREF_RANGE_TYPE_64	0x01
#define  PCI_PREF_RANGE_MASK	(~0x0fUL)
#define PCI_PREF_BASE_UPPER32	0x28	/* Upper half of prefetchable memory range */
#define PCI_PREF_LIMIT_UPPER32	0x2c
#define PCI_IO_BASE_UPPER16	0x30	/* Upper half of I/O addresses */
#define PCI_IO_LIMIT_UPPER16	0x32
/* 0x34 same as for htype 0 */
/* 0x35-0x3b is reserved */
#define PCI_ROM_ADDRESS1	0x38	/* Same as PCI_ROM_ADDRESS, but for htype 1 */
/* 0x3c-0x3d are same as for htype 0 */
#define PCI_BRIDGE_CONTROL	0x3e
#define  PCI_BRIDGE_CTL_PARITY	0x01	/* Enable parity detection on secondary interface */
#define  PCI_BRIDGE_CTL_SERR	0x02	/* The same for SERR forwarding */
#define  PCI_BRIDGE_CTL_ISA	0x04	/* Enable ISA mode */
#define  PCI_BRIDGE_CTL_VGA	0x08	/* Forward VGA addresses */
#define  PCI_BRIDGE_CTL_MASTER_ABORT	0x20  /* Report master aborts */
#define  PCI_BRIDGE_CTL_BUS_RESET	0x40	/* Secondary bus reset */
#define  PCI_BRIDGE_CTL_FAST_BACK	0x80	/* Fast Back2Back enabled on secondary interface */

/* Header type 2 (CardBus bridges) */
#define PCI_CB_CAPABILITY_LIST	0x14
/* 0x15 reserved */
#define PCI_CB_SEC_STATUS	0x16	/* Secondary status */
#define PCI_CB_PRIMARY_BUS	0x18	/* PCI bus number */
#define PCI_CB_CARD_BUS		0x19	/* CardBus bus number */
#define PCI_CB_SUBORDINATE_BUS	0x1a	/* Subordinate bus number */
#define PCI_CB_LATENCY_TIMER	0x1b	/* CardBus latency timer */
#define PCI_CB_MEMORY_BASE_0	0x1c
#define PCI_CB_MEMORY_LIMIT_0	0x20
#define PCI_CB_MEMORY_BASE_1	0x24
#define PCI_CB_MEMORY_LIMIT_1	0x28
#define PCI_CB_IO_BASE_0	0x2c
#define PCI_CB_IO_BASE_0_HI	0x2e
#define PCI_CB_IO_LIMIT_0	0x30
#define PCI_CB_IO_LIMIT_0_HI	0x32
#define PCI_CB_IO_BASE_1	0x34
#define PCI_CB_IO_BASE_1_HI	0x36
#define PCI_CB_IO_LIMIT_1	0x38
#define PCI_CB_IO_LIMIT_1_HI	0x3a
#define  PCI_CB_IO_RANGE_MASK	(~0x03UL)
/* 0x3c-0x3d are same as for htype 0 */
#define PCI_CB_BRIDGE_CONTROL	0x3e
#define  PCI_CB_BRIDGE_CTL_PARITY	0x01	/* Similar to standard bridge control register */
#define  PCI_CB_BRIDGE_CTL_SERR		0x02
#define  PCI_CB_BRIDGE_CTL_ISA		0x04
#define  PCI_CB_BRIDGE_CTL_VGA		0x08
#define  PCI_CB_BRIDGE_CTL_MASTER_ABORT	0x20
#define  PCI_CB_BRIDGE_CTL_CB_RESET	0x40	/* CardBus reset */
#define  PCI_CB_BRIDGE_CTL_16BIT_INT	0x80	/* Enable interrupt for 16-bit cards */
#define  PCI_CB_BRIDGE_CTL_PREFETCH_MEM0 0x100	/* Prefetch enable for both memory regions */
#define  PCI_CB_BRIDGE_CTL_PREFETCH_MEM1 0x200
#define  PCI_CB_BRIDGE_CTL_POST_WRITES	0x400
#define PCI_CB_SUBSYSTEM_VENDOR_ID	0x40
#define PCI_CB_SUBSYSTEM_ID		0x42
#define PCI_CB_LEGACY_MODE_BASE		0x44	/* 16-bit PC Card legacy mode base address (ExCa) */
/* 0x48-0x7f reserved */

/* Capability lists */

#define PCI_CAP_LIST_ID		0	/* Capability ID */
#define  PCI_CAP_ID_PM		0x01	/* Power Management */
#define  PCI_CAP_ID_AGP		0x02	/* Accelerated Graphics Port */
#define  PCI_CAP_ID_VPD		0x03	/* Vital Product Data */
#define  PCI_CAP_ID_SLOTID	0x04	/* Slot Identification */
#define  PCI_CAP_ID_MSI		0x05	/* Message Signalled Interrupts */
#define  PCI_CAP_ID_CHSWP	0x06	/* CompactPCI HotSwap */
#define  PCI_CAP_ID_PCIX	0x07	/* PCI-X */
#define  PCI_CAP_ID_HT		0x08	/* HyperTransport */
#define  PCI_CAP_ID_VNDR	0x09	/* Vendor-Specific */
#define  PCI_CAP_ID_DBG		0x0A	/* Debug port */
#define  PCI_CAP_ID_CCRC	0x0B	/* CompactPCI Central Resource Control */
#define  PCI_CAP_ID_SHPC	0x0C	/* PCI Standard Hot-Plug Controller */
#define  PCI_CAP_ID_SSVID	0x0D	/* Bridge subsystem vendor/device ID */
#define  PCI_CAP_ID_AGP3	0x0E	/* AGP Target PCI-PCI bridge */
#define  PCI_CAP_ID_SECDEV	0x0F	/* Secure Device */
#define  PCI_CAP_ID_EXP		0x10	/* PCI Express */
#define  PCI_CAP_ID_MSIX	0x11	/* MSI-X */
#define  PCI_CAP_ID_SATA	0x12	/* SATA Data/Index Conf. */
#define  PCI_CAP_ID_AF		0x13	/* PCI Advanced Features */
#define  PCI_CAP_ID_EA		0x14	/* PCI Enhanced Allocation */
#define  PCI_CAP_ID_MAX		PCI_CAP_ID_EA
#define PCI_CAP_LIST_NEXT	1	/* Next capability in the list */
#define PCI_CAP_FLAGS		2	/* Capability defined flags (16 bits) */
#define PCI_CAP_SIZEOF		4

/* Power Management Registers */

#define PCI_PM_PMC		2	/* PM Capabilities Register */
#define  PCI_PM_CAP_VER_MASK	0x0007	/* Version */
#define  PCI_PM_CAP_PME_CLOCK	0x0008	/* PME clock required */
#define  PCI_PM_CAP_RESERVED    0x0010  /* Reserved field */
#define  PCI_PM_CAP_DSI		0x0020	/* Device specific initialization */
#define  PCI_PM_CAP_AUX_POWER	0x01C0	/* Auxiliary power support mask */
#define  PCI_PM_CAP_D1		0x0200	/* D1 power state support */
#define  PCI_PM_CAP_D2		0x0400	/* D2 power state support */
#define  PCI_PM_CAP_PME		0x0800	/* PME pin supported */
#define  PCI_PM_CAP_PME_MASK	0xF800	/* PME Mask of all supported states */
#define  PCI_PM_CAP_PME_D0	0x0800	/* PME# from D0 */
#define  PCI_PM_CAP_PME_D1	0x1000	/* PME# from D1 */
#define  PCI_PM_CAP_PME_D2	0x2000	/* PME# from D2 */
#define  PCI_PM_CAP_PME_D3hot	0x4000	/* PME# from D3 (hot) */
#define  PCI_PM_CAP_PME_D3cold	0x8000	/* PME# from D3 (cold) */
#define  PCI_PM_CAP_PME_SHIFT	11	/* Start of the PME Mask in PMC */
#define PCI_PM_CTRL		4	/* PM control and status register */
#define  PCI_PM_CTRL_STATE_MASK	0x0003	/* Current power state (D0 to D3) */
#define  PCI_PM_CTRL_NO_SOFT_RESET	0x0008	/* No reset for D3hot->D0 */
#define  PCI_PM_CTRL_PME_ENABLE	0x0100	/* PME pin enable */
#define  PCI_PM_CTRL_DATA_SEL_MASK	0x1e00	/* Data select (??) */
#define  PCI_PM_CTRL_DATA_SCALE_MASK	0x6000	/* Data scale (??) */
#define  PCI_PM_CTRL_PME_STATUS	0x8000	/* PME pin status */
#define PCI_PM_PPB_EXTENSIONS	6	/* PPB support extensions (??) */
#define  PCI_PM_PPB_B2_B3	0x40	/* Stop clock when in D3hot (??) */
#define  PCI_PM_BPCC_ENABLE	0x80	/* Bus power/clock control enable (??) */
#define PCI_PM_DATA_REGISTER	7	/* (??) */
#define PCI_PM_SIZEOF		8

/* AGP registers */

#define PCI_AGP_VERSION		2	/* BCD version number */
#define PCI_AGP_RFU		3	/* Rest of capability flags */
#define PCI_AGP_STATUS		4	/* Status register */
#define  PCI_AGP_STATUS_RQ_MASK	0xff000000	/* Maximum number of requests - 1 */
#define  PCI_AGP_STATUS_SBA	0x0200	/* Sideband addressing supported */
#define  PCI_AGP_STATUS_64BIT	0x0020	/* 64-bit addressing supported */
#define  PCI_AGP_STATUS_FW	0x0010	/* FW transfers supported */
#define  PCI_AGP_STATUS_RATE4	0x0004	/* 4x transfer rate supported */
#define  PCI_AGP_STATUS_RATE2	0x0002	/* 2x transfer rate supported */
#define  PCI_AGP_STATUS_RATE1	0x0001	/* 1x transfer rate supported */
#define PCI_AGP_COMMAND		8	/* Control register */
#define  PCI_AGP_COMMAND_RQ_MASK 0xff000000  /* Master: Maximum number of requests */
#define  PCI_AGP_COMMAND_SBA	0x0200	/* Sideband addressing enabled */
#define  PCI_AGP_COMMAND_AGP	0x0100	/* Allow processing of AGP transactions */
#define  PCI_AGP_COMMAND_64BIT	0x0020	/* Allow processing of 64-bit addresses */
#define  PCI_AGP_COMMAND_FW	0x0010	/* Force FW transfers */
#define  PCI_AGP_COMMAND_RATE4	0x0004	/* Use 4x rate */
#define  PCI_AGP_COMMAND_RATE2	0x0002	/* Use 2x rate */
#define  PCI_AGP_COMMAND_RATE1	0x0001	/* Use 1x rate */
#define PCI_AGP_SIZEOF		12

/* Vital Product Data */

#define PCI_VPD_ADDR		2	/* Address to access (15 bits!) */
#define  PCI_VPD_ADDR_MASK	0x7fff	/* Address mask */
#define  PCI_VPD_ADDR_F		0x8000	/* Write 0, 1 indicates completion */
#define PCI_VPD_DATA		4	/* 32-bits of data returned here */
#define PCI_CAP_VPD_SIZEOF	8

/* Slot Identification */

#define PCI_SID_ESR		2	/* Expansion Slot Register */
#define  PCI_SID_ESR_NSLOTS	0x1f	/* Number of expansion slots available */
#define  PCI_SID_ESR_FIC	0x20	/* First In Chassis Flag */
#define PCI_SID_CHASSIS_NR	3	/* Chassis Number */

/* Message Signaled Interrupt registers */

#define PCI_MSI_FLAGS		0x02	/* Message Control */
#define  PCI_MSI_FLAGS_ENABLE	0x0001	/* MSI feature enabled */
#define  PCI_MSI_FLAGS_QMASK	0x000e	/* Maximum queue size available */
#define  PCI_MSI_FLAGS_QSIZE	0x0070	/* Message queue size configured */
#define  PCI_MSI_FLAGS_64BIT	0x0080	/* 64-bit addresses allowed */
#define  PCI_MSI_FLAGS_MASKBIT	0x0100	/* Per-vector masking capable */
#define PCI_MSI_RFU		3	/* Rest of capability flags */
#define PCI_MSI_ADDRESS_LO	0x04	/* Lower 32 bits */
#define PCI_MSI_ADDRESS_HI	0x08	/* Upper 32 bits (if PCI_MSI_FLAGS_64BIT set) */
#define PCI_MSI_DATA_32		0x08	/* 16 bits of data for 32-bit devices */
#define PCI_MSI_MASK_32		0x0c	/* Mask bits register for 32-bit devices */
#define PCI_MSI_PENDING_32	0x10	/* Pending intrs for 32-bit devices */
#define PCI_MSI_DATA_64		0x0c	/* 16 bits of data for 64-bit devices */
#define PCI_MSI_MASK_64		0x10	/* Mask bits register for 64-bit devices */
#define PCI_MSI_PENDING_64	0x14	/* Pending intrs for 64-bit devices */

/* MSI-X registers (in MSI-X capability) */
#define PCI_MSIX_FLAGS		2	/* Message Control */
#define  PCI_MSIX_FLAGS_QSIZE	0x07FF	/* Table size */
#define  PCI_MSIX_FLAGS_MASKALL	0x4000	/* Mask all vectors for this function */
#define  PCI_MSIX_FLAGS_ENABLE	0x8000	/* MSI-X enable */
#define PCI_MSIX_TABLE		4	/* Table offset */
#define  PCI_MSIX_TABLE_BIR	0x00000007 /* BAR index */
#define  PCI_MSIX_TABLE_OFFSET	0xfffffff8 /* Offset into specified BAR */
#define PCI_MSIX_PBA		8	/* Pending Bit Array offset */
#define  PCI_MSIX_PBA_BIR	0x00000007 /* BAR index */
#define  PCI_MSIX_PBA_OFFSET	0xfffffff8 /* Offset into specified BAR */
#define PCI_MSIX_FLAGS_BIRMASK	PCI_MSIX_PBA_BIR /* deprecated */
#define PCI_CAP_MSIX_SIZEOF	12	/* size of MSIX registers */

/* MSI-X Table entry format (in memory mapped by a BAR) */
#define PCI_MSIX_ENTRY_SIZE		16
#define PCI_MSIX_ENTRY_LOWER_ADDR	0x0  /* Message Address */
#define PCI_MSIX_ENTRY_UPPER_ADDR	0x4  /* Message Upper Address */
#define PCI_MSIX_ENTRY_DATA		0x8  /* Message Data */
#define PCI_MSIX_ENTRY_VECTOR_CTRL	0xc  /* Vector Control */
#define  PCI_MSIX_ENTRY_CTRL_MASKBIT	0x00000001

/* CompactPCI Hotswap Register */

#define PCI_CHSWP_CSR		2	/* Control and Status Register */
#define  PCI_CHSWP_DHA		0x01	/* Device Hiding Arm */
#define  PCI_CHSWP_EIM		0x02	/* ENUM# Signal Mask */
#define  PCI_CHSWP_PIE		0x04	/* Pending Insert or Extract */
#define  PCI_CHSWP_LOO		0x08	/* LED On / Off */
#define  PCI_CHSWP_PI		0x30	/* Programming Interface */
#define  PCI_CHSWP_EXT		0x40	/* ENUM# status - extraction */
#define  PCI_CHSWP_INS		0x80	/* ENUM# status - insertion */

/* PCI Advanced Feature registers */

#define PCI_AF_LENGTH		2
#define PCI_AF_CAP		3
#define  PCI_AF_CAP_TP		0x01
#define  PCI_AF_CAP_FLR		0x02
#define PCI_AF_CTRL		4
#define  PCI_AF_CTRL_FLR	0x01
#define PCI_AF_STATUS		5
#define  PCI_AF_STATUS_TP	0x01
#define PCI_CAP_AF_SIZEOF	6	/* size of AF registers */

/* PCI Enhanced Allocation registers */

#define PCI_EA_NUM_ENT		2	/* Number of Capability Entries */
#define  PCI_EA_NUM_ENT_MASK	0x3f	/* Num Entries Mask */
#define PCI_EA_FIRST_ENT	4	/* First EA Entry in List */
#define PCI_EA_FIRST_ENT_BRIDGE	8	/* First EA Entry for Bridges */
#define  PCI_EA_ES		0x00000007 /* Entry Size */
#define  PCI_EA_BEI		0x000000f0 /* BAR Equivalent Indicator */

/* EA fixed Secondary and Subordinate bus numbers for Bridge */
#define PCI_EA_SEC_BUS_MASK	0xff
#define PCI_EA_SUB_BUS_MASK	0xff00
#define PCI_EA_SUB_BUS_SHIFT	8

/* 0-5 map to BARs 0-5 respectively */
#define   PCI_EA_BEI_BAR0		0
#define   PCI_EA_BEI_BAR5		5
#define   PCI_EA_BEI_BRIDGE		6	/* Resource behind bridge */
#define   PCI_EA_BEI_ENI		7	/* Equivalent Not Indicated */
#define   PCI_EA_BEI_ROM		8	/* Expansion ROM */
/* 9-14 map to VF BARs 0-5 respectively */
#define   PCI_EA_BEI_VF_BAR0		9
#define   PCI_EA_BEI_VF_BAR5		14
#define   PCI_EA_BEI_RESERVED		15	/* Reserved - Treat like ENI */
#define  PCI_EA_PP		0x0000ff00	/* Primary Properties */
#define  PCI_EA_SP		0x00ff0000	/* Secondary Properties */
#define   PCI_EA_P_MEM			0x00	/* Non-Prefetch Memory */
#define   PCI_EA_P_MEM_PREFETCH		0x01	/* Prefetchable Memory */
#define   PCI_EA_P_IO			0x02	/* I/O Space */
#define   PCI_EA_P_VF_MEM_PREFETCH	0x03	/* VF Prefetchable Memory */
#define   PCI_EA_P_VF_MEM		0x04	/* VF Non-Prefetch Memory */
#define   PCI_EA_P_BRIDGE_MEM		0x05	/* Bridge Non-Prefetch Memory */
#define   PCI_EA_P_BRIDGE_MEM_PREFETCH	0x06	/* Bridge Prefetchable Memory */
#define   PCI_EA_P_BRIDGE_IO		0x07	/* Bridge I/O Space */
/* 0x08-0xfc reserved */
#define   PCI_EA_P_MEM_RESERVED		0xfd	/* Reserved Memory */
#define   PCI_EA_P_IO_RESERVED		0xfe	/* Reserved I/O Space */
#define   PCI_EA_P_UNAVAILABLE		0xff	/* Entry Unavailable */
#define  PCI_EA_WRITABLE	0x40000000	/* Writable: 1 = RW, 0 = HwInit */
#define  PCI_EA_ENABLE		0x80000000	/* Enable for this entry */
#define PCI_EA_BASE		4		/* Base Address Offset */
#define PCI_EA_MAX_OFFSET	8		/* MaxOffset (resource length) */
/* bit 0 is reserved */
#define  PCI_EA_IS_64		0x00000002	/* 64-bit field flag */
#define  PCI_EA_FIELD_MASK	0xfffffffc	/* For Base & Max Offset */

/* PCI-X registers (Type 0 (non-bridge) devices) */

#define PCI_X_CMD		2	/* Modes & Features */
#define  PCI_X_CMD_DPERR_E	0x0001	/* Data Parity Error Recovery Enable */
#define  PCI_X_CMD_ERO		0x0002	/* Enable Relaxed Ordering */
#define  PCI_X_CMD_READ_512	0x0000	/* 512 byte maximum read byte count */
#define  PCI_X_CMD_READ_1K	0x0004	/* 1Kbyte maximum read byte count */
#define  PCI_X_CMD_READ_2K	0x0008	/* 2Kbyte maximum read byte count */
#define  PCI_X_CMD_READ_4K	0x000c	/* 4Kbyte maximum read byte count */
#define  PCI_X_CMD_MAX_READ	0x000c	/* Max Memory Read Byte Count */
				/* Max # of outstanding split transactions */
#define  PCI_X_CMD_SPLIT_1	0x0000	/* Max 1 */
#define  PCI_X_CMD_SPLIT_2	0x0010	/* Max 2 */
#define  PCI_X_CMD_SPLIT_3	0x0020	/* Max 3 */
#define  PCI_X_CMD_SPLIT_4	0x0030	/* Max 4 */
#define  PCI_X_CMD_SPLIT_8	0x0040	/* Max 8 */
#define  PCI_X_CMD_SPLIT_12	0x0050	/* Max 12 */
#define  PCI_X_CMD_SPLIT_16	0x0060	/* Max 16 */
#define  PCI_X_CMD_SPLIT_32	0x0070	/* Max 32 */
#define  PCI_X_CMD_MAX_SPLIT	0x0070	/* Max Outstanding Split Transactions */
#define  PCI_X_CMD_VERSION(x)	(((x) >> 12) & 3) /* Version */
#define PCI_X_STATUS		4	/* PCI-X capabilities */
#define  PCI_X_STATUS_DEVFN	0x000000ff	/* A copy of devfn */
#define  PCI_X_STATUS_BUS	0x0000ff00	/* A copy of bus nr */
#define  PCI_X_STATUS_64BIT	0x00010000	/* 64-bit device */
#define  PCI_X_STATUS_133MHZ	0x00020000	/* 133 MHz capable */
#define  PCI_X_STATUS_SPL_DISC	0x00040000	/* Split Completion Discarded */
#define  PCI_X_STATUS_UNX_SPL	0x00080000	/* Unexpected Split Completion */
#define  PCI_X_STATUS_COMPLEX	0x00100000	/* Device Complexity */
#define  PCI_X_STATUS_MAX_READ	0x00600000	/* Designed Max Memory Read Count */
#define  PCI_X_STATUS_MAX_SPLIT	0x03800000	/* Designed Max Outstanding Split Transactions */
#define  PCI_X_STATUS_MAX_CUM	0x1c000000	/* Designed Max Cumulative Read Size */
#define  PCI_X_STATUS_SPL_ERR	0x20000000	/* Rcvd Split Completion Error Msg */
#define  PCI_X_STATUS_266MHZ	0x40000000	/* 266 MHz capable */
#define  PCI_X_STATUS_533MHZ	0x80000000	/* 533 MHz capable */
#define PCI_X_ECC_CSR		8	/* ECC control and status */
#define PCI_CAP_PCIX_SIZEOF_V0	8	/* size of registers for Version 0 */
#define PCI_CAP_PCIX_SIZEOF_V1	24	/* size for Version 1 */
#define PCI_CAP_PCIX_SIZEOF_V2	PCI_CAP_PCIX_SIZEOF_V1	/* Same for v2 */

/* PCI-X registers (Type 1 (bridge) devices) */

#define PCI_X_BRIDGE_SSTATUS	2	/* Secondary Status */
#define  PCI_X_SSTATUS_64BIT	0x0001	/* Secondary AD interface is 64 bits */
#define  PCI_X_SSTATUS_133MHZ	0x0002	/* 133 MHz capable */
#define  PCI_X_SSTATUS_FREQ	0x03c0	/* Secondary Bus Mode and Frequency */
#define  PCI_X_SSTATUS_VERS	0x3000	/* PCI-X Capability Version */
#define  PCI_X_SSTATUS_V1	0x1000	/* Mode 2, not Mode 1 */
#define  PCI_X_SSTATUS_V2	0x2000	/* Mode 1 or Modes 1 and 2 */
#define  PCI_X_SSTATUS_266MHZ	0x4000	/* 266 MHz capable */
#define  PCI_X_SSTATUS_533MHZ	0x8000	/* 533 MHz capable */
#define PCI_X_BRIDGE_STATUS	4	/* Bridge Status */

/* PCI Bridge Subsystem ID registers */

#define PCI_SSVID_VENDOR_ID     4	/* PCI Bridge subsystem vendor ID */
#define PCI_SSVID_DEVICE_ID     6	/* PCI Bridge subsystem device ID */

/* PCI Express capability registers */

#define PCI_EXP_FLAGS		0x02	/* Capabilities register */
#define  PCI_EXP_FLAGS_VERS	0x000f	/* Capability version */
#define  PCI_EXP_FLAGS_TYPE	0x00f0	/* Device/Port type */
#define   PCI_EXP_TYPE_ENDPOINT	   0x0	/* Express Endpoint */
#define   PCI_EXP_TYPE_LEG_END	   0x1	/* Legacy Endpoint */
#define   PCI_EXP_TYPE_ROOT_PORT   0x4	/* Root Port */
#define   PCI_EXP_TYPE_UPSTREAM	   0x5	/* Upstream Port */
#define   PCI_EXP_TYPE_DOWNSTREAM  0x6	/* Downstream Port */
#define   PCI_EXP_TYPE_PCI_BRIDGE  0x7	/* PCIe to PCI/PCI-X Bridge */
#define   PCI_EXP_TYPE_PCIE_BRIDGE 0x8	/* PCI/PCI-X to PCIe Bridge */
#define   PCI_EXP_TYPE_RC_END	   0x9	/* Root Complex Integrated Endpoint */
#define   PCI_EXP_TYPE_RC_EC	   0xa	/* Root Complex Event Collector */
#define  PCI_EXP_FLAGS_SLOT	0x0100	/* Slot implemented */
#define  PCI_EXP_FLAGS_IRQ	0x3e00	/* Interrupt message number */
#define PCI_EXP_DEVCAP		0x04	/* Device capabilities */
#define  PCI_EXP_DEVCAP_PAYLOAD	0x00000007 /* Max_Payload_Size */
#define  PCI_EXP_DEVCAP_PHANTOM	0x00000018 /* Phantom functions */
#define  PCI_EXP_DEVCAP_EXT_TAG	0x00000020 /* Extended tags */
#define  PCI_EXP_DEVCAP_L0S	0x000001c0 /* L0s Acceptable Latency */
#define  PCI_EXP_DEVCAP_L1	0x00000e00 /* L1 Acceptable Latency */
#define  PCI_EXP_DEVCAP_ATN_BUT	0x00001000 /* Attention Button Present */
#define  PCI_EXP_DEVCAP_ATN_IND	0x00002000 /* Attention Indicator Present */
#define  PCI_EXP_DEVCAP_PWR_IND	0x00004000 /* Power Indicator Present */
#define  PCI_EXP_DEVCAP_RBER	0x00008000 /* Role-Based Error Reporting */
#define  PCI_EXP_DEVCAP_PWR_VAL	0x03fc0000 /* Slot Power Limit Value */
#define  PCI_EXP_DEVCAP_PWR_SCL	0x0c000000 /* Slot Power Limit Scale */
#define  PCI_EXP_DEVCAP_FLR     0x10000000 /* Function Level Reset */
#define PCI_EXP_DEVCTL		0x08	/* Device Control */
#define  PCI_EXP_DEVCTL_CERE	0x0001	/* Correctable Error Reporting En. */
#define  PCI_EXP_DEVCTL_NFERE	0x0002	/* Non-Fatal Error Reporting Enable */
#define  PCI_EXP_DEVCTL_FERE	0x0004	/* Fatal Error Reporting Enable */
#define  PCI_EXP_DEVCTL_URRE	0x0008	/* Unsupported Request Reporting En. */
#define  PCI_EXP_DEVCTL_RELAX_EN 0x0010 /* Enable relaxed ordering */
#define  PCI_EXP_DEVCTL_PAYLOAD	0x00e0	/* Max_Payload_Size */
#define  PCI_EXP_DEVCTL_PAYLOAD_128B 0x0000 /* 128 Bytes */
#define  PCI_EXP_DEVCTL_PAYLOAD_256B 0x0020 /* 256 Bytes */
#define  PCI_EXP_DEVCTL_PAYLOAD_512B 0x0040 /* 512 Bytes */
#define  PCI_EXP_DEVCTL_PAYLOAD_1024B 0x0060 /* 1024 Bytes */
#define  PCI_EXP_DEVCTL_PAYLOAD_2048B 0x0080 /* 2048 Bytes */
#define  PCI_EXP_DEVCTL_PAYLOAD_4096B 0x00a0 /* 4096 Bytes */
#define  PCI_EXP_DEVCTL_EXT_TAG	0x0100	/* Extended Tag Field Enable */
#define  PCI_EXP_DEVCTL_PHANTOM	0x0200	/* Phantom Functions Enable */
#define  PCI_EXP_DEVCTL_AUX_PME	0x0400	/* Auxiliary Power PM Enable */
#define  PCI_EXP_DEVCTL_NOSNOOP_EN 0x0800  /* Enable No Snoop */
#define  PCI_EXP_DEVCTL_READRQ	0x7000	/* Max_Read_Request_Size */
#define  PCI_EXP_DEVCTL_READRQ_128B  0x0000 /* 128 Bytes */
#define  PCI_EXP_DEVCTL_READRQ_256B  0x1000 /* 256 Bytes */
#define  PCI_EXP_DEVCTL_READRQ_512B  0x2000 /* 512 Bytes */
#define  PCI_EXP_DEVCTL_READRQ_1024B 0x3000 /* 1024 Bytes */
#define  PCI_EXP_DEVCTL_READRQ_2048B 0x4000 /* 2048 Bytes */
#define  PCI_EXP_DEVCTL_READRQ_4096B 0x5000 /* 4096 Bytes */
#define  PCI_EXP_DEVCTL_BCR_FLR 0x8000  /* Bridge Configuration Retry / FLR */
#define PCI_EXP_DEVSTA		0x0a	/* Device Status */
#define  PCI_EXP_DEVSTA_CED	0x0001	/* Correctable Error Detected */
#define  PCI_EXP_DEVSTA_NFED	0x0002	/* Non-Fatal Error Detected */
#define  PCI_EXP_DEVSTA_FED	0x0004	/* Fatal Error Detected */
#define  PCI_EXP_DEVSTA_URD	0x0008	/* Unsupported Request Detected */
#define  PCI_EXP_DEVSTA_AUXPD	0x0010	/* AUX Power Detected */
#define  PCI_EXP_DEVSTA_TRPND	0x0020	/* Transactions Pending */
#define PCI_CAP_EXP_RC_ENDPOINT_SIZEOF_V1	12	/* v1 endpoints without link end here */
#define PCI_EXP_LNKCAP		0x0c	/* Link Capabilities */
#define  PCI_EXP_LNKCAP_SLS	0x0000000f /* Supported Link Speeds */
#define  PCI_EXP_LNKCAP_SLS_2_5GB 0x00000001 /* LNKCAP2 SLS Vector bit 0 */
#define  PCI_EXP_LNKCAP_SLS_5_0GB 0x00000002 /* LNKCAP2 SLS Vector bit 1 */
#define  PCI_EXP_LNKCAP_SLS_8_0GB 0x00000003 /* LNKCAP2 SLS Vector bit 2 */
#define  PCI_EXP_LNKCAP_SLS_16_0GB 0x00000004 /* LNKCAP2 SLS Vector bit 3 */
#define  PCI_EXP_LNKCAP_SLS_32_0GB 0x00000005 /* LNKCAP2 SLS Vector bit 4 */
#define  PCI_EXP_LNKCAP_SLS_64_0GB 0x00000006 /* LNKCAP2 SLS Vector bit 5 */
#define  PCI_EXP_LNKCAP_MLW	0x000003f0 /* Maximum Link Width */
#define  PCI_EXP_LNKCAP_ASPMS	0x00000c00 /* ASPM Support */
#define  PCI_EXP_LNKCAP_ASPM_L0S 0x00000400 /* ASPM L0s Support */
#define  PCI_EXP_LNKCAP_ASPM_L1  0x00000800 /* ASPM L1 Support */
#define  PCI_EXP_LNKCAP_L0SEL	0x00007000 /* L0s Exit Latency */
#define  PCI_EXP_LNKCAP_L1EL	0x00038000 /* L1 Exit Latency */
#define  PCI_EXP_LNKCAP_CLKPM	0x00040000 /* Clock Power Management */
#define  PCI_EXP_LNKCAP_SDERC	0x00080000 /* Surprise Down Error Reporting Capable */
#define  PCI_EXP_LNKCAP_DLLLARC	0x00100000 /* Data Link Layer Link Active Reporting Capable */
#define  PCI_EXP_LNKCAP_LBNC	0x00200000 /* Link Bandwidth Notification Capability */
#define  PCI_EXP_LNKCAP_PN	0xff000000 /* Port Number */
#define PCI_EXP_LNKCTL		0x10	/* Link Control */
#define  PCI_EXP_LNKCTL_ASPMC	0x0003	/* ASPM Control */
#define  PCI_EXP_LNKCTL_ASPM_L0S 0x0001	/* L0s Enable */
#define  PCI_EXP_LNKCTL_ASPM_L1  0x0002	/* L1 Enable */
#define  PCI_EXP_LNKCTL_RCB	0x0008	/* Read Completion Boundary */
#define  PCI_EXP_LNKCTL_LD	0x0010	/* Link Disable */
#define  PCI_EXP_LNKCTL_RL	0x0020	/* Retrain Link */
#define  PCI_EXP_LNKCTL_CCC	0x0040	/* Common Clock Configuration */
#define  PCI_EXP_LNKCTL_ES	0x0080	/* Extended Synch */
#define  PCI_EXP_LNKCTL_CLKREQ_EN 0x0100 /* Enable clkreq */
#define  PCI_EXP_LNKCTL_HAWD	0x0200	/* Hardware Autonomous Width Disable */
#define  PCI_EXP_LNKCTL_LBMIE	0x0400	/* Link Bandwidth Management Interrupt Enable */
#define  PCI_EXP_LNKCTL_LABIE	0x0800	/* Link Autonomous Bandwidth Interrupt Enable */
#define PCI_EXP_LNKSTA		0x12	/* Link Status */
#define  PCI_EXP_LNKSTA_CLS	0x000f	/* Current Link Speed */
#define  PCI_EXP_LNKSTA_CLS_2_5GB 0x0001 /* Current Link Speed 2.5GT/s */
#define  PCI_EXP_LNKSTA_CLS_5_0GB 0x0002 /* Current Link Speed 5.0GT/s */
#define  PCI_EXP_LNKSTA_CLS_8_0GB 0x0003 /* Current Link Speed 8.0GT/s */
#define  PCI_EXP_LNKSTA_CLS_16_0GB 0x0004 /* Current Link Speed 16.0GT/s */
#define  PCI_EXP_LNKSTA_CLS_32_0GB 0x0005 /* Current Link Speed 32.0GT/s */
#define  PCI_EXP_LNKSTA_CLS_64_0GB 0x0006 /* Current Link Speed 64.0GT/s */
#define  PCI_EXP_LNKSTA_NLW	0x03f0	/* Negotiated Link Width */
#define  PCI_EXP_LNKSTA_NLW_X1	0x0010	/* Current Link Width x1 */
#define  PCI_EXP_LNKSTA_NLW_X2	0x0020	/* Current Link Width x2 */
#define  PCI_EXP_LNKSTA_NLW_X4	0x0040	/* Current Link Width x4 */
#define  PCI_EXP_LNKSTA_NLW_X8	0x0080	/* Current Link Width x8 */
#define  PCI_EXP_LNKSTA_NLW_SHIFT 4	/* start of NLW mask in link status */
#define  PCI_EXP_LNKSTA_LT	0x0800	/* Link Training */
#define  PCI_EXP_LNKSTA_SLC	0x1000	/* Slot Clock Configuration */
#define  PCI_EXP_LNKSTA_DLLLA	0x2000	/* Data Link Layer Link Active */
#define  PCI_EXP_LNKSTA_LBMS	0x4000	/* Link Bandwidth Management Status */
#define  PCI_EXP_LNKSTA_LABS	0x8000	/* Link Autonomous Bandwidth Status */
#define PCI_CAP_EXP_ENDPOINT_SIZEOF_V1	20	/* v1 endpoints with link end here */
#define PCI_EXP_SLTCAP		0x14	/* Slot Capabilities */
#define  PCI_EXP_SLTCAP_ABP	0x00000001 /* Attention Button Present */
#define  PCI_EXP_SLTCAP_PCP	0x00000002 /* Power Controller Present */
#define  PCI_EXP_SLTCAP_MRLSP	0x00000004 /* MRL Sensor Present */
#define  PCI_EXP_SLTCAP_AIP	0x00000008 /* Attention Indicator Present */
#define  PCI_EXP_SLTCAP_PIP	0x00000010 /* Power Indicator Present */
#define  PCI_EXP_SLTCAP_HPS	0x00000020 /* Hot-Plug Surprise */
#define  PCI_EXP_SLTCAP_HPC	0x00000040 /* Hot-Plug Capable */
#define  PCI_EXP_SLTCAP_SPLV	0x00007f80 /* Slot Power Limit Value */
#define  PCI_EXP_SLTCAP_SPLS	0x00018000 /* Slot Power Limit Scale */
#define  PCI_EXP_SLTCAP_EIP	0x00020000 /* Electromechanical Interlock Present */
#define  PCI_EXP_SLTCAP_NCCS	0x00040000 /* No Command Completed Support */
#define  PCI_EXP_SLTCAP_PSN	0xfff80000 /* Physical Slot Number */
#define PCI_EXP_SLTCTL		0x18	/* Slot Control */
#define  PCI_EXP_SLTCTL_ABPE	0x0001	/* Attention Button Pressed Enable */
#define  PCI_EXP_SLTCTL_PFDE	0x0002	/* Power Fault Detected Enable */
#define  PCI_EXP_SLTCTL_MRLSCE	0x0004	/* MRL Sensor Changed Enable */
#define  PCI_EXP_SLTCTL_PDCE	0x0008	/* Presence Detect Changed Enable */
#define  PCI_EXP_SLTCTL_CCIE	0x0010	/* Command Completed Interrupt Enable */
#define  PCI_EXP_SLTCTL_HPIE	0x0020	/* Hot-Plug Interrupt Enable */
#define  PCI_EXP_SLTCTL_AIC	0x00c0	/* Attention Indicator Control */
#define  PCI_EXP_SLTCTL_ATTN_IND_SHIFT 6      /* Attention Indicator shift */
#define  PCI_EXP_SLTCTL_ATTN_IND_ON    0x0040 /* Attention Indicator on */
#define  PCI_EXP_SLTCTL_ATTN_IND_BLINK 0x0080 /* Attention Indicator blinking */
#define  PCI_EXP_SLTCTL_ATTN_IND_OFF   0x00c0 /* Attention Indicator off */
#define  PCI_EXP_SLTCTL_PIC	0x0300	/* Power Indicator Control */
#define  PCI_EXP_SLTCTL_PWR_IND_ON     0x0100 /* Power Indicator on */
#define  PCI_EXP_SLTCTL_PWR_IND_BLINK  0x0200 /* Power Indicator blinking */
#define  PCI_EXP_SLTCTL_PWR_IND_OFF    0x0300 /* Power Indicator off */
#define  PCI_EXP_SLTCTL_PCC	0x0400	/* Power Controller Control */
#define  PCI_EXP_SLTCTL_PWR_ON         0x0000 /* Power On */
#define  PCI_EXP_SLTCTL_PWR_OFF        0x0400 /* Power Off */
#define  PCI_EXP_SLTCTL_EIC	0x0800	/* Electromechanical Interlock Control */
#define  PCI_EXP_SLTCTL_DLLSCE	0x1000	/* Data Link Layer State Changed Enable */
#define  PCI_EXP_SLTCTL_ASPL_DISABLE	0x2000 /* Auto Slot Power Limit Disable */
#define  PCI_EXP_SLTCTL_IBPD_DISABLE	0x4000 /* In-band PD disable */
#define PCI_EXP_SLTSTA		0x1a	/* Slot Status */
#define  PCI_EXP_SLTSTA_ABP	0x0001	/* Attention Button Pressed */
#define  PCI_EXP_SLTSTA_PFD	0x0002	/* Power Fault Detected */
#define  PCI_EXP_SLTSTA_MRLSC	0x0004	/* MRL Sensor Changed */
#define  PCI_EXP_SLTSTA_PDC	0x0008	/* Presence Detect Changed */
#define  PCI_EXP_SLTSTA_CC	0x0010	/* Command Completed */
#define  PCI_EXP_SLTSTA_MRLSS	0x0020	/* MRL Sensor State */
#define  PCI_EXP_SLTSTA_PDS	0x0040	/* Presence Detect State */
#define  PCI_EXP_SLTSTA_EIS	0x0080	/* Electromechanical Interlock Status */
#define  PCI_EXP_SLTSTA_DLLSC	0x0100	/* Data Link Layer State Changed */
#define PCI_EXP_RTCTL		0x1c	/* Root Control */
#define  PCI_EXP_RTCTL_SECEE	0x0001	/* System Error on Correctable Error */
#define  PCI_EXP_RTCTL_SENFEE	0x0002	/* System Error on Non-Fatal Error */
#define  PCI_EXP_RTCTL_SEFEE	0x0004	/* System Error on Fatal Error */
#define  PCI_EXP_RTCTL_PMEIE	0x0008	/* PME Interrupt Enable */
#define  PCI_EXP_RTCTL_CRSSVE	0x0010	/* CRS Software Visibility Enable */
#define PCI_EXP_RTCAP		0x1e	/* Root Capabilities */
#define  PCI_EXP_RTCAP_CRSVIS	0x0001	/* CRS Software Visibility capability */
#define PCI_EXP_RTSTA		0x20	/* Root Status */
#define  PCI_EXP_RTSTA_PME_RQ_ID 0x0000ffff /* PME Requester ID */
#define  PCI_EXP_RTSTA_PME	0x00010000 /* PME status */
#define  PCI_EXP_RTSTA_PENDING	0x00020000 /* PME pending */
/*
 * The Device Capabilities 2, Device Status 2, Device Control 2,
 * Link Capabilities 2, Link Status 2, Link Control 2,
 * Slot Capabilities 2, Slot Status 2, and Slot Control 2 registers
 * are only present on devices with PCIe Capability version 2.
 * Use pcie_capability_read_word() and similar interfaces to use them
 * safely.
 */
#define PCI_EXP_DEVCAP2		0x24	/* Device Capabilities 2 */
#define  PCI_EXP_DEVCAP2_COMP_TMOUT_DIS	0x00000010 /* Completion Timeout Disable supported */
#define  PCI_EXP_DEVCAP2_ARI		0x00000020 /* Alternative Routing-ID */
#define  PCI_EXP_DEVCAP2_ATOMIC_ROUTE	0x00000040 /* Atomic Op routing */
#define  PCI_EXP_DEVCAP2_ATOMIC_COMP32	0x00000080 /* 32b AtomicOp completion */
#define  PCI_EXP_DEVCAP2_ATOMIC_COMP64	0x00000100 /* 64b AtomicOp completion */
#define  PCI_EXP_DEVCAP2_ATOMIC_COMP128	0x00000200 /* 128b AtomicOp completion */
#define  PCI_EXP_DEVCAP2_LTR		0x00000800 /* Latency tolerance reporting */
#define  PCI_EXP_DEVCAP2_OBFF_MASK	0x000c0000 /* OBFF support mechanism */
#define  PCI_EXP_DEVCAP2_OBFF_MSG	0x00040000 /* New message signaling */
#define  PCI_EXP_DEVCAP2_OBFF_WAKE	0x00080000 /* Re-use WAKE# for OBFF */
#define  PCI_EXP_DEVCAP2_EE_PREFIX	0x00200000 /* End-End TLP Prefix */
#define PCI_EXP_DEVCTL2		0x28	/* Device Control 2 */
#define  PCI_EXP_DEVCTL2_COMP_TIMEOUT	0x000f	/* Completion Timeout Value */
#define  PCI_EXP_DEVCTL2_COMP_TMOUT_DIS	0x0010	/* Completion Timeout Disable */
#define  PCI_EXP_DEVCTL2_ARI		0x0020	/* Alternative Routing-ID */
#define  PCI_EXP_DEVCTL2_ATOMIC_REQ	0x0040	/* Set Atomic requests */
#define  PCI_EXP_DEVCTL2_ATOMIC_EGRESS_BLOCK 0x0080 /* Block atomic egress */
#define  PCI_EXP_DEVCTL2_IDO_REQ_EN	0x0100	/* Allow IDO for requests */
#define  PCI_EXP_DEVCTL2_IDO_CMP_EN	0x0200	/* Allow IDO for completions */
#define  PCI_EXP_DEVCTL2_LTR_EN		0x0400	/* Enable LTR mechanism */
#define  PCI_EXP_DEVCTL2_OBFF_MSGA_EN	0x2000	/* Enable OBFF Message type A */
#define  PCI_EXP_DEVCTL2_OBFF_MSGB_EN	0x4000	/* Enable OBFF Message type B */
#define  PCI_EXP_DEVCTL2_OBFF_WAKE_EN	0x6000	/* OBFF using WAKE# signaling */
#define PCI_EXP_DEVSTA2		0x2a	/* Device Status 2 */
#define PCI_CAP_EXP_RC_ENDPOINT_SIZEOF_V2 0x2c	/* end of v2 EPs w/o link */
#define PCI_EXP_LNKCAP2		0x2c	/* Link Capabilities 2 */
#define  PCI_EXP_LNKCAP2_SLS_2_5GB	0x00000002 /* Supported Speed 2.5GT/s */
#define  PCI_EXP_LNKCAP2_SLS_5_0GB	0x00000004 /* Supported Speed 5GT/s */
#define  PCI_EXP_LNKCAP2_SLS_8_0GB	0x00000008 /* Supported Speed 8GT/s */
#define  PCI_EXP_LNKCAP2_SLS_16_0GB	0x00000010 /* Supported Speed 16GT/s */
#define  PCI_EXP_LNKCAP2_SLS_32_0GB	0x00000020 /* Supported Speed 32GT/s */
#define  PCI_EXP_LNKCAP2_SLS_64_0GB	0x00000040 /* Supported Speed 64GT/s */
#define  PCI_EXP_LNKCAP2_CROSSLINK	0x00000100 /* Crosslink supported */
#define PCI_EXP_LNKCTL2		0x30	/* Link Control 2 */
#define  PCI_EXP_LNKCTL2_TLS		0x000f
#define  PCI_EXP_LNKCTL2_TLS_2_5GT	0x0001 /* Supported Speed 2.5GT/s */
#define  PCI_EXP_LNKCTL2_TLS_5_0GT	0x0002 /* Supported Speed 5GT/s */
#define  PCI_EXP_LNKCTL2_TLS_8_0GT	0x0003 /* Supported Speed 8GT/s */
#define  PCI_EXP_LNKCTL2_TLS_16_0GT	0x0004 /* Supported Speed 16GT/s */
#define  PCI_EXP_LNKCTL2_TLS_32_0GT	0x0005 /* Supported Speed 32GT/s */
#define  PCI_EXP_LNKCTL2_TLS_64_0GT	0x0006 /* Supported Speed 64GT/s */
#define  PCI_EXP_LNKCTL2_ENTER_COMP	0x0010 /* Enter Compliance */
#define  PCI_EXP_LNKCTL2_TX_MARGIN	0x0380 /* Transmit Margin */
#define  PCI_EXP_LNKCTL2_HASD		0x0020 /* HW Autonomous Speed Disable */
#define PCI_EXP_LNKSTA2		0x32	/* Link Status 2 */
#define  PCI_EXP_LNKSTA2_FLIT		0x0400 /* Flit Mode Status */
#define PCI_CAP_EXP_ENDPOINT_SIZEOF_V2	0x32	/* end of v2 EPs w/ link */
#define PCI_EXP_SLTCAP2		0x34	/* Slot Capabilities 2 */
#define  PCI_EXP_SLTCAP2_IBPD	0x00000001 /* In-band PD Disable Supported */
#define PCI_EXP_SLTCTL2		0x38	/* Slot Control 2 */
#define PCI_EXP_SLTSTA2		0x3a	/* Slot Status 2 */

/* Extended Capabilities (PCI-X 2.0 and Express) */
#define PCI_EXT_CAP_ID(header)		(header & 0x0000ffff)
#define PCI_EXT_CAP_VER(header)		((header >> 16) & 0xf)
#define PCI_EXT_CAP_NEXT(header)	((header >> 20) & 0xffc)

#define PCI_EXT_CAP_ID_ERR	0x01	/* Advanced Error Reporting */
#define PCI_EXT_CAP_ID_VC	0x02	/* Virtual Channel Capability */
#define PCI_EXT_CAP_ID_DSN	0x03	/* Device Serial Number */
#define PCI_EXT_CAP_ID_PWR	0x04	/* Power Budgeting */
#define PCI_EXT_CAP_ID_RCLD	0x05	/* Root Complex Link Declaration */
#define PCI_EXT_CAP_ID_RCILC	0x06	/* Root Complex Internal Link Control */
#define PCI_EXT_CAP_ID_RCEC	0x07	/* Root Complex Event Collector */
#define PCI_EXT_CAP_ID_MFVC	0x08	/* Multi-Function VC Capability */
#define PCI_EXT_CAP_ID_VC9	0x09	/* same as _VC */
#define PCI_EXT_CAP_ID_RCRB	0x0A	/* Root Complex RB? */
#define PCI_EXT_CAP_ID_VNDR	0x0B	/* Vendor-Specific */
#define PCI_EXT_CAP_ID_CAC	0x0C	/* Config Access - obsolete */
#define PCI_EXT_CAP_ID_ACS	0x0D	/* Access Control Services */
#define PCI_EXT_CAP_ID_ARI	0x0E	/* Alternate Routing ID */
#define PCI_EXT_CAP_ID_ATS	0x0F	/* Address Translation Services */
#define PCI_EXT_CAP_ID_SRIOV	0x10	/* Single Root I/O Virtualization */
#define PCI_EXT_CAP_ID_MRIOV	0x11	/* Multi Root I/O Virtualization */
#define PCI_EXT_CAP_ID_MCAST	0x12	/* Multicast */
#define PCI_EXT_CAP_ID_PRI	0x13	/* Page Request Interface */
#define PCI_EXT_CAP_ID_AMD_XXX	0x14	/* Reserved for AMD */
#define PCI_EXT_CAP_ID_REBAR	0x15	/* Resizable BAR */
#define PCI_EXT_CAP_ID_DPA	0x16	/* Dynamic Power Allocation */
#define PCI_EXT_CAP_ID_TPH	0x17	/* TPH Requester */
#define PCI_EXT_CAP_ID_LTR	0x18	/* Latency Tolerance Reporting */
#define PCI_EXT_CAP_ID_SECPCI	0x19	/* Secondary PCIe Capability */
#define PCI_EXT_CAP_ID_PMUX	0x1A	/* Protocol Multiplexing */
#define PCI_EXT_CAP_ID_PASID	0x1B	/* Process Address Space ID */
#define PCI_EXT_CAP_ID_DPC	0x1D	/* Downstream Port Containment */
#define PCI_EXT_CAP_ID_L1SS	0x1E	/* L1 PM Substates */
#define PCI_EXT_CAP_ID_PTM	0x1F	/* Precision Time Measurement */
#define PCI_EXT_CAP_ID_DVSEC	0x23	/* Designated Vendor-Specific */
#define PCI_EXT_CAP_ID_DLF	0x25	/* Data Link Feature */
#define PCI_EXT_CAP_ID_PL_16GT	0x26	/* Physical Layer 16.0 GT/s */
#define PCI_EXT_CAP_ID_PL_32GT  0x2A    /* Physical Layer 32.0 GT/s */
#define PCI_EXT_CAP_ID_DOE	0x2E	/* Data Object Exchange */
#define PCI_EXT_CAP_ID_MAX	PCI_EXT_CAP_ID_DOE

#define PCI_EXT_CAP_DSN_SIZEOF	12
#define PCI_EXT_CAP_MCAST_ENDPOINT_SIZEOF 40

/* Advanced Error Reporting */
#define PCI_ERR_UNCOR_STATUS	0x04	/* Uncorrectable Error Status */
#define  PCI_ERR_UNC_UND	0x00000001	/* Undefined */
#define  PCI_ERR_UNC_DLP	0x00000010	/* Data Link Protocol */
#define  PCI_ERR_UNC_SURPDN	0x00000020	/* Surprise Down */
#define  PCI_ERR_UNC_POISON_TLP	0x00001000	/* Poisoned TLP */
#define  PCI_ERR_UNC_FCP	0x00002000	/* Flow Control Protocol */
#define  PCI_ERR_UNC_COMP_TIME	0x00004000	/* Completion Timeout */
#define  PCI_ERR_UNC_COMP_ABORT	0x00008000	/* Completer Abort */
#define  PCI_ERR_UNC_UNX_COMP	0x00010000	/* Unexpected Completion */
#define  PCI_ERR_UNC_RX_OVER	0x00020000	/* Receiver Overflow */
#define  PCI_ERR_UNC_MALF_TLP	0x00040000	/* Malformed TLP */
#define  PCI_ERR_UNC_ECRC	0x00080000	/* ECRC Error Status */
#define  PCI_ERR_UNC_UNSUP	0x00100000	/* Unsupported Request */
#define  PCI_ERR_UNC_ACSV	0x00200000	/* ACS Violation */
#define  PCI_ERR_UNC_INTN	0x00400000	/* internal error */
#define  PCI_ERR_UNC_MCBTLP	0x00800000	/* MC blocked TLP */
#define  PCI_ERR_UNC_ATOMEG	0x01000000	/* Atomic egress blocked */
#define  PCI_ERR_UNC_TLPPRE	0x02000000	/* TLP prefix blocked */
#define PCI_ERR_UNCOR_MASK	0x08	/* Uncorrectable Error Mask */
	/* Same bits as above */
#define PCI_ERR_UNCOR_SEVER	0x0c	/* Uncorrectable Error Severity */
	/* Same bits as above */
#define PCI_ERR_COR_STATUS	0x10	/* Correctable Error Status */
#define  PCI_ERR_COR_RCVR	0x00000001	/* Receiver Error Status */
#define  PCI_ERR_COR_BAD_TLP	0x00000040	/* Bad TLP Status */
#define  PCI_ERR_COR_BAD_DLLP	0x00000080	/* Bad DLLP Status */
#define  PCI_ERR_COR_REP_ROLL	0x00000100	/* REPLAY_NUM Rollover */
#define  PCI_ERR_COR_REP_TIMER	0x00001000	/* Replay Timer Timeout */
#define  PCI_ERR_COR_ADV_NFAT	0x00002000	/* Advisory Non-Fatal */
#define  PCI_ERR_COR_INTERNAL	0x00004000	/* Corrected Internal */
#define  PCI_ERR_COR_LOG_OVER	0x00008000	/* Header Log Overflow */
#define PCI_ERR_COR_MASK	0x14	/* Correctable Error Mask */
	/* Same bits as above */
#define PCI_ERR_CAP		0x18	/* Advanced Error Capabilities & Ctrl*/
#define  PCI_ERR_CAP_FEP(x)	((x) & 0x1f)	/* First Error Pointer */
#define  PCI_ERR_CAP_ECRC_GENC	0x00000020	/* ECRC Generation Capable */
#define  PCI_ERR_CAP_ECRC_GENE	0x00000040	/* ECRC Generation Enable */
#define  PCI_ERR_CAP_ECRC_CHKC	0x00000080	/* ECRC Check Capable */
#define  PCI_ERR_CAP_ECRC_CHKE	0x00000100	/* ECRC Check Enable */
#define PCI_ERR_HEADER_LOG	0x1c	/* Header Log Register (16 bytes) */
#define PCI_ERR_ROOT_COMMAND	0x2c	/* Root Error Command */
#define  PCI_ERR_ROOT_CMD_COR_EN	0x00000001 /* Correctable Err Reporting Enable */
#define  PCI_ERR_ROOT_CMD_NONFATAL_EN	0x00000002 /* Non-Fatal Err Reporting Enable */
#define  PCI_ERR_ROOT_CMD_FATAL_EN	0x00000004 /* Fatal Err Reporting Enable */
#define PCI_ERR_ROOT_STATUS	0x30
#define  PCI_ERR_ROOT_COR_RCV		0x00000001 /* ERR_COR Received */
#define  PCI_ERR_ROOT_MULTI_COR_RCV	0x00000002 /* Multiple ERR_COR */
#define  PCI_ERR_ROOT_UNCOR_RCV		0x00000004 /* ERR_FATAL/NONFATAL */
#define  PCI_ERR_ROOT_MULTI_UNCOR_RCV	0x00000008 /* Multiple FATAL/NONFATAL */
#define  PCI_ERR_ROOT_FIRST_FATAL	0x00000010 /* First UNC is Fatal */
#define  PCI_ERR_ROOT_NONFATAL_RCV	0x00000020 /* Non-Fatal Received */
#define  PCI_ERR_ROOT_FATAL_RCV		0x00000040 /* Fatal Received */
#define  PCI_ERR_ROOT_AER_IRQ		0xf8000000 /* Advanced Error Interrupt Message Number */
#define PCI_ERR_ROOT_ERR_SRC	0x34	/* Error Source Identification */

/* Virtual Channel */
#define PCI_VC_PORT_CAP1	0x04
#define  PCI_VC_CAP1_EVCC	0x00000007	/* extended VC count */
#define  PCI_VC_CAP1_LPEVCC	0x00000070	/* low prio extended VC count */
#define  PCI_VC_CAP1_ARB_SIZE	0x00000c00
#define PCI_VC_PORT_CAP2	0x08
#define  PCI_VC_CAP2_32_PHASE		0x00000002
#define  PCI_VC_CAP2_64_PHASE		0x00000004
#define  PCI_VC_CAP2_128_PHASE		0x00000008
#define  PCI_VC_CAP2_ARB_OFF		0xff000000
#define PCI_VC_PORT_CTRL	0x0c
#define  PCI_VC_PORT_CTRL_LOAD_TABLE	0x00000001
#define PCI_VC_PORT_STATUS	0x0e
#define  PCI_VC_PORT_STATUS_TABLE	0x00000001
#define PCI_VC_RES_CAP		0x10
#define  PCI_VC_RES_CAP_32_PHASE	0x00000002
#define  PCI_VC_RES_CAP_64_PHASE	0x00000004
#define  PCI_VC_RES_CAP_128_PHASE	0x00000008
#define  PCI_VC_RES_CAP_128_PHASE_TB	0x00000010
#define  PCI_VC_RES_CAP_256_PHASE	0x00000020
#define  PCI_VC_RES_CAP_ARB_OFF		0xff000000
#define PCI_VC_RES_CTRL		0x14
#define  PCI_VC_RES_CTRL_LOAD_TABLE	0x00010000
#define  PCI_VC_RES_CTRL_ARB_SELECT	0x000e0000
#define  PCI_VC_RES_CTRL_ID		0x07000000
#define  PCI_VC_RES_CTRL_ENABLE		0x80000000
#define PCI_VC_RES_STATUS	0x1a
#define  PCI_VC_RES_STATUS_TABLE	0x00000001
#define  PCI_VC_RES_STATUS_NEGO		0x00000002
#define PCI_CAP_VC_BASE_SIZEOF		0x10
#define PCI_CAP_VC_PER_VC_SIZEOF	0x0c

/* Power Budgeting */
#define PCI_PWR_DSR		0x04	/* Data Select Register */
#define PCI_PWR_DATA		0x08	/* Data Register */
#define  PCI_PWR_DATA_BASE(x)	((x) & 0xff)	    /* Base Power */
#define  PCI_PWR_DATA_SCALE(x)	(((x) >> 8) & 3)    /* Data Scale */
#define  PCI_PWR_DATA_PM_SUB(x)	(((x) >> 10) & 7)   /* PM Sub State */
#define  PCI_PWR_DATA_PM_STATE(x) (((x) >> 13) & 3) /* PM State */
#define  PCI_PWR_DATA_TYPE(x)	(((x) >> 15) & 7)   /* Type */
#define  PCI_PWR_DATA_RAIL(x)	(((x) >> 18) & 7)   /* Power Rail */
#define PCI_PWR_CAP		0x0c	/* Capability */
#define  PCI_PWR_CAP_BUDGET(x)	((x) & 1)	/* Included in system budget */
#define PCI_EXT_CAP_PWR_SIZEOF	0x10

/* Root Complex Event Collector Endpoint Association  */
#define PCI_RCEC_RCIEP_BITMAP	4	/* Associated Bitmap for RCiEPs */
#define PCI_RCEC_BUSN		8	/* RCEC Associated Bus Numbers */
#define  PCI_RCEC_BUSN_REG_VER	0x02	/* Least version with BUSN present */
#define  PCI_RCEC_BUSN_NEXT(x)	(((x) >> 8) & 0xff)
#define  PCI_RCEC_BUSN_LAST(x)	(((x) >> 16) & 0xff)

/* Vendor-Specific (VSEC, PCI_EXT_CAP_ID_VNDR) */
#define PCI_VNDR_HEADER		4	/* Vendor-Specific Header */
#define  PCI_VNDR_HEADER_ID(x)	((x) & 0xffff)
#define  PCI_VNDR_HEADER_REV(x)	(((x) >> 16) & 0xf)
#define  PCI_VNDR_HEADER_LEN(x)	(((x) >> 20) & 0xfff)

/*
 * HyperTransport sub capability types
 *
 * Unfortunately there are both 3 bit and 5 bit capability types defined
 * in the HT spec, catering for that is a little messy. You probably don't
 * want to use these directly, just use pci_find_ht_capability() and it
 * will do the right thing for you.
 */
#define HT_3BIT_CAP_MASK	0xE0
#define HT_CAPTYPE_SLAVE	0x00	/* Slave/Primary link configuration */
#define HT_CAPTYPE_HOST		0x20	/* Host/Secondary link configuration */

#define HT_5BIT_CAP_MASK	0xF8
#define HT_CAPTYPE_IRQ		0x80	/* IRQ Configuration */
#define HT_CAPTYPE_REMAPPING_40	0xA0	/* 40 bit address remapping */
#define HT_CAPTYPE_REMAPPING_64 0xA2	/* 64 bit address remapping */
#define HT_CAPTYPE_UNITID_CLUMP	0x90	/* Unit ID clumping */
#define HT_CAPTYPE_EXTCONF	0x98	/* Extended Configuration Space Access */
#define HT_CAPTYPE_MSI_MAPPING	0xA8	/* MSI Mapping Capability */
#define  HT_MSI_FLAGS		0x02		/* Offset to flags */
#define  HT_MSI_FLAGS_ENABLE	0x1		/* Mapping enable */
#define  HT_MSI_FLAGS_FIXED	0x2		/* Fixed mapping only */
#define  HT_MSI_FIXED_ADDR	0x00000000FEE00000ULL	/* Fixed addr */
#define  HT_MSI_ADDR_LO		0x04		/* Offset to low addr bits */
#define  HT_MSI_ADDR_LO_MASK	0xFFF00000	/* Low address bit mask */
#define  HT_MSI_ADDR_HI		0x08		/* Offset to high addr bits */
#define HT_CAPTYPE_DIRECT_ROUTE	0xB0	/* Direct routing configuration */
#define HT_CAPTYPE_VCSET	0xB8	/* Virtual Channel configuration */
#define HT_CAPTYPE_ERROR_RETRY	0xC0	/* Retry on error configuration */
#define HT_CAPTYPE_GEN3		0xD0	/* Generation 3 HyperTransport configuration */
#define HT_CAPTYPE_PM		0xE0	/* HyperTransport power management configuration */
#define HT_CAP_SIZEOF_LONG	28	/* slave & primary */
#define HT_CAP_SIZEOF_SHORT	24	/* host & secondary */

/* Alternative Routing-ID Interpretation */
#define PCI_ARI_CAP		0x04	/* ARI Capability Register */
#define  PCI_ARI_CAP_MFVC	0x0001	/* MFVC Function Groups Capability */
#define  PCI_ARI_CAP_ACS	0x0002	/* ACS Function Groups Capability */
#define  PCI_ARI_CAP_NFN(x)	(((x) >> 8) & 0xff) /* Next Function Number */
#define PCI_ARI_CTRL		0x06	/* ARI Control Register */
#define  PCI_ARI_CTRL_MFVC	0x0001	/* MFVC Function Groups Enable */
#define  PCI_ARI_CTRL_ACS	0x0002	/* ACS Function Groups Enable */
#define  PCI_ARI_CTRL_FG(x)	(((x) >> 4) & 7) /* Function Group */
#define PCI_EXT_CAP_ARI_SIZEOF	8

/* Address Translation Service */
#define PCI_ATS_CAP		0x04	/* ATS Capability Register */
#define  PCI_ATS_CAP_QDEP(x)	((x) & 0x1f)	/* Invalidate Queue Depth */
#define  PCI_ATS_MAX_QDEP	32	/* Max Invalidate Queue Depth */
#define  PCI_ATS_CAP_PAGE_ALIGNED	0x0020 /* Page Aligned Request */
#define PCI_ATS_CTRL		0x06	/* ATS Control Register */
#define  PCI_ATS_CTRL_ENABLE	0x8000	/* ATS Enable */
#define  PCI_ATS_CTRL_STU(x)	((x) & 0x1f)	/* Smallest Translation Unit */
#define  PCI_ATS_MIN_STU	12	/* shift of minimum STU block */
#define PCI_EXT_CAP_ATS_SIZEOF	8

/* Page Request Interface */
#define PCI_PRI_CTRL		0x04	/* PRI control register */
#define  PCI_PRI_CTRL_ENABLE	0x0001	/* Enable */
#define  PCI_PRI_CTRL_RESET	0x0002	/* Reset */
#define PCI_PRI_STATUS		0x06	/* PRI status register */
#define  PCI_PRI_STATUS_RF	0x0001	/* Response Failure */
#define  PCI_PRI_STATUS_UPRGI	0x0002	/* Unexpected PRG index */
#define  PCI_PRI_STATUS_STOPPED	0x0100	/* PRI Stopped */
#define  PCI_PRI_STATUS_PASID	0x8000	/* PRG Response PASID Required */
#define PCI_PRI_MAX_REQ		0x08	/* PRI max reqs supported */
#define PCI_PRI_ALLOC_REQ	0x0c	/* PRI max reqs allowed */
#define PCI_EXT_CAP_PRI_SIZEOF	16

/* Process Address Space ID */
#define PCI_PASID_CAP		0x04    /* PASID feature register */
#define  PCI_PASID_CAP_EXEC	0x0002	/* Exec permissions Supported */
#define  PCI_PASID_CAP_PRIV	0x0004	/* Privilege Mode Supported */
#define  PCI_PASID_CAP_WIDTH	0x1f00
#define PCI_PASID_CTRL		0x06    /* PASID control register */
#define  PCI_PASID_CTRL_ENABLE	0x0001	/* Enable bit */
#define  PCI_PASID_CTRL_EXEC	0x0002	/* Exec permissions Enable */
#define  PCI_PASID_CTRL_PRIV	0x0004	/* Privilege Mode Enable */
#define PCI_EXT_CAP_PASID_SIZEOF	8

/* Single Root I/O Virtualization */
#define PCI_SRIOV_CAP		0x04	/* SR-IOV Capabilities */
#define  PCI_SRIOV_CAP_VFM	0x00000001  /* VF Migration Capable */
#define  PCI_SRIOV_CAP_INTR(x)	((x) >> 21) /* Interrupt Message Number */
#define PCI_SRIOV_CTRL		0x08	/* SR-IOV Control */
#define  PCI_SRIOV_CTRL_VFE	0x0001	/* VF Enable */
#define  PCI_SRIOV_CTRL_VFM	0x0002	/* VF Migration Enable */
#define  PCI_SRIOV_CTRL_INTR	0x0004	/* VF Migration Interrupt Enable */
#define  PCI_SRIOV_CTRL_MSE	0x0008	/* VF Memory Space Enable */
#define  PCI_SRIOV_CTRL_ARI	0x0010	/* ARI Capable Hierarchy */
#define PCI_SRIOV_STATUS	0x0a	/* SR-IOV Status */
#define  PCI_SRIOV_STATUS_VFM	0x0001	/* VF Migration Status */
#define PCI_SRIOV_INITIAL_VF	0x0c	/* Initial VFs */
#define PCI_SRIOV_TOTAL_VF	0x0e	/* Total VFs */
#define PCI_SRIOV_NUM_VF	0x10	/* Number of VFs */
#define PCI_SRIOV_FUNC_LINK	0x12	/* Function Dependency Link */
#define PCI_SRIOV_VF_OFFSET	0x14	/* First VF Offset */
#define PCI_SRIOV_VF_STRIDE	0x16	/* Following VF Stride */
#define PCI_SRIOV_VF_DID	0x1a	/* VF Device ID */
#define PCI_SRIOV_SUP_PGSIZE	0x1c	/* Supported Page Sizes */
#define PCI_SRIOV_SYS_PGSIZE	0x20	/* System Page Size */
#define PCI_SRIOV_BAR		0x24	/* VF BAR0 */
#define  PCI_SRIOV_NUM_BARS	6	/* Number of VF BARs */
#define PCI_SRIOV_VFM		0x3c	/* VF Migration State Array Offset*/
#define  PCI_SRIOV_VFM_BIR(x)	((x) & 7)	/* State BIR */
#define  PCI_SRIOV_VFM_OFFSET(x) ((x) & ~7)	/* State Offset */
#define  PCI_SRIOV_VFM_UA	0x0	/* Inactive.Unavailable */
#define  PCI_SRIOV_VFM_MI	0x1	/* Dormant.MigrateIn */
#define  PCI_SRIOV_VFM_MO	0x2	/* Active.MigrateOut */
#define  PCI_SRIOV_VFM_AV	0x3	/* Active.Available */
#define PCI_EXT_CAP_SRIOV_SIZEOF 0x40

#define PCI_LTR_MAX_SNOOP_LAT	0x4
#define PCI_LTR_MAX_NOSNOOP_LAT	0x6
#define  PCI_LTR_VALUE_MASK	0x000003ff
#define  PCI_LTR_SCALE_MASK	0x00001c00
#define  PCI_LTR_SCALE_SHIFT	10
#define  PCI_LTR_NOSNOOP_VALUE	0x03ff0000 /* Max No-Snoop Latency Value */
#define  PCI_LTR_NOSNOOP_SCALE	0x1c000000 /* Scale for Max Value */
#define PCI_EXT_CAP_LTR_SIZEOF	8

/* Access Control Service */
#define PCI_ACS_CAP		0x04	/* ACS Capability Register */
#define  PCI_ACS_SV		0x0001	/* Source Validation */
#define  PCI_ACS_TB		0x0002	/* Translation Blocking */
#define  PCI_ACS_RR		0x0004	/* P2P Request Redirect */
#define  PCI_ACS_CR		0x0008	/* P2P Completion Redirect */
#define  PCI_ACS_UF		0x0010	/* Upstream Forwarding */
#define  PCI_ACS_EC		0x0020	/* P2P Egress Control */
#define  PCI_ACS_DT		0x0040	/* Direct Translated P2P */
#define PCI_ACS_EGRESS_BITS	0x05	/* ACS Egress Control Vector Size */
#define PCI_ACS_CTRL		0x06	/* ACS Control Register */
#define PCI_ACS_EGRESS_CTL_V	0x08	/* ACS Egress Control Vector */

#define PCI_VSEC_HDR		4	/* extended cap - vendor-specific */
#define  PCI_VSEC_HDR_LEN_SHIFT	20	/* shift for length field */

/* SATA capability */
#define PCI_SATA_REGS		4	/* SATA REGs specifier */
#define  PCI_SATA_REGS_MASK	0xF	/* location - BAR#/inline */
#define  PCI_SATA_REGS_INLINE	0xF	/* REGS in config space */
#define PCI_SATA_SIZEOF_SHORT	8
#define PCI_SATA_SIZEOF_LONG	16

/* Resizable BARs */
#define PCI_REBAR_CAP		4	/* capability register */
#define  PCI_REBAR_CAP_SIZES		0x00FFFFF0  /* supported BAR sizes */
#define PCI_REBAR_CTRL		8	/* control register */
#define  PCI_REBAR_CTRL_BAR_IDX		0x00000007  /* BAR index */
#define  PCI_REBAR_CTRL_NBAR_MASK	0x000000E0  /* # of resizable BARs */
#define  PCI_REBAR_CTRL_NBAR_SHIFT	5	    /* shift for # of BARs */
#define  PCI_REBAR_CTRL_BAR_SIZE	0x00001F00  /* BAR size */
#define  PCI_REBAR_CTRL_BAR_SHIFT	8	    /* shift for BAR size */

/* Dynamic Power Allocation */
#define PCI_DPA_CAP		4	/* capability register */
#define  PCI_DPA_CAP_SUBSTATE_MASK	0x1F	/* # substates - 1 */
#define PCI_DPA_BASE_SIZEOF	16	/* size with 0 substates */

/* TPH Requester */
#define PCI_TPH_CAP		4	/* capability register */
#define  PCI_TPH_CAP_LOC_MASK	0x600	/* location mask */
#define   PCI_TPH_LOC_NONE	0x000	/* no location */
#define   PCI_TPH_LOC_CAP	0x200	/* in capability */
#define   PCI_TPH_LOC_MSIX	0x400	/* in MSI-X */
#define PCI_TPH_CAP_ST_MASK	0x07FF0000	/* ST table mask */
#define PCI_TPH_CAP_ST_SHIFT	16	/* ST table shift */
#define PCI_TPH_BASE_SIZEOF	0xc	/* size with no ST table */

/* Downstream Port Containment */
#define PCI_EXP_DPC_CAP			0x04	/* DPC Capability */
#define PCI_EXP_DPC_IRQ			0x001F	/* Interrupt Message Number */
#define  PCI_EXP_DPC_CAP_RP_EXT		0x0020	/* Root Port Extensions */
#define  PCI_EXP_DPC_CAP_POISONED_TLP	0x0040	/* Poisoned TLP Egress Blocking Supported */
#define  PCI_EXP_DPC_CAP_SW_TRIGGER	0x0080	/* Software Triggering Supported */
#define  PCI_EXP_DPC_RP_PIO_LOG_SIZE	0x0F00	/* RP PIO Log Size */
#define  PCI_EXP_DPC_CAP_DL_ACTIVE	0x1000	/* ERR_COR signal on DL_Active supported */

#define PCI_EXP_DPC_CTL			0x06	/* DPC control */
#define  PCI_EXP_DPC_CTL_EN_FATAL	0x0001	/* Enable trigger on ERR_FATAL message */
#define  PCI_EXP_DPC_CTL_EN_NONFATAL	0x0002	/* Enable trigger on ERR_NONFATAL message */
#define  PCI_EXP_DPC_CTL_INT_EN		0x0008	/* DPC Interrupt Enable */

#define PCI_EXP_DPC_STATUS		0x08	/* DPC Status */
#define  PCI_EXP_DPC_STATUS_TRIGGER	    0x0001 /* Trigger Status */
#define  PCI_EXP_DPC_STATUS_TRIGGER_RSN	    0x0006 /* Trigger Reason */
#define  PCI_EXP_DPC_STATUS_TRIGGER_RSN_UNCOR  0x0000 /* Uncorrectable error */
#define  PCI_EXP_DPC_STATUS_TRIGGER_RSN_NFE    0x0002 /* Rcvd ERR_NONFATAL */
#define  PCI_EXP_DPC_STATUS_TRIGGER_RSN_FE     0x0004 /* Rcvd ERR_FATAL */
#define  PCI_EXP_DPC_STATUS_TRIGGER_RSN_IN_EXT 0x0006 /* Reason in Trig Reason Extension field */
#define  PCI_EXP_DPC_STATUS_INTERRUPT	    0x0008 /* Interrupt Status */
#define  PCI_EXP_DPC_RP_BUSY		    0x0010 /* Root Port Busy */
#define  PCI_EXP_DPC_STATUS_TRIGGER_RSN_EXT 0x0060 /* Trig Reason Extension */
#define  PCI_EXP_DPC_STATUS_TRIGGER_RSN_RP_PIO		0x0000	/* RP PIO error */
#define  PCI_EXP_DPC_STATUS_TRIGGER_RSN_SW_TRIGGER	0x0020	/* DPC SW Trigger bit */
#define  PCI_EXP_DPC_RP_PIO_FEP		    0x1f00 /* RP PIO First Err Ptr */

#define PCI_EXP_DPC_SOURCE_ID		 0x0A	/* DPC Source Identifier */

#define PCI_EXP_DPC_RP_PIO_STATUS	 0x0C	/* RP PIO Status */
#define PCI_EXP_DPC_RP_PIO_MASK		 0x10	/* RP PIO Mask */
#define PCI_EXP_DPC_RP_PIO_SEVERITY	 0x14	/* RP PIO Severity */
#define PCI_EXP_DPC_RP_PIO_SYSERROR	 0x18	/* RP PIO SysError */
#define PCI_EXP_DPC_RP_PIO_EXCEPTION	 0x1C	/* RP PIO Exception */
#define PCI_EXP_DPC_RP_PIO_HEADER_LOG	 0x20	/* RP PIO Header Log */
#define PCI_EXP_DPC_RP_PIO_IMPSPEC_LOG	 0x30	/* RP PIO ImpSpec Log */
#define PCI_EXP_DPC_RP_PIO_TLPPREFIX_LOG 0x34	/* RP PIO TLP Prefix Log */

/* Precision Time Measurement */
#define PCI_PTM_CAP			0x04	    /* PTM Capability */
#define  PCI_PTM_CAP_REQ		0x00000001  /* Requester capable */
#define  PCI_PTM_CAP_RES		0x00000002  /* Responder capable */
#define  PCI_PTM_CAP_ROOT		0x00000004  /* Root capable */
#define  PCI_PTM_GRANULARITY_MASK	0x0000FF00  /* Clock granularity */
#define PCI_PTM_CTRL			0x08	    /* PTM Control */
#define  PCI_PTM_CTRL_ENABLE		0x00000001  /* PTM enable */
#define  PCI_PTM_CTRL_ROOT		0x00000002  /* Root select */

/* ASPM L1 PM Substates */
#define PCI_L1SS_CAP		0x04	/* Capabilities Register */
#define  PCI_L1SS_CAP_PCIPM_L1_2	0x00000001  /* PCI-PM L1.2 Supported */
#define  PCI_L1SS_CAP_PCIPM_L1_1	0x00000002  /* PCI-PM L1.1 Supported */
#define  PCI_L1SS_CAP_ASPM_L1_2		0x00000004  /* ASPM L1.2 Supported */
#define  PCI_L1SS_CAP_ASPM_L1_1		0x00000008  /* ASPM L1.1 Supported */
#define  PCI_L1SS_CAP_L1_PM_SS		0x00000010  /* L1 PM Substates Supported */
#define  PCI_L1SS_CAP_CM_RESTORE_TIME	0x0000ff00  /* Port Common_Mode_Restore_Time */
#define  PCI_L1SS_CAP_P_PWR_ON_SCALE	0x00030000  /* Port T_POWER_ON scale */
#define  PCI_L1SS_CAP_P_PWR_ON_VALUE	0x00f80000  /* Port T_POWER_ON value */
#define PCI_L1SS_CTL1		0x08	/* Control 1 Register */
#define  PCI_L1SS_CTL1_PCIPM_L1_2	0x00000001  /* PCI-PM L1.2 Enable */
#define  PCI_L1SS_CTL1_PCIPM_L1_1	0x00000002  /* PCI-PM L1.1 Enable */
#define  PCI_L1SS_CTL1_ASPM_L1_2	0x00000004  /* ASPM L1.2 Enable */
#define  PCI_L1SS_CTL1_ASPM_L1_1	0x00000008  /* ASPM L1.1 Enable */
#define  PCI_L1SS_CTL1_L1_2_MASK	0x00000005
#define  PCI_L1SS_CTL1_L1SS_MASK	0x0000000f
#define  PCI_L1SS_CTL1_CM_RESTORE_TIME	0x0000ff00  /* Common_Mode_Restore_Time */
#define  PCI_L1SS_CTL1_LTR_L12_TH_VALUE	0x03ff0000  /* LTR_L1.2_THRESHOLD_Value */
#define  PCI_L1SS_CTL1_LTR_L12_TH_SCALE	0xe0000000  /* LTR_L1.2_THRESHOLD_Scale */
#define PCI_L1SS_CTL2		0x0c	/* Control 2 Register */
#define  PCI_L1SS_CTL2_T_PWR_ON_SCALE	0x00000003  /* T_POWER_ON Scale */
#define  PCI_L1SS_CTL2_T_PWR_ON_VALUE	0x000000f8  /* T_POWER_ON Value */

/* Designated Vendor-Specific (DVSEC, PCI_EXT_CAP_ID_DVSEC) */
#define PCI_DVSEC_HEADER1		0x4 /* Designated Vendor-Specific Header1 */
#define  PCI_DVSEC_HEADER1_VID(x)	((x) & 0xffff)
#define  PCI_DVSEC_HEADER1_REV(x)	(((x) >> 16) & 0xf)
#define  PCI_DVSEC_HEADER1_LEN(x)	(((x) >> 20) & 0xfff)
#define PCI_DVSEC_HEADER2		0x8 /* Designated Vendor-Specific Header2 */
#define  PCI_DVSEC_HEADER2_ID(x)		((x) & 0xffff)

/* Data Link Feature */
#define PCI_DLF_CAP		0x04	/* Capabilities Register */
#define  PCI_DLF_EXCHANGE_ENABLE	0x80000000  /* Data Link Feature Exchange Enable */

/* Physical Layer 16.0 GT/s */
#define PCI_PL_16GT_LE_CTRL	0x20	/* Lane Equalization Control Register */
#define  PCI_PL_16GT_LE_CTRL_DSP_TX_PRESET_MASK		0x0000000F
#define  PCI_PL_16GT_LE_CTRL_USP_TX_PRESET_MASK		0x000000F0
#define  PCI_PL_16GT_LE_CTRL_USP_TX_PRESET_SHIFT	4

/* Data Object Exchange */
#define PCI_DOE_CAP		0x04    /* DOE Capabilities Register */
#define  PCI_DOE_CAP_INT_SUP			0x00000001  /* Interrupt Support */
#define  PCI_DOE_CAP_INT_MSG_NUM		0x00000ffe  /* Interrupt Message Number */
#define PCI_DOE_CTRL		0x08    /* DOE Control Register */
#define  PCI_DOE_CTRL_ABORT			0x00000001  /* DOE Abort */
#define  PCI_DOE_CTRL_INT_EN			0x00000002  /* DOE Interrupt Enable */
#define  PCI_DOE_CTRL_GO			0x80000000  /* DOE Go */
#define PCI_DOE_STATUS		0x0c    /* DOE Status Register */
#define  PCI_DOE_STATUS_BUSY			0x00000001  /* DOE Busy */
#define  PCI_DOE_STATUS_INT_STATUS		0x00000002  /* DOE Interrupt Status */
#define  PCI_DOE_STATUS_ERROR			0x00000004  /* DOE Error */
#define  PCI_DOE_STATUS_DATA_OBJECT_READY	0x80000000  /* Data Object Ready */
#define PCI_DOE_WRITE		0x10    /* DOE Write Data Mailbox Register */
#define PCI_DOE_READ		0x14    /* DOE Read Data Mailbox Register */
#define PCI_DOE_CAP_SIZEOF	0x18	/* Size of DOE register block */

/* DOE Data Object - note not actually registers */
#define PCI_DOE_DATA_OBJECT_HEADER_1_VID		0x0000ffff
#define PCI_DOE_DATA_OBJECT_HEADER_1_TYPE		0x00ff0000
#define PCI_DOE_DATA_OBJECT_HEADER_2_LENGTH		0x0003ffff

#define PCI_DOE_DATA_OBJECT_DISC_REQ_3_INDEX		0x000000ff
#define PCI_DOE_DATA_OBJECT_DISC_RSP_3_VID		0x0000ffff
#define PCI_DOE_DATA_OBJECT_DISC_RSP_3_PROTOCOL		0x00ff0000
#define PCI_DOE_DATA_OBJECT_DISC_RSP_3_NEXT_INDEX	0xff000000

#endif /* LINUX_PCI_REGS_H */

/*
 * IO resources have these defined flags.
 *
 * PCI devices expose these flags to userspace in the "resource" sysfs file,
 * so don't move them.
 */
#define IORESOURCE_BITS		0x000000ff	/* Bus-specific bits */

#define IORESOURCE_TYPE_BITS	0x00001f00	/* Resource type */
#define IORESOURCE_IO		0x00000100	/* PCI/ISA I/O ports */
#define IORESOURCE_MEM		0x00000200
#define IORESOURCE_REG		0x00000300	/* Register offsets */
#define IORESOURCE_IRQ		0x00000400
#define IORESOURCE_DMA		0x00000800
#define IORESOURCE_BUS		0x00001000

#define IORESOURCE_PREFETCH	0x00002000	/* No side effects */
#define IORESOURCE_READONLY	0x00004000
#define IORESOURCE_CACHEABLE	0x00008000
#define IORESOURCE_RANGELENGTH	0x00010000
#define IORESOURCE_SHADOWABLE	0x00020000

#define IORESOURCE_SIZEALIGN	0x00040000	/* size indicates alignment */
#define IORESOURCE_STARTALIGN	0x00080000	/* start field is alignment */

#define IORESOURCE_MEM_64	0x00100000
#define IORESOURCE_WINDOW	0x00200000	/* forwarded by bridge */
#define IORESOURCE_MUXED	0x00400000	/* Resource is software muxed */

#define IORESOURCE_EXT_TYPE_BITS 0x01000000	/* Resource extended types */
#define IORESOURCE_SYSRAM	0x01000000	/* System RAM (modifier) */

/* IORESOURCE_SYSRAM specific bits. */
#define IORESOURCE_SYSRAM_DRIVER_MANAGED	0x02000000 /* Always detected via a driver. */
#define IORESOURCE_SYSRAM_MERGEABLE		0x04000000 /* Resource can be merged. */

#define IORESOURCE_EXCLUSIVE	0x08000000	/* Userland may not map this resource */

#define IORESOURCE_DISABLED	0x10000000
#define IORESOURCE_UNSET	0x20000000	/* No address assigned yet */
#define IORESOURCE_AUTO		0x40000000
#define IORESOURCE_BUSY		0x80000000	/* Driver has marked this resource busy */

/* PnP IRQ specific bits (IORESOURCE_BITS) */
#define IORESOURCE_IRQ_HIGHEDGE		(1<<0)
#define IORESOURCE_IRQ_LOWEDGE		(1<<1)
#define IORESOURCE_IRQ_HIGHLEVEL	(1<<2)
#define IORESOURCE_IRQ_LOWLEVEL		(1<<3)
#define IORESOURCE_IRQ_SHAREABLE	(1<<4)
#define IORESOURCE_IRQ_OPTIONAL		(1<<5)
#define IORESOURCE_IRQ_WAKECAPABLE	(1<<6)

/* PnP DMA specific bits (IORESOURCE_BITS) */
#define IORESOURCE_DMA_TYPE_MASK	(3<<0)
#define IORESOURCE_DMA_8BIT		(0<<0)
#define IORESOURCE_DMA_8AND16BIT	(1<<0)
#define IORESOURCE_DMA_16BIT		(2<<0)

#define IORESOURCE_DMA_MASTER		(1<<2)
#define IORESOURCE_DMA_BYTE		(1<<3)
#define IORESOURCE_DMA_WORD		(1<<4)

#define IORESOURCE_DMA_SPEED_MASK	(3<<6)
#define IORESOURCE_DMA_COMPATIBLE	(0<<6)
#define IORESOURCE_DMA_TYPEA		(1<<6)
#define IORESOURCE_DMA_TYPEB		(2<<6)
#define IORESOURCE_DMA_TYPEF		(3<<6)

/* PnP memory I/O specific bits (IORESOURCE_BITS) */
#define IORESOURCE_MEM_WRITEABLE	(1<<0)	/* dup: IORESOURCE_READONLY */
#define IORESOURCE_MEM_CACHEABLE	(1<<1)	/* dup: IORESOURCE_CACHEABLE */
#define IORESOURCE_MEM_RANGELENGTH	(1<<2)	/* dup: IORESOURCE_RANGELENGTH */
#define IORESOURCE_MEM_TYPE_MASK	(3<<3)
#define IORESOURCE_MEM_8BIT		(0<<3)
#define IORESOURCE_MEM_16BIT		(1<<3)
#define IORESOURCE_MEM_8AND16BIT	(2<<3)
#define IORESOURCE_MEM_32BIT		(3<<3)
#define IORESOURCE_MEM_SHADOWABLE	(1<<5)	/* dup: IORESOURCE_SHADOWABLE */
#define IORESOURCE_MEM_EXPANSIONROM	(1<<6)
#define IORESOURCE_MEM_NONPOSTED	(1<<7)

/* PnP I/O specific bits (IORESOURCE_BITS) */
#define IORESOURCE_IO_16BIT_ADDR	(1<<0)
#define IORESOURCE_IO_FIXED		(1<<1)
#define IORESOURCE_IO_SPARSE		(1<<2)

/* PCI ROM control bits (IORESOURCE_BITS) */
#define IORESOURCE_ROM_ENABLE		(1<<0)	/* ROM is enabled, same as PCI_ROM_ADDRESS_ENABLE */
#define IORESOURCE_ROM_SHADOW		(1<<1)	/* Use RAM image, not ROM BAR */

/* PCI control bits.  Shares IORESOURCE_BITS with above PCI ROM.  */
#define IORESOURCE_PCI_FIXED		(1<<4)	/* Do not move resource */
#define IORESOURCE_PCI_EA_BEI		(1<<5)	/* BAR Equivalent Indicator */

#define PCI_D0		((pci_power_t __force) 0)
#define PCI_D1		((pci_power_t __force) 1)
#define PCI_D2		((pci_power_t __force) 2)
#define PCI_D3hot	((pci_power_t __force) 3)
#define PCI_D3cold	((pci_power_t __force) 4)
#define PCI_UNKNOWN	((pci_power_t __force) 5)
#define PCI_POWER_ERROR	((pci_power_t __force) -1)

#define PCI_MAX_BARS	6
#define PCI_MAX_IRQS	32
#define PCI_CONFIG_SIZE	4096
#define PCI_DMA_ALIGN   4096
#define PCI_DMA_BASE    0x10000000

/* Number of possible devfns: 0.0 to 1f.7 inclusive */
#define MAX_NR_DEVFNS 256
#define PCI_MAX_NR_DEVS	32
#define MAX_NR_LANES 16
#define PCI_FIND_CAP_TTL 48
#define PCI_VSEC_ID_INTEL_TBT 0x1234	/* Thunderbolt */
#define PCIE_LINK_RETRAIN_TIMEOUT_MS 1000

#define PCI_REBAR_MIN_SIZE ((resource_size_t)SZ_1M)

enum {
	/* I/O channel is in normal state */
	pci_channel_io_normal = (__force pci_channel_state_t) 1,

	/* I/O to channel is blocked */
	pci_channel_io_frozen = (__force pci_channel_state_t) 2,

	/* PCI card is dead */
	pci_channel_io_perm_failure = (__force pci_channel_state_t) 3,
};

enum pcie_reset_state {
	/* Reset is NOT asserted (Use to deassert reset) */
	pcie_deassert_reset = (__force pcie_reset_state_t) 1,

	/* Use #PERST to reset PCIe device */
	pcie_warm_reset = (__force pcie_reset_state_t) 2,

	/* Use PCIe Hot Reset to reset device */
	pcie_hot_reset = (__force pcie_reset_state_t) 3
};

enum pci_search_direction {
	PCI_SEARCH_FORWARD,
	PCI_SEARCH_REVERSE,
};

/* File state for mmap()s on /proc/bus/pci/X/Y */
enum pci_mmap_state {
	pci_mmap_io,
	pci_mmap_mem
};

/* For PCI devices, the region numbers are assigned this way: */
enum {
	/* #0-5: standard PCI resources */
	PCI_STD_RESOURCES,
	PCI_STD_RESOURCE_END = PCI_STD_RESOURCES + PCI_STD_NUM_BARS - 1,

	/* #6: expansion ROM resource */
	PCI_ROM_RESOURCE,

	/* Device-specific resources */
	#ifdef CONFIG_PCI_IOV
	PCI_IOV_RESOURCES,
	PCI_IOV_RESOURCE_END = PCI_IOV_RESOURCES + PCI_SRIOV_NUM_BARS - 1,
	#endif

	/* PCI-to-PCI (P2P) bridge windows */
	#define PCI_BRIDGE_IO_WINDOW		(PCI_BRIDGE_RESOURCES + 0)
	#define PCI_BRIDGE_MEM_WINDOW		(PCI_BRIDGE_RESOURCES + 1)
	#define PCI_BRIDGE_PREF_MEM_WINDOW	(PCI_BRIDGE_RESOURCES + 2)

	/* CardBus bridge windows */
	#define PCI_CB_BRIDGE_IO_0_WINDOW	(PCI_BRIDGE_RESOURCES + 0)
	#define PCI_CB_BRIDGE_IO_1_WINDOW	(PCI_BRIDGE_RESOURCES + 1)
	#define PCI_CB_BRIDGE_MEM_0_WINDOW	(PCI_BRIDGE_RESOURCES + 2)
	#define PCI_CB_BRIDGE_MEM_1_WINDOW	(PCI_BRIDGE_RESOURCES + 3)

	/* Total number of bridge resources for P2P and CardBus */
	#define PCI_P2P_BRIDGE_RESOURCE_NUM	3
	#define PCI_BRIDGE_RESOURCE_NUM		4

	/* Resources assigned to buses behind the bridge */
	PCI_BRIDGE_RESOURCES,
	PCI_BRIDGE_RESOURCE_END = PCI_BRIDGE_RESOURCES +
				  PCI_BRIDGE_RESOURCE_NUM - 1,

	/* Total resources associated with a PCI device */
	PCI_NUM_RESOURCES,

	/* Preserve this for compatibility */
	DEVICE_COUNT_RESOURCE = PCI_NUM_RESOURCES,
};

/*
 * enum pci_interrupt_pin - PCI INTx interrupt values
 * @PCI_INTERRUPT_UNKNOWN: Unknown or unassigned interrupt
 * @PCI_INTERRUPT_INTA: PCI INTA pin
 * @PCI_INTERRUPT_INTB: PCI INTB pin
 * @PCI_INTERRUPT_INTC: PCI INTC pin
 * @PCI_INTERRUPT_INTD: PCI INTD pin
 *
 * Corresponds to values for legacy PCI INTx interrupts, as can be found in the
 * PCI_INTERRUPT_PIN register.
 */
enum pci_interrupt_pin {
	PCI_INTERRUPT_UNKNOWN,
	PCI_INTERRUPT_INTA,
	PCI_INTERRUPT_INTB,
	PCI_INTERRUPT_INTC,
	PCI_INTERRUPT_INTD,
};

struct pci_bus_region {
	pci_bus_addr_t	start;
	pci_bus_addr_t	end;
};

/*
 * struct pci_device_id - PCI device ID structure
 * @vendor:		Vendor ID to match (or PCI_ANY_ID)
 * @device:		Device ID to match (or PCI_ANY_ID)
 * @subvendor:		Subsystem vendor ID to match (or PCI_ANY_ID)
 * @subdevice:		Subsystem device ID to match (or PCI_ANY_ID)
 * @class:		Device class, subclass, and "interface" to match.
 *			See Appendix D of the PCI Local Bus Spec or
 *			include/linux/pci_ids.h for a full list of classes.
 *			Most drivers do not need to specify class/class_mask
 *			as vendor/device is normally sufficient.
 * @class_mask:		Limit which sub-fields of the class field are compared.
 *			See drivers/scsi/sym53c8xx_2/ for example of usage.
 * @driver_data:	Data private to the driver.
 *			Most drivers don't need to use driver_data field.
 *			Best practice is to use driver_data as an index
 *			into a static list of equivalent device types,
 *			instead of using it as a pointer.
 * @override_only:	Match only when dev->driver_override is this driver.
 */
struct pci_device_id {
	__u32 vendor, device;		/* Vendor and device ID or PCI_ANY_ID*/
	__u32 subvendor, subdevice;	/* Subsystem ID's or PCI_ANY_ID */
	__u32 class, class_mask;	/* (class,subclass,prog-if) triplet */
	unsigned long driver_data;	/* Data private to the driver */
	__u32 override_only;
};

/*
 * Resources are tree-like, allowing
 * nesting etc..
 */
struct resource {
	resource_size_t start;
	resource_size_t end;
	const char *name;
	unsigned long flags;
	unsigned long desc;
	struct resource *parent, *sibling, *child;
};

/* the following structs are the real bridge between the target PCIe device and Exsul */
struct exsul_pci_dma {
	void *address;
	void *map_address;
	size_t requested_size;
	size_t map_size;
	u64 iova;
	u64 map_iova;
	unsigned int references;
	bool owned;
	/* next pci dma area */
	struct exsul_pci_dma *next;
};

/* exsul_pci_irq: contains the irq specific data, the file descriptor, the associated thread and the handler ptr */
struct exsul_pci_irq {
	int event_fd;
	unsigned int linux_irq;
	unsigned int vector;
	irq_handler_t handler;
	irq_handler_t thread_fn;
	void *dev_id;
	pthread_t thread;
	atomic_bool stop;
	bool started;
};

struct exsul_vfio {
	int container_fd;
	int group_fd;
	int device_fd;
	int group_id;
	bool group_attached;
	bool opened;
	char bdf[32];

	struct vfio_device_info device_info;
	struct vfio_region_info regions[VFIO_PCI_NUM_REGIONS];
	void *bars[PCI_MAX_BARS];
	size_t bar_sizes[PCI_MAX_BARS];

	unsigned int irq_index;
	unsigned int irq_count;
	struct exsul_pci_irq irqs[PCI_MAX_IRQS];

	u64 next_iova;
	struct exsul_pci_dma *dma;
	pthread_mutex_t lock;
	bool lock_initialized;
};

/*
 * resource_set_size - Calculate resource end address from size and start
 * @res: Resource descriptor
 * @size: Size of the resource
 *
 * Calculate the end address for @res based on @size.
 *
 * Note: The start address of @res must be set when calling this function.
 * Prefer resource_set_range() if setting both the start address and @size.
 */
static inline void resource_set_size(struct resource *res, resource_size_t size){
	res->end = res->start + size - 1;
}

/*
 * resource_set_range - Set resource start and end addresses
 * @res: Resource descriptor
 * @start: Start address for the resource
 * @size: Size of the resource
 *
 * Set @res start address and calculate the end address based on @size.
 */
static inline void resource_set_range(struct resource *res, resource_size_t start, resource_size_t size){
	res->start = start;
	resource_set_size(res, size);
}

static inline resource_size_t resource_size(const struct resource *res){
	return res->end - res->start + 1;
}

static inline unsigned long resource_type(const struct resource *res){
	return res->flags & IORESOURCE_TYPE_BITS;
}

static inline unsigned long resource_ext_type(const struct resource *res){
	return res->flags & IORESOURCE_EXT_TYPE_BITS;
}

/*
 * For checking if @r1 completely contains @r2 for resources that have real
 * addresses but are not yet crafted into the resource tree. Normally
 * resource_contains() should be used instead of this function as it checks
 * also IORESOURCE_UNSET flag.
 */
static inline bool __resource_contains_unbound(const struct resource *r1, const struct resource *r2){
	if (resource_type(r1) != resource_type(r2)){
		return false;
	}
	return r1->start <= r2->start && r1->end >= r2->end;
}
/* True iff r1 completely contains r2 */
static inline bool resource_contains(const struct resource *r1, const struct resource *r2){
	if (r1->flags & IORESOURCE_UNSET || r2->flags & IORESOURCE_UNSET){
		return false;
	}
	return __resource_contains_unbound(r1, r2);
}

/* True if any part of r1 overlaps r2 */
static inline bool resource_overlaps(const struct resource *r1, const struct resource *r2){
       return r1->start <= r2->end && r1->end >= r2->start;
}

static inline bool resource_intersection(const struct resource *r1, const struct resource *r2, struct resource *r){
	if (!resource_overlaps(r1, r2)){
		return false;
	}
	r->start = max(r1->start, r2->start);
	r->end = min(r1->end, r2->end);
	return true;
}

static inline bool resource_union(const struct resource *r1, const struct resource *r2, struct resource *r){
	if (!resource_overlaps(r1, r2)){
		return false;
	}
	r->start = min(r1->start, r2->start);
	r->end = max(r1->end, r2->end);
	return true;
}

static int read_hex_file(const char *path, u32 *value){
	int ret = 0;
	int fd = 0;
	int length = 0;
	int saved_errno = 0;
	char *end = NULL;
	char buffer[32] = {};

	if(path == NULL || value == NULL){
		ret = -EINVAL;
		return ret;
	}

	fd = open(path, O_RDONLY | O_CLOEXEC);
	if(fd < 0){
		ret = -errno;
		return ret;
	}

	length = read(fd, buffer, sizeof(buffer) - 1);
	saved_errno = errno;
	close(fd);
	if(length < 0){
		ret = -saved_errno;
		return ret;
	}
	if(length == 0){
		ret = -EIO;
		return ret;
	}

	buffer[length] = 0;
	errno = 0;
	*value = strtoul(buffer, &end, 0);
	if(errno || end == buffer){
		ret = -EINVAL;
		return ret;
	}
	return ret;
}

static int get_vfio_group_id(const char *bdf){
	int ret = 0;
	int length = 0;
	char *name = NULL;
	char path[PATH_MAX] = {};
	char target[PATH_MAX] = {};

	if(bdf == NULL){
		ret = -EINVAL;
		return ret;
	}
	snprintf(path, sizeof(path), "/sys/bus/pci/devices/%s/iommu_group", bdf);
	length = readlink(path, target, sizeof(target) - 1);
	if(length < 0){
		ret = -errno;
		#ifdef ADVANCED_PCI_DBG

		#endif
		return ret;
	}

	target[length] = 0;
	name = strrchr(target, '/');
	if(name == NULL || name[1] == 0){
		ret = -EINVAL;
		return ret;
	}

	ret = atoi(name + 1);
	return ret;
}


static int vfio_region_info(unsigned int index, struct vfio_region_info *region){
	int ret = 0;

	if(region == NULL || director.vfio->device_fd < 0){
		ret = -EINVAL;
		return ret;
	}

	memset(region, 0, sizeof(*region));
	region->argsz = sizeof(*region);
	region->index = index;
	ret = ioctl(director.vfio->device_fd, VFIO_DEVICE_GET_REGION_INFO, region);
	if(ret < 0){
		ret = -errno;
		#ifdef ADVANCED_PCI_DBG

		#endif
		return ret;
	}

	return ret;
}

static int vfio_dma_unmap(struct exsul_pci_dma *dma){
	int ret = 0;
	struct exsul_pci_dma **item = NULL;
	struct vfio_iommu_type1_dma_unmap unmap = {};

	if(dma == NULL){
		ret = -EINVAL;
		#ifdef ADVANCED_PCI_DBG

		#endif
		return ret;
	}

	unmap.argsz = sizeof(unmap);
	unmap.iova = dma->map_iova;
	unmap.size = dma->map_size;
	ret = ioctl(director.vfio->container_fd, VFIO_IOMMU_UNMAP_DMA, &unmap);
	if(ret < 0){
		ret = -errno;
		#ifdef ADVANCED_PCI_DBG

		#endif
		return ret;
	}

	pthread_mutex_lock(&director.vfio->lock);
	for(item = &director.vfio->dma; *item != NULL; item = &(*item)->next){
		if(*item == dma){
			*item = dma->next;
			break;
		}
	}
	pthread_mutex_unlock(&director.vfio->lock);
	if(dma->owned){
		munmap(dma->map_address, dma->map_size);
	}
	free(dma);
	return ret;
}

static int vfio_close(void){
	int ret = 0;
	struct exsul_vfio *vfio = NULL;
	struct exsul_pci_dma *dma = NULL;
	struct exsul_pci_dma *next = NULL;

	vfio = &director.vfio;
	if(vfio->device_fd >= 0){
		pci_free_irq_vectors(director.pci_device);
	}

	for(dma = vfio->dma; dma != NULL; dma = next){
		next = dma->next;
		vfio_dma_unmap(dma);
	}
	vfio->dma = NULL;

	for(int i = 0; i < PCI_MAX_BARS; i++){
		if(vfio->bars[i] != MAP_FAILED){
			munmap(vfio->bars[i], vfio->bar_sizes[i]);
			vfio->bars[i] = MAP_FAILED;
			vfio->bar_sizes[i] = 0;
		}
	}

	if(vfio->device_fd >= 0){
		if(vfio->device_info.flags & VFIO_DEVICE_FLAGS_RESET){
			ioctl(vfio->device_fd, VFIO_DEVICE_RESET);
		}
		close(vfio->device_fd);
		vfio->device_fd = -1;
	}

	if(vfio->group_attached && vfio->group_fd >= 0){
		ioctl(vfio->group_fd, VFIO_GROUP_UNSET_CONTAINER);
		vfio->group_attached = false;
	}

	if(vfio->group_fd >= 0){
		close(vfio->group_fd);
		vfio->group_fd = -1;
	}

	if(vfio->container_fd >= 0){
		close(vfio->container_fd);
		vfio->container_fd = -1;
	}

	if(vfio->lock_initialized){
		pthread_mutex_destroy(&vfio->lock);
		vfio->lock_initialized = false;
	}
	vfio->opened = false;
	return ret;
}

static int vfio_open(const char *bdf){
	int ret = 0;
	struct exsul_vfio *vfio = NULL;
	char path[64] = {};
	struct vfio_group_status status = {};

	vfio = director.vfio;
	/* initial sanity check */
	if(bdf == NULL || bdf[0] == 0){
		ret = -EINVAL;
		#ifdef ADVANCED_PCI_DBG

		#endif
		return ret;
	}

	memset(vfio, 0, sizeof(*vfio));
	vfio->container_fd = -1;
	vfio->group_fd = -1;
	vfio->device_fd = -1;
	vfio->next_iova = PCI_DMA_BASE;
	strncpy(vfio->bdf, bdf, sizeof(vfio->bdf) - 1);
	/* initialize the PCIe BARs and irqs vector */
	for(int i = 0; i < PCI_MAX_BARS; i++){
		vfio->bars[i] = MAP_FAILED;
	}
	for(int i = 0; i < PCI_MAX_IRQS; i++){
		vfio->irqs[i].event_fd = -1;
	}

	ret = pthread_mutex_init(&vfio->lock, NULL);
	/* probably this check will be deleted soon */
	if(ret){
		#ifdef ADVANCED_PCI_DBG

		#endif
		return ret;
	}
	vfio->lock_initialized = true;

	vfio->group_id = vfio_group_id(bdf);
	if(vfio->group_id < 0){
		ret = vfio->group_id;
		/* check which group id fails */
		#ifdef ADVANCED_PCI_DBG

		#endif
		goto error;
	}

	vfio->container_fd = open("/dev/vfio/vfio", O_RDWR | O_CLOEXEC);
	/* failed to open the vfio character device, probably there is some error at the kernel level */
	if(vfio->container_fd < 0){
		ret = -errno;
		#ifdef ADVANCED_PCI_DBG

		#endif
		goto error;
	}

	ret = ioctl(vfio->container_fd, VFIO_GET_API_VERSION);
	if(ret != VFIO_API_VERSION){
		ret = -ENOTSUP;
		#ifdef ADVANCED_PCI_DBG

		#endif
		goto error;
	}

	ret = ioctl(vfio->container_fd, VFIO_CHECK_EXTENSION, VFIO_TYPE1_IOMMU);
	if(ret <= 0){
		ret = -ENOTSUP;
		#ifdef ADVANCED_PCI_DBG

		#endif
		goto error;
	}

	snprintf(path, sizeof(path), "/dev/vfio/%d", vfio->group_id);
	vfio->group_fd = open(path, O_RDWR | O_CLOEXEC);
	if(vfio->group_fd < 0){
		ret = -errno;
		#ifdef ADVANCED_PCI_DBG

		#endif
		goto error;
	}

	status.argsz = sizeof(status);
	ret = ioctl(vfio->group_fd, VFIO_GROUP_GET_STATUS, &status);
	if(ret < 0){
		ret = -errno;
		#ifdef ADVANCED_PCI_DBG

		#endif
		goto error;
	}
	if(!(status.flags & VFIO_GROUP_FLAGS_VIABLE)){
		ret = -EBUSY;
		#ifdef ADVANCED_PCI_DBG

		#endif
		goto error;
	}

	ret = ioctl(vfio->group_fd, VFIO_GROUP_SET_CONTAINER, &vfio->container_fd);
	if(ret < 0){
		ret = -errno;
		#ifdef ADVANCED_PCI_DBG

		#endif
		goto error;
	}
	vfio->group_attached = true;

	ret = ioctl(vfio->container_fd, VFIO_SET_IOMMU, VFIO_TYPE1_IOMMU);
	if(ret < 0){
		ret = -errno;
		#ifdef ADVANCED_PCI_DBG

		#endif
		goto error;
	}

	vfio->device_fd = ioctl(vfio->group_fd, VFIO_GROUP_GET_DEVICE_FD, bdf);
	if(vfio->device_fd < 0){
		ret = -errno;
		#ifdef ADVANCED_PCI_DBG

		#endif
		goto error;
	}

	vfio->device_info.argsz = sizeof(vfio->device_info);
	ret = ioctl(vfio->device_fd, VFIO_DEVICE_GET_INFO, &vfio->device_info);
	if(ret < 0){
		ret = -errno;
		#ifdef ADVANCED_PCI_DBG

		#endif
		goto error;
	}

	for(int i = 0; i < vfio->device_info.num_regions && i < VFIO_PCI_NUM_REGIONS; i++){
		/* check if every pci mmu region exists */
		ret = vfio_region_info(i, &vfio->regions[i]);
		if(ret){
			#ifdef ADVANCED_PCI_DBG

			#endif
			goto error;
		}
	}

	vfio->opened = true;
	return ret;

	error:
	/* in case of an error, close everything.. in the future I will develop a self recovery
	 * mechanism for reloading the vfio kernel module */
	vfio_close();
	return ret;
}

static int vfio_region_read(unsigned int index, u64 offset, void *buffer, size_t length){
	int ret = 0;
	int result = 0;
	struct vfio_region_info *region = NULL;

	if(buffer == NULL || index >= VFIO_PCI_NUM_REGIONS){
		ret = -EINVAL;
		#ifdef ADVANCED_PCI_DBG

		#endif
		return ret;
	}

	region = &director.vfio->regions[index];
	if(offset > region->size || length > region->size - offset){
		ret = -ERANGE;
		#ifdef ADVANCED_PCI_DBG

		#endif
		return ret;
	}

	result = pread(director.vfio->device_fd, buffer, length, region->offset + offset);
	if(result < 0){
		ret = -errno;
		#ifdef ADVANCED_PCI_DBG

		#endif
		return ret;
	}
	if(result != length){
		ret = -EIO;
		#ifdef ADVANCED_PCI_DBG

		#endif
		return ret;
	}
	return ret;
}

static int vfio_region_write(unsigned int index, u64 offset, const void *buffer, size_t length){
	int ret = 0;
	int result = 0;
	struct vfio_region_info *region = NULL;

	if(buffer == NULL || index >= VFIO_PCI_NUM_REGIONS){
		ret = -EINVAL;
		#ifdef ADVANCED_PCI_DBG

		#endif
		return ret;
	}

	region = &director.vfio->regions[index];
	if(offset > region->size || length > region->size - offset){
		ret = -ERANGE;
		#ifdef ADVANCED_PCI_DBG

		#endif
		return ret;
	}

	result = pwrite(director.vfio->device_fd, buffer, length, region->offset + offset);
	if(result < 0){
		ret = -errno;
		#ifdef ADVANCED_PCI_DBG

		#endif
		return ret;
	}
	if(result != length){
		ret = -EIO;
		#ifdef ADVANCED_PCI_DBG

		#endif
		return ret;
	}

	return ret;
}

static int pci_config_read(unsigned int where, void *value, size_t size){
	int ret = 0;

	if(where > PCI_CONFIG_SIZE || size > PCI_CONFIG_SIZE - where){
		ret = -EINVAL;
		#ifdef ADVANCED_PCI_DBG

		#endif
		return ret;
	}

	ret = vfio_region_read(VFIO_PCI_CONFIG_REGION_INDEX, where, value, size);
	return ret;
}

static int pci_config_write(unsigned int where, const void *value, size_t size){
	int ret = 0;

	if(where > PCI_CONFIG_SIZE || size > PCI_CONFIG_SIZE - where){
		ret = -EINVAL;
		#ifdef ADVANCED_PCI_DBG

		#endif
		return ret;
	}

	ret = vfio_region_write(VFIO_PCI_CONFIG_REGION_INDEX, where, value, size);
	return ret;
}

static void *vfio_map_bar(unsigned int bar){
	int ret = 0;
	int prot = 0;
	void *address = NULL;
	struct vfio_region_info *region = NULL;

	if(bar >= PCI_MAX_BARS){
		#ifdef ADVANCED_PCI_DBG

		#endif
		return NULL;
	}
	if(director.vfio->bars[bar] != MAP_FAILED){
		return director.vfio->bars[bar];
	}

	region = &director.vfio->regions[VFIO_PCI_BAR0_REGION_INDEX + bar];
	if(region->size == 0 || !(region->flags & VFIO_REGION_INFO_FLAG_MMAP)){
		return NULL;
	}

	if(region->flags & VFIO_REGION_INFO_FLAG_READ){
		prot |= PROT_READ;
	}
	if(region->flags & VFIO_REGION_INFO_FLAG_WRITE){
		prot |= PROT_WRITE;
	}

	address = mmap(NULL, region->size, prot, MAP_SHARED, director.vfio->device_fd, region->offset);
	if(address == MAP_FAILED){
		return NULL;
	}

	director.vfio->bars[bar] = address;
	director.vfio->bar_sizes[bar] = region->size;
	return address;
}

/*
 * pci_pcie_cap - get the saved PCIe capability offset
 * @dev: PCI device
 *
 * PCIe capability offset is calculated at PCI device initialization
 * time and saved in the data structure. This function returns saved
 * PCIe capability offset. Using this instead of pci_find_capability()
 * reduces unnecessary search in the PCI configuration space. If you
 * need to calculate PCIe capability offset from raw device for some
 * reasons, please use pci_find_capability() instead.
 */
static inline int pci_pcie_cap(struct pci_dev *pci_dev){
	int ret = 0;

	ret = pci_dev->pcie_cap;
	return ret;
}

/*
 * pci_is_pcie - check if the PCI device is PCI Express capable
 * @dev: PCI device
 *
 * Returns: true if the PCI device is PCI Express capable, false otherwise.
 */
static inline bool pci_is_pcie(struct pci_dev *pci_dev){
	return pci_pcie_cap(pci_dev);
}

/*
 * pcie_caps_reg - get the PCIe Capabilities Register
 * @dev: PCI device
 */
static inline u16 pcie_caps_reg(const struct pci_dev *pci_dev){
	return pci_dev->pcie_flags_reg;
}

/*
 * pci_pcie_type - get the PCIe device/port type
 * @dev: PCI device
 */
static inline int pci_pcie_type(const struct pci_dev *pci_dev){
	int ret = 0;

	ret = (pcie_caps_reg(pci_dev) & PCI_EXP_FLAGS_TYPE) >> 4;
	return ret;
}

static inline bool pcie_downstream_port(const struct pci_dev *pci_dev){
	int type = 0;

	type = pci_pcie_type(pci_dev);
	return type == PCI_EXP_TYPE_ROOT_PORT || type == PCI_EXP_TYPE_DOWNSTREAM || type == PCI_EXP_TYPE_PCIE_BRIDGE;
}

static inline int pcie_cap_version(const struct pci_dev *pci_dev){
	int ret = 0;

	ret = pcie_caps_reg(pci_dev) & PCI_EXP_FLAGS_VERS;
	return ret;
}

static bool pcie_cap_has_lnkctl(const struct pci_dev *pci_dev){
	int type = 0;

	type = pci_pcie_type(pci_dev);
	return type == PCI_EXP_TYPE_ENDPOINT || type == PCI_EXP_TYPE_LEG_END ||
	       type == PCI_EXP_TYPE_ROOT_PORT || type == PCI_EXP_TYPE_UPSTREAM ||
	       type == PCI_EXP_TYPE_DOWNSTREAM || type == PCI_EXP_TYPE_PCI_BRIDGE || type == PCI_EXP_TYPE_PCIE_BRIDGE;
}

static bool pcie_cap_has_lnkctl2(const struct pci_dev *pci_dev){
	return pcie_cap_has_lnkctl(pci_dev) && pcie_cap_version(pci_dev) > 1;
}

static inline bool pcie_cap_has_sltctl(const struct pci_dev *pci_dev){
	return pcie_downstream_port(pci_dev) && pcie_caps_reg(pci_dev) & PCI_EXP_FLAGS_SLOT;
}

static bool pcie_cap_has_rtctl(const struct pci_dev *pci_dev){
	int type = 0;

	type = pci_pcie_type(pci_dev);
	return type == PCI_EXP_TYPE_ROOT_PORT || type == PCI_EXP_TYPE_RC_EC;
}

static bool pcie_capability_reg_implemented(struct pci_dev *pci_dev, int pos){
	if (!pci_is_pcie(pci_dev)){
		return false;
	}
	switch (pos) {
	case PCI_EXP_FLAGS:
		return true;
	case PCI_EXP_DEVCAP:
	case PCI_EXP_DEVCTL:
	case PCI_EXP_DEVSTA:
		return true;
	case PCI_EXP_LNKCAP:
	case PCI_EXP_LNKCTL:
	case PCI_EXP_LNKSTA:
		return pcie_cap_has_lnkctl(pci_dev);
	case PCI_EXP_SLTCAP:
	case PCI_EXP_SLTCTL:
	case PCI_EXP_SLTSTA:
		return pcie_cap_has_sltctl(pci_dev);
	case PCI_EXP_RTCTL:
	case PCI_EXP_RTCAP:
	case PCI_EXP_RTSTA:
		return pcie_cap_has_rtctl(pci_dev);
	case PCI_EXP_DEVCAP2:
	case PCI_EXP_DEVCTL2:
		return pcie_cap_version(pci_dev) > 1;
	case PCI_EXP_LNKCAP2:
	case PCI_EXP_LNKCTL2:
	case PCI_EXP_LNKSTA2:
		return pcie_cap_has_lnkctl2(pci_dev);
	default:
		return false;
	}
}

static inline size_t page_align(size_t size){
	int ret = 0;
	size_t page_size = 0;

	page_size = director.page_size; /* sysconf(_SC_PAGESIZE); */
	ret = (size + page_size - 1) & ~(page_size - 1);
	return ret;
}

static u64 iova_alloc(size_t size){
	int ret = 0;
	u64 iova = 0;
	struct exsul_vfio *vfio = NULL;

	vfio = director.vfio;
	size = page_align(size);
	pthread_mutex_lock(&vfio->lock);
	iova = (vfio->next_iova + PCI_DMA_ALIGN - 1) & ~(PCI_DMA_ALIGN - 1);
	vfio->next_iova = iova + size;
	pthread_mutex_unlock(&vfio->lock);
	return iova;
}

static int vfio_dma_map(void *address, size_t size, u64 iova, bool owned, struct exsul_pci_dma **out){
	int ret = 0;
	uintptr_t end = 0;
	uintptr_t base = 0;
	size_t page_size = 0;
	struct exsul_pci_dma *dma = NULL;
	struct vfio_iommu_type1_dma_map map = {};

	if(address == NULL || size == 0 || out == NULL){
		ret = -EINVAL;
		#ifdef page_align

		#endif
		return ret;
	}

	page_size = director.page_size; /* sysconf(_SC_PAGESIZE); */
	base = (uintptr_t)address & ~(page_size - 1);
	end = ((uintptr_t)address + size + page_size - 1) & ~(page_size - 1);

	dma = calloc(1, sizeof(*dma));
	if(dma == NULL){
		ret = -ENOMEM;
		#ifdef ADVANCED_PCI_DBG

		#endif
		return ret;
	}

	dma->address = address;
	dma->map_address = (void *)base;
	dma->requested_size = size;
	dma->map_size = end - base;
	dma->iova = iova + ((uintptr_t)address - base);
	dma->map_iova = iova;
	dma->owned = owned;
	dma->references = 1;

	map.argsz = sizeof(map);
	map.flags = VFIO_DMA_MAP_FLAG_READ | VFIO_DMA_MAP_FLAG_WRITE;
	map.vaddr = base;
	map.iova = dma->map_iova;
	map.size = dma->map_size;

	ret = ioctl(director.vfio->container_fd, VFIO_IOMMU_MAP_DMA, &map);
	if(ret < 0){
		ret = -errno;
		#ifdef ADVANCED_PCI_DBG

		#endif
		free(dma);
		return ret;
	}

	pthread_mutex_lock(&director.vfio->lock);
	dma->next = director.vfio->dma;
	director.vfio->dma = dma;
	pthread_mutex_unlock(&director.vfio->lock);
	*out = dma;
	return ret;
}

static void *vfio_irq_thread(void *argument){
	int ret = 0;
	int length = 0;
	u64 counter = 0;
	struct exsul_pci_irq *irq = NULL;
	struct pollfd poll_fd = {};
	irqreturn_t result = IRQ_NONE;

	irq = argument;
	poll_fd.fd = irq->event_fd;
	poll_fd.events = POLLIN;
	while(!atomic_load_explicit(&irq->stop, memory_order_acquire)){
		ret = poll(&poll_fd, 1, 100);
		if(ret <= 0){
			continue;
		}
		length = read(irq->event_fd, &counter, sizeof(counter));
		if(length != sizeof(counter)){
			continue;
		}

		result = IRQ_WAKE_THREAD;
		if(irq->handler != NULL){
			result = irq->handler(irq->linux_irq, irq->dev_id);
		}
		if(result == IRQ_WAKE_THREAD && irq->thread_fn != NULL){
			irq->thread_fn(irq->linux_irq, irq->dev_id);
		}
	}
	return NULL;
}

static int vfio_irq_bind(unsigned int vector, irq_handler_t handler, irq_handler_t thread_fn, void *dev_id){
	int ret = 0;
	int size = 0;
	struct vfio_irq_set *set = NULL;
	struct exsul_pci_irq *irq = NULL;

	if(vector >= director.vfio->irq_count || vector >= PCI_MAX_IRQS){
		ret = -EINVAL;
		#ifdef ADVANCED_PCI_DBG

		#endif
		return ret;
	}

	irq = &director.vfio->irqs[vector];
	if(irq->started){
		ret = -EBUSY;
		#ifdef ADVANCED_PCI_DBG

		#endif
		return ret;
	}

	irq->event_fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
	if(irq->event_fd < 0){
		ret = -errno;
		#ifdef ADVANCED_PCI_DBG

		#endif
		return ret;
	}

	size = sizeof(*set) + sizeof(irq->event_fd);
	set = calloc(1, size);
	if(set == NULL){
		#ifdef ADVANCED_PCI_DBG

		#endif
		close(irq->event_fd);
		irq->event_fd = -1;
		ret = -ENOMEM;
		return ret;
	}

	set->argsz = size;
	set->flags = VFIO_IRQ_SET_DATA_EVENTFD | VFIO_IRQ_SET_ACTION_TRIGGER;
	set->index = director.vfio->irq_index;
	set->start = vector;
	set->count = 1;
	memcpy(set->data, &irq->event_fd, sizeof(irq->event_fd));

	ret = ioctl(director.vfio->device_fd, VFIO_DEVICE_SET_IRQS, set);
	free(set);
	if(ret < 0){
		ret = -errno;
		#ifdef ADVANCED_PCI_DBG

		#endif
		close(irq->event_fd);
		irq->event_fd = -1;
		return ret;
	}

	irq->linux_irq = vector + 1;
	irq->vector = vector;
	irq->handler = handler;
	irq->thread_fn = thread_fn;
	irq->dev_id = dev_id;
	atomic_store(&irq->stop, false);
	ret = pthread_create(&irq->thread, NULL, vfio_irq_thread, irq);
	if(ret){
		#ifdef ADVANCED_PCI_DBG

		#endif
		close(irq->event_fd);
		irq->event_fd = -1;
		return ret;
	}
	irq->started = true;
	return ret;
}

static int vfio_irq_select(unsigned int min_vectors, unsigned int max_vectors, unsigned int flags){
	int ret = 0;
	int count = 0;
	int indexes[3] = {};
	struct vfio_irq_info info = {};

	if(flags & PCI_IRQ_MSIX){
		indexes[count++] = VFIO_PCI_MSIX_IRQ_INDEX;
	}
	if(flags & PCI_IRQ_MSI){
		indexes[count++] = VFIO_PCI_MSI_IRQ_INDEX;
	}
	if(flags & PCI_IRQ_LEGACY){
		indexes[count++] = VFIO_PCI_INTX_IRQ_INDEX;
	}

	for(int i = 0; i < count; i++){
		memset(&info, 0, sizeof(info));
		info.argsz = sizeof(info);
		info.index = indexes[i];
		ret = ioctl(director.vfio->device_fd, VFIO_DEVICE_GET_IRQ_INFO, &info);
		if(ret < 0 || !(info.flags & VFIO_IRQ_INFO_EVENTFD)){
			#ifdef ADVANCED_PCI_DBG

			#endif
			continue;
		}
		if(info.count < min_vectors){
			#ifdef ADVANCED_PCI_DBG

			#endif
			continue;
		}

		director.vfio->irq_index = indexes[i];
		director.vfio->irq_count = info.count < max_vectors ? info.count : max_vectors;
		if(director.vfio->irq_count > PCI_MAX_IRQS){
			director.vfio->irq_count = PCI_MAX_IRQS;
		}
		ret = director.vfio->irq_count;
		return ret;
	}
	ret = -ENOSPC;
	return ret;
}

static dma_addr_t dma_map_single_attrs(struct device *dev, void *address, size_t size, enum dma_data_direction direction, unsigned long attrs){
	int ret = 0;
	u64 iova = 0;
	struct exsul_pci_dma *dma = NULL;

	iova = iova_alloc(size + PCI_DMA_ALIGN);
	ret = vfio_dma_map(address, size, iova, false, &dma);
	if(ret){
		ret = DMA_MAPPING_ERROR;
		return ret;
	}
	return dma->iova;
}

static void dma_unmap_single_attrs(struct device *dev, dma_addr_t address, size_t size, enum dma_data_direction direction, unsigned long attrs){
	int ret = 0;
	struct exsul_pci_dma *dma = NULL;

	pthread_mutex_lock(&director.vfio->lock);
	for(dma = director.vfio->dma; dma != NULL; dma = dma->next){
		if(dma->iova == address){
			break;
		}
	}
	pthread_mutex_unlock(&director.vfio->lock);
	if(dma != NULL){
		vfio_dma_unmap(dma);
	}
}

static int dma_mapping_error(struct device *dev, dma_addr_t address){
	int ret = 0;

	if(address == DMA_MAPPING_ERROR){
		ret = true;
	}
	return ret;
}

static bool pci_id_matches(const struct pci_device_id *id, u32 vendor, u32 device, u32 subvendor, u32 subdevice, u32 class){
	int ret = 0;

	if(id->vendor != PCI_ANY_ID && id->vendor != vendor){
		ret = false;
		return ret;
	}
	if(id->device != PCI_ANY_ID && id->device != device){
		ret = false;
		return ret;
	}
	if(id->subvendor != PCI_ANY_ID && id->subvendor != subvendor){
		ret = false;
		return ret;
	}
	if(id->subdevice != PCI_ANY_ID && id->subdevice != subdevice){
		ret = false;
		return ret;
	}
	if(((id->class ^ class) & id->class_mask) != 0){
		ret = false;
		return ret;
	}
	ret = true;
	return ret;
}

static const struct pci_device_id *pci_match_id(const struct pci_device_id *table, u32 vendor, u32 device, u32 subvendor, u32 subdevice, u32 class){
	int ret = 0;
	const struct pci_device_id *id = NULL;

	if(table == NULL){
		return NULL;
	}

	for(id = table; id->vendor != 0 || id->device != 0 || id->subvendor != 0 || id->subdevice != 0 || id->class != 0 || id->class_mask != 0 || id->driver_data != NULL; id++){
		ret = pci_id_matches(id, vendor, device, subvendor, subdevice, class);
		if(ret){
			return id;
		}
	}
	return NULL;
}

static int pci_read_identity(const char *bdf, u32 *vendor, u32 *device, u32 *subvendor, u32 *subdevice, u32 *class){
	int ret = 0;
	char path[PATH_MAX] = {};

	READ_PCI_VALUE("vendor", vendor);
	READ_PCI_VALUE("device", device);
	READ_PCI_VALUE("subsystem_vendor", subvendor);
	READ_PCI_VALUE("subsystem_device", subdevice);
	READ_PCI_VALUE("class", class);
	return ret;
}

static int pci_prepare_device(struct pci_dev *pci_dev, const char *bdf){
	int ret = 0;
	u8  rev = 0;
	u32 class = 0;
	u32 vendor = 0;
	u32 device = 0;
	u32 subvendor = 0;
	u32 subdevice = 0;
	u64 *dma_mask = NULL;
	struct device *dev = NULL;
	struct resource *pci_resource = NULL;

	ret = pci_read_identity(bdf, &vendor, &device, &subvendor, &subdevice, &class);
	if(ret){
		return ret;
	}
	ret = pci_config_read(PCI_REVISION_ID, &rev, sizeof(rev));
	if(ret){
		return ret;
	}
	pci_dev->vendor = vendor;
	pci_dev->device = device;
	pci_dev->subsystem_vendor = subvendor;
	pci_dev->subsystem_device = subdevice;
	pci_dev->class = class;
	pci_dev->revision = rev;
	pci_dev->irq = 1;

	dma_mask = calloc(1, sizeof(*dma_mask));
	if(dma_mask == NULL){
		ret = -ENOMEM;
		return ret;
	}
	*dma_mask = DMA_BIT_MASK(64);
	dev = &pci_dev->dev;
	dev->dma_mask = dma_mask;
	dev->coherent_dma_mask = *dma_mask;

	for(int bar = 0; bar < PCI_MAX_BARS; bar++){
		pci_dev->resource[bar].start = 0;
		if(director.vfio->regions[VFIO_PCI_BAR0_REGION_INDEX + bar].size > 0){
			pci_resource = &pci_dev->resource[bar];
			pci_resource->end = director.vfio->regions[VFIO_PCI_BAR0_REGION_INDEX + bar].size - 1;
		}else{
			pci_resource = &pci_dev->resource[bar];
			pci_resource->end = 0;
		}
		pci_resource = &pci_dev->resource[bar];
		pci_resource->flags = IORESOURCE_MEM;
	}
	return ret;
}

/*
 * pci_rebar_find_pos - find position of resize control reg for BAR
 * @pdev: PCI device
 * @bar: BAR to find
 *
 * Helper to find the position of the control register for a BAR.
 *
 * Return:
 * * %-ENOTSUPP if resizable BARs are not supported at all,
 * * %-ENOENT if no control register for the BAR could be found.
 */
static int pci_rebar_find_pos(struct pci_dev *pci_dev, int bar){
	int ret = 0;
	int pos = 0;
	int nbars = 0;
	int bar_idx = 0;
	u32 ctrl = 0;

	pos = pci_dev->rebar_cap;
	if (pos == 0){
		ret = -ENOTSUPP;
		return ret;
	}
	pci_read_config_dword(pci_dev, pos + PCI_REBAR_CTRL, &ctrl);
	nbars = FIELD_GET(PCI_REBAR_CTRL_NBAR_MASK, ctrl);
	for (int i = 0; i < nbars; i++, pos += 8) {
		pci_read_config_dword(pci_dev, pos + PCI_REBAR_CTRL, &ctrl);
		bar_idx = FIELD_GET(PCI_REBAR_CTRL_BAR_IDX, ctrl);
		if (bar_idx == bar){
			return pos;
		}
	}
	ret = -ENOENT;
	return ret;
}

PCI_OP_READ(byte, u8, 1)
PCI_OP_READ(word, u16, 2)
PCI_OP_READ(dword, u32, 4)
PCI_OP_WRITE(byte, u8, 1)
PCI_OP_WRITE(word, u16, 2)
PCI_OP_WRITE(dword, u32, 4)

static u8 __pci_find_next_ht_cap(struct pci_dev *pci_dev, u8 pos, int ht_cap){
	int ret = 0;
	int rc = 0;
	u8 cap = 0;
	u8 mask = 0;

	if (ht_cap == HT_CAPTYPE_SLAVE || ht_cap == HT_CAPTYPE_HOST){
		mask = HT_3BIT_CAP_MASK;
	}else{
		mask = HT_5BIT_CAP_MASK;
	}
	pos = PCI_FIND_NEXT_CAP(pci_bus_read_config, pos, PCI_CAP_ID_HT, NULL, pci_dev->bus, pci_dev->devfn);
	while (pos) {
		rc = pci_read_config_byte(pci_dev, pos + 3, &cap);
		if (rc != PCIBIOS_SUCCESSFUL){
			return ret;
		}
		if ((cap & mask) == ht_cap){
			ret = pos;
			return ret;
		}
		pos = PCI_FIND_NEXT_CAP(pci_bus_read_config, pos + PCI_CAP_LIST_NEXT, PCI_CAP_ID_HT, NULL, pci_dev->bus, pci_dev->devfn);
	}
	return ret;
}

static struct pci_dev *__pci_get_subsys(unsigned int vendor, unsigned int device, unsigned int ss_vendor, unsigned int ss_device, struct pci_dev *from, enum pci_search_direction dir){
	return NULL;
}

static void __pci_set_master(struct pci_dev *pci_dev, bool enable){
	u16 cmd = 0;
	u16 old_cmd = 0;

	pci_read_config_word(pci_dev, PCI_COMMAND, &old_cmd);
	if (enable == true){
		cmd = old_cmd | PCI_COMMAND_MASTER;
	}else{
		cmd = old_cmd & ~PCI_COMMAND_MASTER;
	}
	if (cmd != old_cmd) {
		pci_write_config_word(pci_dev, PCI_COMMAND, cmd);
	}
	pci_dev->is_busmaster = enable;
}

/*
 * pcibios_set_master - enable PCI bus-mastering for device dev
 * @dev: the PCI device to enable
 *
 * Enables PCI bus-mastering for the device.  This is the default
 * implementation.  Architecture specific implementations can override
 * this if necessary.
 */
static void pcibios_set_master(struct pci_dev *pci_dev){
	u8 lat = 0;
	int pcibios_max_latency = 255;

	pci_read_config_byte(pci_dev, PCI_LATENCY_TIMER, &lat);
	if (lat < 16){
		lat = (64 <= pcibios_max_latency) ? 64 : pcibios_max_latency;
	}else if (lat > pcibios_max_latency){
		lat = pcibios_max_latency;
	}else{
		return;
	}
	pci_write_config_byte(pci_dev, PCI_LATENCY_TIMER, lat);
}

static inline u8 readb(const volatile void __iomem *address){
	u8 value = 0;

	__atomic_thread_fence(__ATOMIC_SEQ_CST);
	value = *(const volatile u8 *)address;
	__atomic_thread_fence(__ATOMIC_SEQ_CST);
	return value;
}

static inline u16 readw(const volatile void __iomem *address){
	u16 value = 0;

	__atomic_thread_fence(__ATOMIC_SEQ_CST);
	value = *(const volatile u16 *)address;
	__atomic_thread_fence(__ATOMIC_SEQ_CST);

	#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	value = __builtin_bswap16(value);
	#endif
	return value;
}

static inline u32 readl(const volatile void __iomem *address){
	u32 value = 0;

	__atomic_thread_fence(__ATOMIC_SEQ_CST);
	value = *(const volatile u32 *)address;
	__atomic_thread_fence(__ATOMIC_SEQ_CST);

	#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	value = __builtin_bswap32(value);
	#endif
	return value;
}

static inline void writeb(u8 value, volatile void __iomem *address){
	__atomic_thread_fence(__ATOMIC_SEQ_CST);
	*(volatile u8 *)address = value;
	__atomic_thread_fence(__ATOMIC_SEQ_CST);
}

static inline void writew(u16 value, volatile void __iomem *address){
	#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	value = __builtin_bswap16(value);
	#endif

	__atomic_thread_fence(__ATOMIC_SEQ_CST);
	*(volatile u16 *)address = value;
	__atomic_thread_fence(__ATOMIC_SEQ_CST);
}

static inline void writel(u32 value, volatile void __iomem *address){
	#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	value = __builtin_bswap32(value);
	#endif

	__atomic_thread_fence(__ATOMIC_SEQ_CST);
	*(volatile u32 *)address = value;
	__atomic_thread_fence(__ATOMIC_SEQ_CST);
}
#endif
