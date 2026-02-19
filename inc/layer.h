#ifndef __LAYER_H__
#define __LAYER_H__

#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <asm-generic/param.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <stdarg.h>
#include <errno.h>
#include <signal.h>
#include <elf.h>
#include <fcntl.h>
#include <libusb-1.0/libusb.h>
#include <linux/const.h>
#include <linux/if_ether.h>
#include <linux/nl80211.h>
#include <linux/swab.h>
#include <linux/types.h>
#include <linux/usb/ch9.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/random.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <stdatomic.h>
#include <netinet/ether.h>

#if __BYTE_ORDER == __LITTLE_ENDIAN
#include <linux/byteorder/little_endian.h>
#else
#include <linux/byteorder/big_endian.h>
#endif

#if UINTPTR_MAX == 0xffffffff
        #define Elf(field)      Elf32_##field
        #define BITS_PER_LONG   32
        #define RAND_BASE_ADDR  unsigned int
#elif UINTPTR_MAX == 0xffffffffffffffff
        #define Elf(field)      Elf64_##field
        #define BITS_PER_LONG   64
        #define RAND_BASE_ADDR  unsigned long
#else
        #error NO UINTPTR???
#endif

#include "utils.h"

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

#define s8 int8_t
#define s16 int16_t
#define s32 int32_t
#define s64 int64_t

#define __u8 u8
#define __u16 u16
#define __u32 u32
#define __u64 u64

#define __s8 s8
#define __s16 s16
#define __s32 s32
#define __s64 s64

/* define the number of maximun lkm which can be loaded in memory for one istance of this application */
#define MAX_ALLOCATED_LKM_NUMBER 12

/* Hard limit */
#define USB_MAXENDPOINTS	30
/* this maximum is arbitrary */
#define USB_MAXINTERFACES	32
#define USB_MAXIADS		(USB_MAXINTERFACES/2)
#define USB_MAX_ENDPOINT_NUM 12

#define TIMER_CPUMASK		0x0003FFFF
#define TIMER_MIGRATING		0x00040000
#define TIMER_BASEMASK		(TIMER_CPUMASK | TIMER_MIGRATING)
#define TIMER_DEFERRABLE	0x00080000
#define TIMER_PINNED		0x00100000
#define TIMER_IRQSAFE		0x00200000
#define TIMER_INIT_FLAGS	(TIMER_DEFERRABLE | TIMER_PINNED | TIMER_IRQSAFE)
#define TIMER_ARRAYSHIFT	22
#define TIMER_ARRAYMASK		0xFFC00000

/* for knowing the vid/pid of the usb device associated to a loaded lkm, we can read the data pointed by the symbol 'usb_device_id' */
#define USB_TABLE_SYM_NAME	"__mod_device_table__usb__"

enum {
	DUMP_PREFIX_NONE,
	DUMP_PREFIX_ADDRESS,
	DUMP_PREFIX_OFFSET
};

enum usb_interface_condition {
	USB_INTERFACE_UNBOUND = 0,
	USB_INTERFACE_BINDING,
	USB_INTERFACE_BOUND,
	USB_INTERFACE_UNBINDING,
};

enum mod_mem_type {
        MOD_TEXT                = 0,
        MOD_DATA                = 1,
        MOD_RODATA              = 2,
        MOD_RO_AFTER_INIT       = 3,
        MOD_INIT_TEXT           = 4,
        MOD_INIT_DATA           = 5,
        MOD_INIT_RODATA         = 6,

        MOD_MEM_NUM_TYPES       = 7,
        MOD_INVALID             = -1,
};

enum insn_mode {
        INSN_MODE_32,
        INSN_MODE_64,
        INSN_MODE_KERN,
        INSN_NUM_MODES,
};

enum module_state {
        MODULE_STATE_LIVE,
        MODULE_STATE_COMING,
        MODULE_STATE_GOING,
        MODULE_STATE_UNFORMED,
};

/*
 * Flags available for kernel_param_ops
 *
 * NOARG - the parameter allows for no argument (foo instead of foo=1)
 */
enum {
	KERNEL_PARAM_OPS_FL_NOARG = (1 << 0)
};

/**
 * enum irqreturn - irqreturn type values
 * @IRQ_NONE:		interrupt was not from this device or was not handled
 * @IRQ_HANDLED:	interrupt was handled by this device
 * @IRQ_WAKE_THREAD:	handler requests to wake the handler thread
 */
