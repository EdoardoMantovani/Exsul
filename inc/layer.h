#ifndef __LAYER_H__
#define __LAYER_H__

#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <asm-generic/param.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <alloca.h>
#include <stdarg.h>
#include <errno.h>
#include <signal.h>
#include <elf.h>
#include <fcntl.h>
#include <libusb-1.0/libusb.h>
#include <linux/udp.h>
#include <linux/in.h>
#include <linux/in6.h>
#include <linux/const.h>
#include <linux/if_ether.h>
#include <linux/nl80211.h>
#include <linux/swab.h>
#include <linux/types.h>
#include <linux/ethtool.h>
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
#include <dwarf.h>
#include <libdwarf/libdwarf.h>

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

#define be8  __be8
#define be16 __be16
#define be32 __be32
#define be64 __be64

#include <utils.h>
#include <jhash.h>
#include <struct_definition.h>

/* defines the maximun size of the buffer data which can be transferred from the user to the kernel stub */
#define MAX_DEBUGFS_SIZE	128

/* defines the maximun number of usb devices which can be present in the current kernel module */
#define MAX_USB_DEV_LIST	36

/* array size of the module memory allocated by the functions 'kmem_xxx' */
#define MAX_KMEM_ARRAY_SIZE	36

/* define the number of maximun lkm which can be loaded in memory for one istance of this application */
#define MAX_ALLOCATED_LKM_NUMBER 12

/* size for the sk_buff struct, usually is around 384 bytes */
#define SK_BUFF_STRUCT_SIZE	512

/* maximun number of files that can be opened by the kernel module */
#define MAX_FILE_IDX		32

#define NR_KMALLOC_TYPES	8
#define	NUMA_NO_NODE		(-1)

/* Hard limit */
#define USB_MAXENDPOINTS	30
/* this maximum is arbitrary */
#define USB_MAXINTERFACES	32
#define USB_MAXIADS		(USB_MAXINTERFACES/2)
#define USB_MAX_ENDPOINT_NUM	12

#define GFP_KERNEL		0
#define GFP_ATOMIC		0
#define __GFP_NOWARN		0
#define __GFP_ZERO		0

#define L1_CACHE_SHIFT		5
#define L1_CACHE_BYTES		(1 << L1_CACHE_SHIFT)

#define PAGE_SIZE		4096
#define SMP_CACHE_BYTES		L1_CACHE_BYTES

#define WHITEOUT_MODE		0
#define WHITEOUT_DEV		0

#define CSUM_MANGLED_0		((__force __sum16)0xffff)
#define TIMER_CPUMASK		0x0003FFFF
#define TIMER_MIGRATING		0x00040000
#define TIMER_BASEMASK		(TIMER_CPUMASK | TIMER_MIGRATING)
#define TIMER_DEFERRABLE	0x00080000
#define TIMER_PINNED		0x00100000
#define TIMER_IRQSAFE		0x00200000
#define TIMER_INIT_FLAGS	(TIMER_DEFERRABLE | TIMER_PINNED | TIMER_IRQSAFE)
#define TIMER_ARRAYSHIFT	22
#define TIMER_ARRAYMASK		0xFFC00000

#define WORK_STRUCT_FLAG_MASK	0xFUL
#define WORK_STRUCT_PENDING	0x1UL
#define WORK_STRUCT_RUNNING	0x2UL
#define WORK_STRUCT_DELAYED	0x4UL

#define WQ_FLAG_EXCLUSIVE	0x01
#define WQ_FLAG_WOKEN		0x02
#define WQ_FLAG_CUSTOM		0x04
#define WQ_FLAG_DONE		0x08
#define WQ_FLAG_PRIORITY	0x10
#define TASK_RUNNING		0x00000000
#define TASK_INTERRUPTIBLE	0x00000001
#define TASK_UNINTERRUPTIBLE	0x00000002
#define TASK_NORMAL		(TASK_INTERRUPTIBLE | TASK_UNINTERRUPTIBLE)

/* for knowing the vid/pid of the usb device associated to a loaded lkm, we can read the data pointed by the symbol 'usb_device_id' */
#define USB_TABLE_SYM_NAME	"__mod_device_table__usb__"

#define MAX_DRIVER_TO_USERSPACE_IO_SIZE	256
#define MAX_USERSPACE_TO_DRIVER_IO_SIZE	256

/* macro used for getting the current 'task_struct' */
#define current (current_thread_info())

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

/* callbacks used in the 'work_struct' */
typedef void (*work_func_t)(struct work_struct *work);

/* unused function callback used in IRQ context with sdio/pcie devices */
typedef enum irqreturn irqreturn_t;
typedef irqreturn_t (*irq_handler_t)(int, void *);

/* function pointer used for the sort function */
typedef void (*swap_r_func_t)(void *a, void *b, int size, const void *priv);
typedef void (*swap_func_t)(void *a, void *b, int size);

typedef int (*cmp_r_func_t)(const void *a, const void *b, const void *priv);
typedef int (*cmp_func_t)(const void *a, const void *b);

typedef unsigned long __bitwise netmem_ref;

struct wait_queue_entry;
typedef int (*wait_queue_func_t)(struct wait_queue_entry *wq_entry, unsigned mode, int flags, void *key);

struct wait_queue_head;
typedef struct wait_queue_head wait_queue_head_t;

typedef struct {
	int counter;
} atomic_t;

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

struct kobject {
	const char		*name;
	struct list_head	entry;
	struct kobject		*parent;
	struct kset		*kset;
	const struct kobj_type	*ktype;
	struct kernfs_node	*sd; /* sysfs directory entry */
	struct kref		kref;

	unsigned int state_initialized:1;
	unsigned int state_in_sysfs:1;
	unsigned int state_add_uevent_sent:1;
	unsigned int state_remove_uevent_sent:1;
	unsigned int uevent_suppress:1;

	/* 'CONFIG_DEBUG_KOBJECT_RELEASE' usually isn't enabled in most kernels... */
#ifdef CONFIG_DEBUG_KOBJECT_RELEASE
	struct delayed_work	release;
#endif
};

struct cdev {
	struct kobject kobj;
	struct module *owner;
	const struct file_operations *ops;
	struct list_head list;
	u32 dev;
	unsigned int count;
};

/*
 * A single wait-queue entry structure:
 */
struct wait_queue_entry {
	unsigned int		flags;
	void			*private;
	wait_queue_func_t	func;
	struct list_head	entry;
};

/* custom task_struct, this can be possible to use because the kernel programming style uses those structs as opaque objects */
struct task_struct {
	pthread_t tid;			/* pthread id associated with the task_struct, this is used for the coordination between the pthread and the kthread_xx API */
	pthread_mutex_t lock;		/* lock variable used for concurrency */
	pthread_cond_t  cond;		/* condition variable used for the thread synchronization */

	atomic_int  __state;		/* task_state, used for declaring if a task is running or not */
	atomic_bool started;		/* emulate "stopped state" until wake_up_process() */
	atomic_bool should_stop;	/* used for indicating that the pthread had been killed or is almost killed */

	int (*threadfn)(void *data);	/* pthread callback */
	void *data;			/* pthread callback arg */

	int exit_code;			/* exit code of the pthread callback */
	char name[64];			/* thread name */
};

struct wait_queue_head {
	int32_t			lock;
	struct list_head	head;
};

struct completion {
        atomic_int done;
        wait_queue_head_t wait;
};

struct timer_list {
	struct hlist_node	entry;
	unsigned long		expires;
	void			(*function)(struct timer_list *);
	u32			flags;
};

struct rb_node {
	unsigned long  __rb_parent_color;
	struct rb_node *rb_right;
	struct rb_node *rb_left;
} __attribute__((aligned(sizeof(long))));
/* The alignment might seem pointless, but allegedly CRIS needs it */