enum irqreturn {
	IRQ_NONE		= (0 << 0),
	IRQ_HANDLED		= (1 << 0),
	IRQ_WAKE_THREAD		= (1 << 1),
};

struct device;
struct pci_dev;
struct net_device;
struct pci_driver;

/* unused function callback used in IRQ context with sdio/pcie devices */
typedef enum irqreturn irqreturn_t;
typedef irqreturn_t (*irq_handler_t)(int, void *);

/* function pointer used for the sort function */
typedef void (*swap_r_func_t)(void *a, void *b, int size, const void *priv);
typedef void (*swap_func_t)(void *a, void *b, int size);

typedef int (*cmp_r_func_t)(const void *a, const void *b, const void *priv);
typedef int (*cmp_func_t)(const void *a, const void *b);

typedef struct {
	int counter;
} atomic_t;

struct completion {
	atomic_int done;
};

struct list_head {
        struct list_head *next;
        struct list_head *prev;
};

struct hlist_head {
	struct hlist_node *first;
};

struct hlist_node {
	struct hlist_node *next;
	struct hlist_node **pprev;
};

struct timer_list {
	struct hlist_node	entry;
	unsigned long		expires;
	void			(*function)(struct timer_list *);
	u32			flags;
};

struct sk_buff_head {
	/* These two members must be first. */
	struct sk_buff	*next;
	struct sk_buff	*prev;

	__u32		qlen;
};

/* Tasklets --- multithreaded analogue of BHs.

   This API is deprecated. Please consider using threaded IRQs instead:
   https://lore.kernel.org/lkml/20200716081538.2sivhkj4hcyrusem@linutronix.de

   Main feature differing them of generic softirqs: tasklet
   is running only on one CPU simultaneously.

   Main feature differing them of BHs: different tasklets
   may be run simultaneously on different CPUs.

   Properties:
   * If tasklet_schedule() is called, then tasklet is guaranteed
     to be executed on some cpu at least once after this.
   * If the tasklet is already scheduled, but its execution is still not
     started, it will be executed only once.
   * If this tasklet is already running on another CPU (or schedule is called
     from tasklet itself), it is rescheduled for later.
   * Tasklet is strictly serialized wrt itself, but not
     wrt another tasklets. If client needs some intertask synchronization,
     he makes it with spinlocks.
 */

struct tasklet_struct {
	struct tasklet_struct *next;
	unsigned long state;
	atomic_uint count;
	bool use_callback;
	union {
		void (*func)(unsigned long data);
		void (*callback)(struct tasklet_struct *t);
	};
	unsigned long data;
};

struct load_info {
	const char *name;
	struct module *mod;
	Elf(Ehdr) *hdr;
	unsigned long len;
	Elf(Shdr) *sechdrs;
	char *secstrings;
	char *strtab;
	unsigned long symoffs;
	unsigned long stroffs;
	unsigned long init_typeoffs;
	unsigned long core_typeoffs;
	bool sig_ok;
	unsigned long mod_kallsyms_init_off;
	struct {
		unsigned int sym;
		unsigned int str;
		unsigned int mod;
		unsigned int vers;
		unsigned int info;
		unsigned int pcpu;
	} index;
};

struct kernel_param {
	const char *name;
	struct module *mod;
};

struct firmware {
	size_t size;
	const u8 *data;

	/* firmware loader private fields */
	void *priv;
};

struct kernel_param_ops {
	/* How the ops should behave */
	unsigned int flags;
	/* Returns 0, or -errno.  arg is in kp->arg. */
	int (*set)(const char *val, const struct kernel_param *kp);
	/* Returns length written or -errno.  Buffer is 4k (ie. be short!) */
	int (*get)(char *buffer, const struct kernel_param *kp);
	/* Optional function to free kp->arg when module unloaded. */
	void (*free)(void *arg);
};

struct usb_device;

struct usb_device_id {
	/* which fields to match against? */
	__u16		match_flags;

	/* Used for product specific matches; range is inclusive */
	__u16		idVendor;
	__u16		idProduct;
	__u16		bcdDevice_lo;
	__u16		bcdDevice_hi;

	/* Used for device class matches */
	__u8		bDeviceClass;
	__u8		bDeviceSubClass;
	__u8		bDeviceProtocol;

	/* Used for interface class matches */
	__u8		bInterfaceClass;
	__u8		bInterfaceSubClass;
	__u8		bInterfaceProtocol;

	/* Used for vendor-specific interface matches */
	__u8		bInterfaceNumber;

	/* not matched against */
	unsigned long	driver_info __attribute__((aligned(sizeof(unsigned long))));
};


/**
 * struct usb_host_endpoint - host-side endpoint descriptor and queue
 * @desc: descriptor for this endpoint, wMaxPacketSize in native byteorder
 * @ss_ep_comp: SuperSpeed companion descriptor for this endpoint
 * @ssp_isoc_ep_comp: SuperSpeedPlus isoc companion descriptor for this endpoint
 * @urb_list: urbs queued to this endpoint; maintained by usbcore
 * @hcpriv: for use by HCD; typically holds hardware dma queue head (QH)
 *	with one or more transfer descriptors (TDs) per urb
 * @ep_dev: ep_device for sysfs info
 * @extra: descriptors following this endpoint in the configuration
 * @extralen: how many bytes of "extra" are valid
 * @enabled: URBs may be submitted to this endpoint
 * @streams: number of USB-3 streams allocated on the endpoint
 *
 * USB requests are always queued to a given endpoint, identified by a
 * descriptor within an active interface in a given USB configuration.
 */
struct usb_host_endpoint {
	struct usb_endpoint_descriptor		desc;
	struct usb_ss_ep_comp_descriptor	ss_ep_comp;
	struct usb_ssp_isoc_ep_comp_descriptor	ssp_isoc_ep_comp;
	struct list_head		urb_list;
	void				*hcpriv;
	struct ep_device		*ep_dev;	/* For sysfs info */

	unsigned char *extra;   /* Extra descriptors */
	int extralen;
	int enabled;
	int streams;
};

/* host-side wrapper for one interface setting's parsed descriptors */
struct usb_host_interface {
	struct usb_interface_descriptor	desc;

	int extralen;
	unsigned char *extra;   /* Extra descriptors */

	/* array of desc.bNumEndpoints endpoints associated with this
	 * interface setting.  these will be in no particular order.
	 */
	struct usb_host_endpoint *endpoint;

	char *string;		/* iInterface string, if present */
};

struct usb_interface {
	/* array of alternate settings for this interface,
	 * stored in no particular order */
	struct usb_host_interface *altsetting;

	struct usb_host_interface *cur_altsetting;	/* the currently
					 * active alternate setting */
	unsigned num_altsetting;	/* number of alternate settings */

	/* If there is an interface association descriptor then it will list
	 * the associated interfaces */
	struct usb_interface_assoc_descriptor *intf_assoc;

	int minor;			/* minor number this interface is
					 * bound to */
	enum usb_interface_condition condition;		/* state of binding */
	unsigned sysfs_files_created:1;	/* the sysfs attributes exist */
	unsigned ep_devs_created:1;	/* endpoint "devices" exist */
	unsigned unregistering:1;	/* unregistration is in progress */
	unsigned needs_remote_wakeup:1;	/* driver requires remote wakeup */
	unsigned needs_altsetting0:1;	/* switch to altsetting 0 is pending */
	unsigned needs_binding:1;	/* needs delayed unbind/rebind */
	unsigned resetting_device:1;	/* true: bandwidth alloc after reset */
};

/**
 * struct usb_host_config - representation of a device's configuration
 * @desc: the device's configuration descriptor.
 * @string: pointer to the cached version of the iConfiguration string, if
 *	present for this configuration.
 * @intf_assoc: list of any interface association descriptors in this config
 * @interface: array of pointers to usb_interface structures, one for each
 *	interface in the configuration.  The number of interfaces is stored
 *	in desc.bNumInterfaces.  These pointers are valid only while the
 *	configuration is active.
 * @intf_cache: array of pointers to usb_interface_cache structures, one
 *	for each interface in the configuration.  These structures exist
 *	for the entire life of the device.
 * @extra: pointer to buffer containing all extra descriptors associated
 *	with this configuration (those preceding the first interface
 *	descriptor).
 * @extralen: length of the extra descriptors buffer.
 *
 * USB devices may have multiple configurations, but only one can be active
 * at any time.  Each encapsulates a different operational environment;
 * for example, a dual-speed device would have separate configurations for
 * full-speed and high-speed operation.  The number of configurations
 * available is stored in the device descriptor as bNumConfigurations.
 *
 * A configuration can contain multiple interfaces.  Each corresponds to
 * a different function of the USB device, and all are available whenever
 * the configuration is active.  The USB standard says that interfaces
 * are supposed to be numbered from 0 to desc.bNumInterfaces-1, but a lot
 * of devices get this wrong.  In addition, the interface array is not
 * guaranteed to be sorted in numerical order.  Use usb_ifnum_to_if() to
 * look up an interface entry based on its number.
 *
 * Device drivers should not attempt to activate configurations.  The choice
 * of which configuration to install is a policy decision based on such
 * considerations as available power, functionality provided, and the user's
 * desires (expressed through userspace tools).  However, drivers can call
 * usb_reset_configuration() to reinitialize the current configuration and
 * all its interfaces.
 */