struct rb_root {
	struct rb_node *rb_node;
};

/*
 * Leftmost-cached rbtrees.
 *
 * We do not cache the rightmost node based on footprint
 * size vs number of potential users that could benefit
 * from O(1) rb_last(). Just not worth it, users that want
 * this feature can always implement the logic explicitly.
 * Furthermore, users that want to cache both pointers may
 * find it a bit asymmetric, but that's ok.
 */
struct rb_root_cached {
	struct rb_root rb_root;
	struct rb_node *rb_leftmost;
};

/**
 * struct callback_head - callback structure for use with RCU and task_work
 * @next: next update requests in a list
 * @func: actual update function to call after the grace period.
 *
 * The struct is aligned to size of pointer. On most architectures it happens
 * naturally due ABI requirements, but some architectures (like CRIS) have
 * weird ABI and we need to ask it explicitly.
 *
 * The alignment is required to guarantee that bit 0 of @next will be
 * clear under normal conditions -- as long as we use call_rcu() or
 * call_srcu() to queue the callback.
 *
 * This guarantee is important for few reasons:
 *  - future call_rcu_lazy() will make use of lower bits in the pointer;
 *  - the structure shares storage space in struct page with @compound_head,
 *    which encode PageTail() in bit 0. The guarantee is needed to avoid
 *    false-positive PageTail().
 */
struct callback_head {
	struct callback_head *next;
	void (*func)(struct callback_head *head);
} __attribute__((aligned(sizeof(void *))));
#define rcu_head callback_head

struct work_struct {
	atomic_long data;
	struct list_head entry;
	work_func_t func;
};

struct delayed_work {
	struct work_struct work;
	struct timespec expires;
};

struct workqueue_struct {
	char name[64];

	pthread_mutex_t lock;
	pthread_cond_t  cond;
	clockid_t       clock_id;

	struct list_head work_list;
	struct list_head delayed_list;

	int stop;
	int nr_active;

	pthread_t thread;
};

struct usb_anchor {
	struct list_head urb_list;
	/* don't count the other fields for now */
};

struct urb {
	/* the order of the first elements isn't important, luckily */
	struct kref kref;		/* reference count of the URB */
	int unlinked;			/* unlink error code */
	void *hcpriv;			/* private data for host controller */
	atomic_uint use_count;		/* concurrent submissions counter */
	atomic_uint reject;		/* submissions will fail */

	struct list_head urb_list;	/* list head for use by the urb's current owner */
	struct list_head anchor_list;	/* the URB may be anchored */
	struct usb_anchor *anchor;
	/* ... */
};

/**
 * struct wakeup_source - Representation of wakeup sources
 *
 * @name: Name of the wakeup source
 * @id: Wakeup source id
 * @entry: Wakeup source list entry
 * @lock: Wakeup source lock
 * @wakeirq: Optional device specific wakeirq
 * @timer: Wakeup timer list
 * @timer_expires: Wakeup timer expiration
 * @total_time: Total time this wakeup source has been active.
 * @max_time: Maximum time this wakeup source has been continuously active.
 * @last_time: Monotonic clock when the wakeup source's was touched last time.
 * @prevent_sleep_time: Total time this source has been preventing autosleep.
 * @event_count: Number of signaled wakeup events.
 * @active_count: Number of times the wakeup source was activated.
 * @relax_count: Number of times the wakeup source was deactivated.
 * @expire_count: Number of times the wakeup source's timeout has expired.
 * @wakeup_count: Number of times the wakeup source might abort suspend.
 * @dev: Struct device for sysfs statistics about the wakeup source.
 * @active: Status of the wakeup source.
 * @autosleep_enabled: Autosleep is active, so update @prevent_sleep_time.
 */