struct usb_host_config {
	struct usb_config_descriptor	desc;

	char *string;		/* iConfiguration string, if present */

	/* List of any Interface Association Descriptors in this
	 * configuration. */
	struct usb_interface_assoc_descriptor *intf_assoc[USB_MAXIADS];

	/* the interfaces associated with this configuration,
	 * stored in no particular order */
	struct usb_interface *interface[USB_MAXINTERFACES];

	/* Interface information available even when this is not the
	 * active configuration */
	struct usb_interface_cache *intf_cache[USB_MAXINTERFACES];

	unsigned char *extra;   /* Extra descriptors */
	int extralen;
};

struct usb_driver {
	const char *name;
	int (*probe) (struct usb_interface *intf, const struct usb_device_id *id);
	void (*disconnect) (struct usb_interface *intf);
};

struct module_memory {
	void *base;
	bool is_rox;
	unsigned int size;
};

struct mod_kallsyms {
	Elf(Sym) *symtab;
	unsigned int num_symtab;
	char *strtab;
	char *typetab;
};

struct internal_cb_table{
	unsigned char *function_name;
	void *function_ptr;
};

struct module_controller {
        char module_name[16];
        unsigned char *trampoline_area;
	int trampoline_used;
	int libusb_ret;
        struct module *mod;
	struct load_info info;
	struct internal_cb_table *internal_cb_table;
	/* libusb data */
	libusb_device_handle *usb_dev;

	/* data gathered when the usb device is attached to the host system */
	u8                                usb_interface_number;
	u8                                usb_endpoint_num;
	struct libusb_endpoint_descriptor usb_ep[USB_MAX_ENDPOINT_NUM];
	struct libusb_device_descriptor   usb_dev_desc;

	/* provide the support for loading multiple lkm, permitting thus to reuse the functions from every lkm loaded */
	u8 loaded_lkm_num;
	void *lkm_current_area[MAX_ALLOCATED_LKM_NUMBER];
};

extern struct module_controller director;

/* taken from 'https://android.googlesource.com/platform/external/mtools/+/8c780b311a17226862cbfd6ae6fafda2f09d50d7/strtonum.c#35' */
static unsigned long strtoul_with_range(const char *nptr, char **endptr,
					int base, unsigned long max) {
    unsigned long l = strtoul(nptr, endptr, base);
    if(l > max) {
	errno = ERANGE;
	return max;
    }
    return l;
}

static int try_consume_completion(struct completion *x){
    int v = atomic_load_explicit(&x->done, memory_order_acquire);
    while (v > 0) {
        if (atomic_compare_exchange_weak_explicit(
                &x->done, &v, v - 1,
                memory_order_acq_rel, memory_order_acquire)) {
            return 1;
        }
    }
    return 0;
}

/*
 * prng for emulating the 'get_random_uxx' function
 * taken from 'https://www.pcg-random.org/download.html'
*/
static u32 pcg32_random_r(void *arg){
	u32 rot = 0;
	u32 xorshifted = 0;
	struct pcg32_random {
		u64 state;
		u64 inc;
	};
	struct pcg32_random *rng = (struct pcg32_random *)arg;

	u64 oldstate = rng->state;
	rng->state = oldstate * 6364136223846793005ULL + (rng->inc|1);
	xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
	rot = oldstate >> 59u;
	return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

#include "module.h"
#include "skb.h"
#include "ieee80211.h"
#include "prototypes.h"

#endif