struct wakeup_source {
	const char 		*name;
	int			id;
	struct list_head	entry;
	u32			lock;
	struct wake_irq		*wakeirq;
	struct timer_list	timer;
	unsigned long		timer_expires;
	ktime_t total_time;
	ktime_t max_time;
	ktime_t last_time;
	ktime_t start_prevent_time;
	ktime_t prevent_sleep_time;
	unsigned long		event_count;
	unsigned long		active_count;
	unsigned long		relax_count;
	unsigned long		expire_count;
	unsigned long		wakeup_count;
	struct device		*dev;
	bool			active:1;
	bool			autosleep_enabled:1;
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

/* this represent the global tasklet queue */
struct tasklet_head {
	struct tasklet_struct *head;
	struct tasklet_struct **tail;
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
	const struct kernel_param_ops *ops;
	const u16 perm;
	s8 level;
	u8 flags;
	union {
		void *arg;
		const struct kparam_string *str;
		const struct kparam_array *arr;
	};
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

struct seq_operations {
	void * (*start) (struct seq_file *m, int *pos);
	void (*stop) (struct seq_file *m, void *v);
	void * (*next) (struct seq_file *m, void *v, int *pos);
	int (*show) (struct seq_file *m, void *v);
};

struct seq_file {
	char *buf;
	size_t size;
	size_t from;
	size_t count;
	size_t pad_until;
	int index;
	int read_pos;
	pthread_mutex_t lock;
	const struct seq_operations *op;
	int poll_event;
	const struct file *file;
	void *private;
};

struct seq_list {
	int seq_list_entries;
	struct seq_file *next;
	struct seq_file *prev;
};

/* 'struct file' transation layer */
struct file_layer {
	/* play with the file pointer, associate a fd with an file pointer used by the kernel module */
	void *original_file_ptr;
	int actual_fd;
	int actual_offset;
};

struct file_list {
	int file_list_entries;
	struct file_layer *elem;
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

struct usb_dev_tuple {
	__u16 usb_pid;
	__u16 usb_vid;
	bool is_used;
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
	struct list_head			urb_list;
	void					*hcpriv;
	struct ep_device			*ep_dev;	/* For sysfs info */

	unsigned char				*extra;   /* Extra descriptors */
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

struct internal_cb_table {
	unsigned char *function_name;
	void *function_ptr;
};

struct module_controller;
extern struct module_controller director;

/**
 * refcount_set - set a refcount's value
 * @r: the refcount
 * @n: value to which the refcount will be set
 */
static inline void refcount_set(refcount_t *r, int n){
	atomic_store(&r->refs, n);
}

/**
 * refcount_read - get a refcount's value
 * @r: the refcount
 *
 * Return: the refcount's value
 */
static inline unsigned int refcount_read(const refcount_t *r){
	return atomic_load(&r->refs);
}

/**
 * INIT_LIST_HEAD - Initialize a list_head structure
 * @list: list_head structure to be initialized.
 *
 * Initializes the list_head to point to itself.  If it is a list header,
 * the result is an empty list.
 */
static inline void INIT_LIST_HEAD(struct list_head *list){
	WRITE_ONCE(list->next, list);
	WRITE_ONCE(list->prev, list);
}

/**
 * list_is_first -- tests whether @list is the first entry in list @head
 * @list: the entry to test
 * @head: the head of the list
 */
static inline int list_is_first(const struct list_head *list, const struct list_head *head){
	return list->prev == head;
}

/**
 * list_is_last - tests whether @list is the last entry in list @head
 * @list: the entry to test
 * @head: the head of the list
 */
static inline int list_is_last(const struct list_head *list, const struct list_head *head){
	return list->next == head;
}

/**
 * list_is_head - tests whether @list is the list @head
 * @list: the entry to test
 * @head: the head of the list
 */
static inline int list_is_head(const struct list_head *list, const struct list_head *head){
	return list == head;
}

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static inline int list_empty(const struct list_head *head){
	return READ_ONCE(head->next) == head;
}

/*
 * Performs list corruption checks before __list_add(). Returns false if a
 * corruption is detected, true otherwise.
 *
 * With CONFIG_LIST_HARDENED only, performs minimal list integrity checking
 * inline to catch non-faulting corruptions, and only if a corruption is
 * detected calls the reporting function __list_add_valid_or_report().
 */
static inline bool __list_add_valid(struct list_head *new, struct list_head *prev, struct list_head *next){
	bool ret = true;

	if (likely(next->prev == prev && prev->next == next && new != prev && new != next)){
		return ret;
	}else{
		ret = false;
		return false;
	}
}

/*
 * Performs list corruption checks before __list_del_entry(). Returns false if a
 * corruption is detected, true otherwise.
 *
 * With CONFIG_LIST_HARDENED only, performs minimal list integrity checking
 * inline to catch non-faulting corruptions, and only if a corruption is
 * detected calls the reporting function __list_del_entry_valid_or_report().
 */
static inline bool __list_del_entry_valid(struct list_head *entry){
	bool ret = true;
	struct list_head *prev = entry->prev;
	struct list_head *next = entry->next;

	if (likely(prev->next == entry && next->prev == entry)){
		return ret;
	}else{
		ret = false;
		return ret;
	}
}

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_add(struct list_head *new, struct list_head *prev, struct list_head *next){
	if (!__list_add_valid(new, prev, next)){
		return;
	}

	next->prev = new;
	new->next = next;
	new->prev = prev;
	WRITE_ONCE(prev->next, new);
}

/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static inline void list_add(struct list_head *new, struct list_head *head){
	__list_add(new, head, head->next);
}

/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void list_add_tail(struct list_head *new, struct list_head *head){
	__list_add(new, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_del(struct list_head * prev, struct list_head * next){
	next->prev = prev;
	WRITE_ONCE(prev->next, next);
}

/*
 * Delete a list entry and clear the 'prev' pointer.
 *
 * This is a special-purpose list clearing method used in the networking code
 * for lists allocated as per-cpu, where we don't want to incur the extra
 * WRITE_ONCE() overhead of a regular list_del_init(). The code that uses this
 * needs to check the node 'prev' pointer instead of calling list_empty().
 */
static inline void __list_del_clearprev(struct list_head *entry){
	__list_del(entry->prev, entry->next);
	entry->prev = NULL;
}

static inline void __list_del_entry(struct list_head *entry){
	if (!__list_del_entry_valid(entry)){
		return;
	}
	__list_del(entry->prev, entry->next);
}

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty() on entry does not return true after this, the entry is
 * in an undefined state.
 */
static inline void list_del(struct list_head *entry){
	__list_del_entry(entry);
	entry->next = (void *)0x100;
	entry->prev = (void *)0x122;
}

/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
static inline void list_del_init(struct list_head *entry){
	__list_del_entry(entry);
	INIT_LIST_HEAD(entry);
}

/**
 * list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
static inline void list_move(struct list_head *list, struct list_head *head){
	__list_del_entry(list);
	list_add(list, head);
}

/**
 * list_replace - replace old entry by new one
 * @old : the element to be replaced
 * @new : the new element to insert
 *
 * If @old was empty, it will be overwritten.
 */
static inline void list_replace(struct list_head *old, struct list_head *new){
	new->next = old->next;
	new->next->prev = new;
	new->prev = old->prev;
	new->prev->next = new;
}

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_add_rcu(struct list_head *new, struct list_head *prev, struct list_head *next){
	if (!__list_add_valid(new, prev, next)){
		return;
	}
	new->next = next;
	new->prev = prev;
//	rcu_assign_pointer(list_next_rcu(prev), new);
	next->prev = new;
}

/**
 * list_add_rcu - add a new entry to rcu-protected list
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 *
 * The caller must take whatever precautions are necessary
 * (such as holding appropriate locks) to avoid racing
 * with another list-mutation primitive, such as list_add_rcu()
 * or list_del_rcu(), running on this same list.
 * However, it is perfectly legal to run concurrently with
 * the _rcu list-traversal primitives, such as
 * list_for_each_entry_rcu().
 */
static inline void list_add_rcu(struct list_head *new, struct list_head *head){
	__list_add_rcu(new, head, head->next);
}

/**
 * list_add_tail_rcu - add a new entry to rcu-protected list
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 *
 * The caller must take whatever precautions are necessary
 * (such as holding appropriate locks) to avoid racing
 * with another list-mutation primitive, such as list_add_tail_rcu()
 * or list_del_rcu(), running on this same list.
 * However, it is perfectly legal to run concurrently with
 * the _rcu list-traversal primitives, such as
 * list_for_each_entry_rcu().
 */
static inline void list_add_tail_rcu(struct list_head *new, struct list_head *head){
	__list_add_rcu(new, head->prev, head);
}

static inline void __refcount_add(int i, refcount_t *r, int *oldp){
	int old = atomic_fetch_add_explicit(&r->refs, i, memory_order_relaxed);

	if (oldp){
		*oldp = old;
	}
}

static inline void __refcount_inc(refcount_t *r, int *oldp){
	__refcount_add(1, r, oldp);
}

static inline void refcount_inc(refcount_t *r){
	__refcount_inc(r, NULL);
}

static inline void __refcount_dec(refcount_t *r, int *oldp){
        int old = 0;

        old = atomic_fetch_sub_explicit(&r->refs, 1, memory_order_release);

        if (oldp){
                *oldp = old;
        }
}

static inline bool __refcount_sub_and_test(int i, refcount_t *r, int *oldp){
	int old = 0;

	old = atomic_fetch_sub_explicit(&r->refs, i, memory_order_release);

	if (oldp){
		*oldp = old;
	}

	if (old > 0 && old == i) {
		return true;
	}

	return false;
}

static inline bool refcount_sub_and_test(int i, refcount_t *r){
	return __refcount_sub_and_test(i, r, NULL);
}

static inline bool __refcount_dec_and_test(refcount_t *r, int *oldp){
	return __refcount_sub_and_test(1, r, oldp);
}

static inline bool refcount_dec_and_test(refcount_t *r){
	return __refcount_dec_and_test(r, NULL);
}

static inline void kref_get(struct kref *kref){
	refcount_inc(&kref->refcount);
}

/**
 * ERR_PTR - Create an error pointer.
 * @error: A negative error code.
 *
 * Encodes @error into a pointer value. Users should consider the result
 * opaque and not assume anything about how the error is encoded.
 *
 * Return: A pointer with @error encoded within its value.
 */
static inline void *ERR_PTR(long error){
	return (void *)(intptr_t)error;
}

static inline long PTR_ERR(const void *ptr){
	return (long)(intptr_t)ptr;
}

static inline spin_lock_init(pthread_mutex_t *lock){
	pthread_mutex_init(lock, NULL);
}

static inline spin_lock(pthread_mutex_t *lock){
	pthread_mutex_lock(lock);
}

static inline spin_unlock(pthread_mutex_t *lock){
	pthread_mutex_unlock(lock);
}

static inline struct timespec wq_now(struct workqueue_struct *wq){
        struct timespec now = {};
        clock_gettime(wq->clock_id, &now);
        return now;
}

static inline long work_encode(struct workqueue_struct *wq, long flags){
	return ((long)(uintptr_t)wq) | (flags & (long)WORK_STRUCT_FLAG_MASK);
}

static inline struct workqueue_struct *work_wq(struct work_struct *work){
	long v = atomic_load_explicit(&work->data, memory_order_relaxed);
	return (struct workqueue_struct *)(uintptr_t)(v & ~(long)WORK_STRUCT_FLAG_MASK);
}

static inline long work_flags(struct work_struct *work){
	return atomic_load_explicit(&work->data, memory_order_relaxed) & (long)WORK_STRUCT_FLAG_MASK;
}

static inline bool work_is_linked(struct work_struct *work){
        return !list_empty(&work->entry);
}

static inline bool wq_in_worker(struct workqueue_struct *wq){
        return pthread_equal(pthread_self(), wq->thread);
}

static inline int timespec_cmp(const struct timespec *a, const struct timespec *b){
	if (a->tv_sec != b->tv_sec){
		return (a->tv_sec > b->tv_sec) - (a->tv_sec < b->tv_sec);
	}
	return (a->tv_nsec > b->tv_nsec) - (a->tv_nsec < b->tv_nsec);
}

static void delayed_list_insert_sorted(struct workqueue_struct *wq, struct delayed_work *dwork){
	struct work_struct *w = NULL;
	struct delayed_work *dw = NULL;
	struct list_head *pos = NULL;
	struct list_head *n = NULL;

	list_for_each_safe(pos, n, &wq->delayed_list) {
		w = container_of(pos, struct work_struct, entry);
		dw = container_of(w, struct delayed_work, work);
		if (timespec_cmp(&dwork->expires, &dw->expires) < 0) {
			__list_add(&dwork->work.entry, pos->prev, pos);
			return;
		}
	}
	list_add_tail(&dwork->work.entry, &wq->delayed_list);
}

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

static u64 read_uleb128(u8 *p, int len){
	int counter = 0;
	u8 b = 0;
	u64 v = 0;
	u32 shift = 0;

	while (counter < len) {
		b = p[counter++];
		v |= (u64)(b & 0x7f) << shift;
		if (!(b & 0x80)){
			break;
		}
		shift += 7;
	}

	return v;
}

static struct task_struct *tls_current;
static struct tasklet_head tasklet_global_queue;

/**
 * worker_thread - the worker thread function
 * @__worker: self
 *
 * The worker thread function.  All workers belong to a worker_pool -
 * either a per-cpu one or dynamic unbound one.  These workers process all
 * work items regardless of their specific target workqueue.  The only
 * exception is work items which belong to workqueues with a rescuer which
 * will be explained in rescuer_thread().
 *
 * Return: 0
 */
static void *worker_thread(void *arg){
	struct work_struct *work = NULL;
	struct workqueue_struct *wq = NULL;

	wq = (struct workqueue_struct *)arg;

	pthread_mutex_lock(&wq->lock);

	for (;;) {
		if (wq->stop && list_empty(&wq->work_list) && list_empty(&wq->delayed_list) && wq->nr_active == 0){
			break;
		}
		delayed_work_timer_fn(wq);

		if (!list_empty(&wq->work_list)) {
			work = list_first_entry(&wq->work_list, struct work_struct, entry);
			list_del_init(&work->entry);

			atomic_store_explicit(&work->data, work_encode(wq, WORK_STRUCT_RUNNING), memory_order_relaxed);
			wq->nr_active++;

			pthread_mutex_unlock(&wq->lock);
			work->func(work);
			pthread_mutex_lock(&wq->lock);

			wq->nr_active--;
			atomic_store_explicit(&work->data, 0, memory_order_release);
			pthread_cond_broadcast(&wq->cond);
			continue;
		}

		if (wq->stop) {
			pthread_cond_wait(&wq->cond, &wq->lock);
			continue;
		}

		if (list_empty(&wq->delayed_list)) {
			pthread_cond_wait(&wq->cond, &wq->lock);
		}else{
			struct delayed_work *dw = NULL;
			struct work_struct *w = NULL;

			w = list_first_entry(&wq->delayed_list, struct work_struct, entry);
			dw = container_of(w, struct delayed_work, work);

			pthread_cond_timedwait(&wq->cond, &wq->lock, &dw->expires);
		}
	}

	pthread_mutex_unlock(&wq->lock);
	return NULL;
}

static struct task_struct *current_thread_info(void){
	struct task_struct *current_task = NULL;
	pthread_condattr_t current_condition_attr = {};

	if (tls_current != NULL){
		return tls_current;
	}

	current_task = (struct task_struct *)malloc(sizeof(struct task_struct));

	if(current_task == NULL){
		return NULL;
	}

	pthread_mutex_init(&current_task->lock, NULL);

	pthread_condattr_init(&current_condition_attr);
	pthread_condattr_setclock(&current_condition_attr, CLOCK_MONOTONIC);
	pthread_cond_init(&current_task->cond, &current_condition_attr);
	pthread_condattr_destroy(&current_condition_attr);

	atomic_store(&current_task->__state, TASK_RUNNING);
	atomic_store(&current_task->started, true);
	atomic_store(&current_task->should_stop, false);

	current_task->tid = pthread_self();
	snprintf(current_task->name, sizeof(current_task->name), "main");

	tls_current = current_task;
	return current_task;
}

static inline bool wq_entry_is_linked(const struct wait_queue_entry *wq_entry){
	return !(wq_entry->entry.next == &wq_entry->entry && wq_entry->entry.prev == &wq_entry->entry);
}

/* kthread_trampoline: main function used for invoking the kthread cb */
static void *kthread_trampoline(void *arg){
	int ret = 0;
	struct task_struct *current_thread = NULL;
	current_thread = (struct task_struct *)arg;
	tls_current = current_thread;

	pthread_mutex_lock(&current_thread->lock);

	/* wait until the 'current_thread' has the 'started' flag set and the 'should_stop' flag isn't set */
	while (!atomic_load(&current_thread->started) && !atomic_load(&current_thread->should_stop)){
		pthread_cond_wait(&current_thread->cond, &current_thread->lock);
	}
	pthread_mutex_unlock(&current_thread->lock);

	if (!atomic_load(&current_thread->should_stop) && current_thread->threadfn){
		/* invoke the real thread callback */
		ret = current_thread->threadfn(current_thread->data);
	}
	/* set the exit code of the thread callback */
	current_thread->exit_code = ret;

	atomic_store(&current_thread->__state, TASK_RUNNING);
	pthread_mutex_lock(&current_thread->lock);
	pthread_cond_broadcast(&current_thread->cond);
	pthread_mutex_unlock(&current_thread->lock);

	return NULL;
}

static unsigned long round_jiffies_common(unsigned long j, int cpu, bool force_up){
	int rem = 0;
	unsigned long original = j;

	/*
	 * We don't want all cpus firing their timers at once hitting the
	 * same lock or cachelines, so we skew each extra cpu with an extra
	 * 3 jiffies. This 3 jiffies came originally from the mm/ code which
	 * already did this.
	 * The skew is done by adding 3*cpunr, then round, then subtract this
	 * extra offset again.
	 */
	j += cpu * 3;

	rem = j % HZ;

	/*
	 * If the target jiffy is just after a whole second (which can happen
	 * due to delays of the timer irq, long irq off times etc etc) then
	 * we should round down to the whole second, not up. Use 1/4th second
	 * as cutoff for this rounding as an extreme upper bound for this.
	 * But never round down if @force_up is set.
	 */
	if (rem < HZ/4 && !force_up){ /* round down */
		j = j - rem;
	}else{ /* round up */
		j = j - rem + HZ;
	}
	/* now that we have rounded, subtract the extra skew again */
	j -= cpu * 3;

	/*
	 * Make sure j is still in the future. Otherwise return the
	 * unmodified value.
	 */
//	return time_is_after_jiffies(j) ? j : original;
}

#include <prototypes.h>
#include <single_offset.h>
#include <struct_offset.h>
#include <mod_controller.h>
#include <hashtable.h>
#include <bits.h>
#include <skb.h>
#include <ieee80211.h>
#include <module.h>

#endif
