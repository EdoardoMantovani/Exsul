/* Copyright (C) Edoardo Mantovani - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
*/

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

/* size for the 'net_device' struct, usually around 2600 bytes */
#define NET_DEVICE_STRUCT_SIZE	3 * 1024

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

struct swait_queue_head {
	u32			lock;
	struct list_head	task_list;
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

/*
 * Notes on lockless lookups:
 *
 * All stores to the tree structure (rb_left and rb_right) must be done using
 * WRITE_ONCE(). And we must not inadvertently cause (temporary) loops in the
 * tree structure as seen in program order.
 *
 * These two requirements will allow lockless iteration of the tree -- not
 * correct iteration mind you, tree rotations are not atomic so a lookup might
 * miss entire subtrees.
 *
 * But they do guarantee that any such traversal will only see valid elements
 * and that it will indeed complete -- does not get stuck in a loop.
 *
 * It also guarantees that if the lookup returns an element it is the 'correct'
 * one. But not returning an element does _NOT_ mean it's not present.
 *
 * NOTE:
 *
 * Stores to __rb_parent_color are not important for simple lookups so those
 * are left undone as of now. Nor did I check for loops involving parent
 * pointers.
 */

static inline void rb_set_black(struct rb_node *rb){
	rb->__rb_parent_color += RB_BLACK;
}

static inline struct rb_node *rb_red_parent(struct rb_node *red){
	return (struct rb_node *)red->__rb_parent_color;
}

static inline void rb_set_parent(struct rb_node *rb, struct rb_node *p){
	rb->__rb_parent_color = rb_color(rb) + (unsigned long)p;
}

static inline void rb_set_parent_color(struct rb_node *rb, struct rb_node *p, int color){
	rb->__rb_parent_color = (unsigned long)p + color;
}

static inline void rb_link_node(struct rb_node *node, struct rb_node *parent, struct rb_node **rb_link){
	node->__rb_parent_color = (unsigned long)parent;
	node->rb_left = node->rb_right = NULL;

	*rb_link = node;
}

static inline void __rb_change_child(struct rb_node *old, struct rb_node *new, struct rb_node *parent, struct rb_root *root){
	if (parent) {
		if (parent->rb_left == old){
			WRITE_ONCE(parent->rb_left, new);
		}else{
			WRITE_ONCE(parent->rb_right, new);
		}
	}else{
		WRITE_ONCE(root->rb_node, new);
	}
}

/*
 * Helper function for rotations:
 * - old's parent and color get assigned to new
 * - old gets assigned new as a parent and 'color' as a color.
 */
static inline void __rb_rotate_set_parents(struct rb_node *old, struct rb_node *new, struct rb_root *root, int color){
	struct rb_node *parent = rb_parent(old);
	new->__rb_parent_color = old->__rb_parent_color;
	rb_set_parent_color(old, new, color);
	__rb_change_child(old, new, parent, root);
}

static inline void __rb_insert(struct rb_node *node, struct rb_root *root, void (*augment_rotate)(struct rb_node *old, struct rb_node *new)){
	struct rb_node *parent = rb_red_parent(node);
	struct rb_node *gparent = NULL;
	struct rb_node *tmp = NULL;

	while (true) {
		/*
		 * Loop invariant: node is red.
		 */
		if (unlikely(!parent)) {
			/*
			 * The inserted node is root. Either this is the
			 * first node, or we recursed at Case 1 below and
			 * are no longer violating 4).
			 */
			rb_set_parent_color(node, NULL, RB_BLACK);
			break;
		}

		/*
		 * If there is a black parent, we are done.
		 * Otherwise, take some corrective action as,
		 * per 4), we don't want a red root or two
		 * consecutive red nodes.
		 */
		if(rb_is_black(parent)){
			break;
		}

		gparent = rb_red_parent(parent);

		tmp = gparent->rb_right;
		if (parent != tmp) {	/* parent == gparent->rb_left */
			if (tmp && rb_is_red(tmp)) {
				/*
				 * Case 1 - node's uncle is red (color flips).
				 *
				 *       G            g
				 *      / \          / \
				 *     p   u  -->   P   U
				 *    /            /
				 *   n            n
				 *
				 * However, since g's parent might be red, and
				 * 4) does not allow this, we need to recurse
				 * at g.
				 */
				rb_set_parent_color(tmp, gparent, RB_BLACK);
				rb_set_parent_color(parent, gparent, RB_BLACK);
				node = gparent;
				parent = rb_parent(node);
				rb_set_parent_color(node, parent, RB_RED);
				continue;
			}

			tmp = parent->rb_right;
			if (node == tmp) {
				/*
				 * Case 2 - node's uncle is black and node is
				 * the parent's right child (left rotate at parent).
				 *
				 *      G             G
				 *     / \           / \
				 *    p   U  -->    n   U
				 *     \           /
				 *      n         p
				 *
				 * This still leaves us in violation of 4), the
				 * continuation into Case 3 will fix that.
				 */
				tmp = node->rb_left;
				WRITE_ONCE(parent->rb_right, tmp);
				WRITE_ONCE(node->rb_left, parent);
				if (tmp){
					rb_set_parent_color(tmp, parent, RB_BLACK);
				}
				rb_set_parent_color(parent, node, RB_RED);
				augment_rotate(parent, node);
				parent = node;
				tmp = node->rb_right;
			}

			/*
			 * Case 3 - node's uncle is black and node is
			 * the parent's left child (right rotate at gparent).
			 *
			 *        G           P
			 *       / \         / \
			 *      p   U  -->  n   g
			 *     /                 \
			 *    n                   U
			 */
			WRITE_ONCE(gparent->rb_left, tmp); /* == parent->rb_right */
			WRITE_ONCE(parent->rb_right, gparent);
			if (tmp){
				rb_set_parent_color(tmp, gparent, RB_BLACK);
			}
			__rb_rotate_set_parents(gparent, parent, root, RB_RED);
			augment_rotate(gparent, parent);
			break;
		} else {
			tmp = gparent->rb_left;
			if (tmp && rb_is_red(tmp)) {
				/* Case 1 - color flips */
				rb_set_parent_color(tmp, gparent, RB_BLACK);
				rb_set_parent_color(parent, gparent, RB_BLACK);
				node = gparent;
				parent = rb_parent(node);
				rb_set_parent_color(node, parent, RB_RED);
				continue;
			}

			tmp = parent->rb_left;
			if (node == tmp) {
				/* Case 2 - right rotate at parent */
				tmp = node->rb_right;
				WRITE_ONCE(parent->rb_left, tmp);
				WRITE_ONCE(node->rb_right, parent);
				if (tmp){
					rb_set_parent_color(tmp, parent, RB_BLACK);
				}
				rb_set_parent_color(parent, node, RB_RED);
				augment_rotate(parent, node);
				parent = node;
				tmp = node->rb_left;
			}

			/* Case 3 - left rotate at gparent */
			WRITE_ONCE(gparent->rb_right, tmp); /* == parent->rb_left */
			WRITE_ONCE(parent->rb_left, gparent);
			if (tmp){
				rb_set_parent_color(tmp, gparent, RB_BLACK);
			}
			__rb_rotate_set_parents(gparent, parent, root, RB_RED);
			augment_rotate(gparent, parent);
			break;
		}
	}
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

static inline bool queue_work(struct workqueue_struct *wq, struct work_struct *work){
	return queue_work_on(0, wq, work);
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
#include <net.h>
#include <skb.h>
#include <ieee80211.h>
#include <module.h>

#endif

#ifndef __UTILS_H__
#define __UTILS_H__

#ifndef SPEED_1600000
#define SPEED_1600000		1600000
#endif

#define	RB_RED			0
#define	RB_BLACK		1

#define OFFSET_TABLE_SECTION	__attribute__((section(".offset")))
#define MODULE_INPUT_MIN_LEN 	4

#define UL(x)			(_UL(x))
#define ULL(x)			(_ULL(x))
#define BIT(nr)			(UL(1) << (nr))
#define BIT_ULL(nr)		(ULL(1) << (nr))

#define BIT_MASK(nr)		(UL(1) << ((nr) % BITS_PER_LONG))
#define BIT_WORD(nr)		((nr) / BITS_PER_LONG)
#define BIT_ULL_MASK(nr)	(ULL(1) << ((nr) % BITS_PER_LONG_LONG))
#define BIT_ULL_WORD(nr)	((nr) / BITS_PER_LONG_LONG)
#define BITS_PER_BYTE		8
#define BITS_PER_TYPE(type)	(sizeof(type) * BITS_PER_BYTE)

#define DIV_ROUND_UP(n,d)	(((n) + (d) - 1) / (d))
#define BITS_TO_LONGS(nr)	__KERNEL_DIV_ROUND_UP(nr, BITS_PER_TYPE(long))
#define BITS_TO_U64(nr)		__KERNEL_DIV_ROUND_UP(nr, BITS_PER_TYPE(u64))
#define BITS_TO_U32(nr)		__KERNEL_DIV_ROUND_UP(nr, BITS_PER_TYPE(u32))
#define BITS_TO_BYTES(nr)	__KERNEL_DIV_ROUND_UP(nr, BITS_PER_TYPE(char))

#define _MAX(a, b)		(((a)>(b))?(a):(b))

#define ARRAY_SIZE(array)	(sizeof(array)/sizeof(array[0]))

#define offsetof(TYPE, MEMBER)	__builtin_offsetof(TYPE, MEMBER)

/* The "volatile" is due to gcc bugs */
# define barrier()		__asm__ __volatile__("": : :"memory")

/**
 * sizeof_field() - Report the size of a struct field in bytes
 *
 * @TYPE: The structure containing the field of interest
 * @MEMBER: The field to return the size of
 */
#define sizeof_field(TYPE, MEMBER) sizeof((((TYPE *)0)->MEMBER))

/**
 * offsetofend() - Report the offset of a struct field within the struct
 *
 * @TYPE: The type of the structure
 * @MEMBER: The member within the structure to get the end offset of
 */
#define offsetofend(TYPE, MEMBER) (offsetof(TYPE, MEMBER) + sizeof_field(TYPE, MEMBER))

/**
 * LIST_HEAD_INIT - initialize a &struct list_head's links to point to itself
 * @name: name of the list_head
 */
#define LIST_HEAD_INIT(name) { &(name), &(name) }

/**
 * LIST_HEAD - definition of a &struct list_head with initialization values
 * @name: name of the list_head
 */
#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

#define MAX_JIFFY_OFFSET ((LONG_MAX >> 1)-1)

/* NOTE:  these are not the standard USB_ENDPOINT_XFER_* values!! */
/* (yet ... they're the values used by usbfs) */
#define PIPE_ISOCHRONOUS		0
#define PIPE_INTERRUPT			1
#define PIPE_CONTROL			2
#define PIPE_BULK			3

#define NULLS_MARKER(value) (1UL | (((long)value) << 1))
#define INIT_HLIST_NULLS_HEAD(ptr, nulls) ((ptr)->first = (struct hlist_nulls_node *) NULLS_MARKER(nulls))
#define HLIST_NULLS_HEAD_INIT(nulls) {.first = (struct hlist_nulls_node *)NULLS_MARKER(nulls)}

#define DECLARE_BITMAP(name,bits)		unsigned long name[BITS_TO_LONGS(bits)]
#define __ETHTOOL_DECLARE_LINK_MODE_MASK(name)	DECLARE_BITMAP(name, __ETHTOOL_LINK_MODE_MASK_NBITS)

/*
 * small_const_nbits(n) is true precisely when it is known at compile-time
 * that BITMAP_SIZE(n) is 1, i.e. 1 <= n <= BITS_PER_LONG. This allows
 * various bit/bitmap APIs to provide a fast inline implementation. Bitmaps
 * of size 0 are very rare, and a compile-time-known-size 0 is most likely
 * a sign of error. They will be handled correctly by the bit/bitmap APIs,
 * but using the out-of-line functions, so that the inline implementations
 * can unconditionally dereference the pointer(s).
 */
#define small_const_nbits(nbits) (__builtin_constant_p(nbits) && (nbits) <= BITS_PER_LONG && (nbits) > 0)

#define hlist_nulls_entry(ptr, type, member) container_of(ptr,type,member)

#define hlist_nulls_entry_safe(ptr, type, member) \
	({ typeof(ptr) ____ptr = (ptr); \
	   !is_a_nulls(____ptr) ? hlist_nulls_entry(____ptr, type, member) : NULL; \
	})

/*
 * NULLS_MARKER() expects a hash value with the low
 * bits mostly likely to be significant, and it discards
 * the msb.
 * We give it an address, in which the bottom bit is
 * always 0, and the msb might be significant.
 * So we shift the address down one bit to align with
 * expectations and avoid losing a significant bit.
 *
 * We never store the NULLS_MARKER in the hash table
 * itself as we need the lsb for locking.
 * Instead we store a NULL
 */
#define	RHT_NULLS_MARKER(ptr)		((void *)NULLS_MARKER(((unsigned long) (ptr)) >> 1))
#define INIT_RHT_NULLS_HEAD(ptr)	((ptr) = NULL)

/*
 *       even if it isn't "correct", let's simplify 'usleep_range' in this way
 */
#define usleep_range(min_sleep, max_sleep)	usleep((min_sleep + max_sleep) / 2)

#define ms_to_us(ms)				((useconds_t)((ms) * 1000))
#define udelay(us)				usleep(us)
#define prep_zero_mask(a, bits, c)		(bits)
#define __fls(word)				generic___fls(word)
#define find_zero(bits)				(__ffs(bits) >> 3)

#define copy_from_user				_copy_from_user
#define copy_to_user				_copy_to_user

#define linkmode_test_bit			test_bit
#define linkmode_set_bit			__set_bit
#define linkmode_clear_bit			__clear_bit
#define linkmode_mod_bit			__assign_bit

/* pr_xxx functions */
#define pr_fmt(fmt)        	fmt
#define pr_notice(fmt, ...)	printk(pr_fmt(fmt), ##__VA_ARGS__)
#define pr_warn(fmt, ...)	printk(pr_fmt(fmt), ##__VA_ARGS__)
#define pr_err(fmt, ...)	printk(pr_fmt(fmt), ##__VA_ARGS__)
#define pr_crit(fmt, ...)	printk(pr_fmt(fmt), ##__VA_ARGS__)
#define pr_info(fmt, ...)	printk(pr_fmt(fmt), ##__VA_ARGS__)
#define pr_debug(fmt, ...)	printk(pr_fmt(fmt), ##__VA_ARGS__)
#define pr_alert(fmt, ...)	printk(pr_fmt(fmt), ##__VA_ARGS__)

#define kobj_to_dev(__kobj)	container_of_const(__kobj, struct device, kobj)

/* define the function attributes macro in the linux kernel as empty macro here */
#define __must_check
#define __signed_wrap
#define __fix_address
#define __kernel
#define __user
#define __iomem
#define __percpu
#define __rcu

#define __packed __attribute__((packed))
#define __force __attribute__((force))
#define __aligned(x) __attribute__((aligned(x)))
#define fallthrough __attribute__((fallthrough))

/*
 * Optional: only supported since gcc >= 15
 * Optional: only supported since clang >= 18
 *
 *   gcc: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=108896
 * clang: https://github.com/llvm/llvm-project/pull/76348
 *
 * __bdos on clang < 19.1.2 can erroneously return 0:
 * https://github.com/llvm/llvm-project/pull/110497
 *
 * __bdos on clang < 19.1.3 can be off by 4:
 * https://github.com/llvm/llvm-project/pull/112636
 */
#ifdef CONFIG_CC_HAS_COUNTED_BY
#define __counted_by(member)		__attribute__((__counted_by__(member)))
#else
#define __counted_by(member)
#endif

/* macro for translating a pipe (used only in the linux kernel) into an endpoint (used in libusb) */
#define usb_pipeendpoint(pipe)	(((pipe) >> 15) & 0xf)

/* macro used for finding out if the pipe is IN, otherwise is OUT */
#define usb_pipein(pipe)	((pipe) & USB_DIR_IN)

/* macros used for getting the endpoint/pipe type */
#define usb_pipetype(pipe)	(((pipe) >> 30) & 3)
#define usb_pipeisoc(pipe)	(usb_pipetype((pipe)) == PIPE_ISOCHRONOUS)
#define usb_pipeint(pipe)	(usb_pipetype((pipe)) == PIPE_INTERRUPT)
#define usb_pipecontrol(pipe)	(usb_pipetype((pipe)) == PIPE_CONTROL)
#define usb_pipebulk(pipe)	(usb_pipetype((pipe)) == PIPE_BULK)

#ifndef cpu_to_be16
#define cpu_to_be16 __cpu_to_be16
#endif

#ifndef cpu_to_le16
#define cpu_to_le16 __cpu_to_le16
#endif

#ifndef le16_to_cpu
#define le16_to_cpu __le16_to_cpu
#endif

#ifndef le32_to_cpu
#define le32_to_cpu __le32_to_cpu
#endif

#ifndef cpu_to_le32
#define cpu_to_le32 __cpu_to_le32
#endif

#ifndef be32_to_cpu
#define be32_to_cpu __be32_to_cpu
#endif

#ifndef le64_to_cpu
#define le64_to_cpu __le64_to_cpu
#endif

/**
 * do_div - returns 2 values: calculate remainder and update new dividend
 * @n: uint64_t dividend (will be updated)
 * @base: uint32_t divisor
 *
 * Summary:
 * ``uint32_t remainder = n % base;``
 * ``n = n / base;``
 *
 * Return: (uint32_t)remainder
 *
 * NOTE: macro parameter @n is evaluated multiple times,
 * beware of side effects!
 */
# define do_div(n,base) ({					\
	uint32_t __base = (base);				\
	uint32_t __rem;						\
	__rem = ((uint64_t)(n)) % __base;			\
	(n) = ((uint64_t)(n)) / __base;				\
	__rem;							\
 })

#ifndef htonl
#if __BYTE_ORDER == __BIG_ENDIAN
#define htonl(x) x
#else
#define htonl(x) __builtin_bswap32(x)
#endif
#endif

#define rb_parent(r)   ((struct rb_node *)((r)->__rb_parent_color & ~3))
#define	rb_entry(ptr, type, member) container_of(ptr, type, member)
#define RB_ROOT (struct rb_root) { NULL, }
#define RB_ROOT_CACHED (struct rb_root_cached) { {NULL, }, NULL }

#define __rb_color(pc)     ((pc) & 1)
#define __rb_is_black(pc)  __rb_color(pc)
#define __rb_is_red(pc)    (!__rb_color(pc))
#define rb_color(rb)       __rb_color((rb)->__rb_parent_color)
#define rb_is_red(rb)      __rb_is_red((rb)->__rb_parent_color)
#define rb_is_black(rb)    __rb_is_black((rb)->__rb_parent_color)

#define rcu_assign_pointer(p, v)					      \
do {									      \
	uintptr_t _r_a_p__v = (uintptr_t)(v);				      \
									      \
	if (__builtin_constant_p(v) && (_r_a_p__v) == (uintptr_t)NULL)	      \
		WRITE_ONCE((p), (typeof(p))(_r_a_p__v));		      \
} while (0)

/**
 * rcu_dereference_all() - fetch RCU-all-protected pointer for dereferencing
 * @p: The pointer to read, prior to dereferencing
 *
 * Makes rcu_dereference_check() do the dirty work.
 */
#define rcu_dereference_all(p) rcu_dereference_all_check(p, 0)

/*
 * Prefer C11 _Generic for better compile-times and simpler code. Note: 'char'
 * is not type-compatible with 'signed char', and we define a separate case.
 */
#define __scalar_type_to_expr_cases(type)				\
		unsigned type:	(unsigned type)0,			\
		signed type:	(signed type)0

#define __unqual_scalar_typeof(x) typeof(				\
		_Generic((x),						\
			 char:	(char)0,				\
			 __scalar_type_to_expr_cases(char),		\
			 __scalar_type_to_expr_cases(short),		\
			 __scalar_type_to_expr_cases(int),		\
			 __scalar_type_to_expr_cases(long),		\
			 __scalar_type_to_expr_cases(long long),	\
			 default: (x)))

#ifndef __READ_ONCE
#define __READ_ONCE(x)	(*(const volatile __unqual_scalar_typeof(x) *)&(x))
#endif

#define READ_ONCE(x)							\
({									\
	__READ_ONCE(x);							\
})

#define __WRITE_ONCE(x, val)						\
do {									\
	*(volatile typeof(x) *)&(x) = (val);				\
} while (0)

#define WRITE_ONCE(x, val)						\
do {									\
	__WRITE_ONCE(x, val);						\
} while (0)

#define container_of(ptr, type, member)                                        \
        ({                                                                     \
                const typeof(((type *)0)->member) *__mptr = ((void *)(ptr));   \
                (type *)((char *)__mptr - offsetof(type, member));             \
        })

/**
 * container_of_const - cast a member of a structure out to the containing
 *			structure and preserve the const-ness of the pointer
 * @ptr:		the pointer to the member
 * @type:		the type of the container struct this is embedded in.
 * @member:		the name of the member within the struct.
 *
 * Always prefer container_of_const() instead of container_of() in new code.
 */
#define container_of_const(ptr, type, member)				\
	_Generic(ptr,							\
		const typeof(*(ptr)) *: ((const type *)container_of(ptr, type, member)),\
		default: ((type *)container_of(ptr, type, member))	\
	)

#define __branch_check__(x, expect, is_constant)                               \
        ({                                                                     \
                long ______r;                                                  \
                ______r = __builtin_expect(!!(x), expect);                     \
                ______r;                                                       \
        })

/*
 * Check at compile time that something is of a particular type.
 * Always evaluates to 1 so you may use it easily in comparisons.
 */
#define typecheck(type,x) \
({	type __dummy; \
	typeof(x) __dummy2; \
	(void)(&__dummy == &__dummy2); \
	1; \
})

/*
 * Check at compile time that 'function' is a certain type, or is a pointer
 * to that type (needs to use typedef for the function type.)
 */
#define typecheck_fn(type,function) \
({	typeof(type) __tmp = function; \
	(void)__tmp; \
})

/*
 * Check at compile time that something is a pointer type.
 */
#define typecheck_pointer(x) \
({	typeof(x) __dummy; \
	(void)sizeof(*__dummy); \
	1; \
})

/**
 * time_after - returns true if the time a is after time b.
 * @a: first comparable as unsigned long
 * @b: second comparable as unsigned long
 *
 * Do this with "<0" and ">=0" to only test the sign of the result. A
 * good compiler would generate better code (and a really good compiler
 * wouldn't care). Gcc is currently neither.
 *
 * Return: %true is time a is after time b, otherwise %false.
 */
#define time_after(a,b)		\
	(typecheck(unsigned long, a) && \
	 typecheck(unsigned long, b) && \
	 ((long)((b) - (a)) < 0))
/**
 * time_before - returns true if the time a is before time b.
 * @a: first comparable as unsigned long
 * @b: second comparable as unsigned long
 *
 * Return: %true is time a is before time b, otherwise %false.
 */
#define time_before(a,b)	time_after(b,a)

#ifndef likely
#define likely(x) (__branch_check__(x, 1, __builtin_constant_p(x)))
#endif

#ifndef unlikely
#define unlikely(x) (__branch_check__(x, 0, __builtin_constant_p(x)))
#endif

#define __get_unaligned_t(type, ptr) ({                                                         \
        const struct { type x; } __attribute__((packed)) *__pptr = (typeof(__pptr))(ptr);       \
        __pptr->x;                                                                              \
})

#define REPEAT_BYTE(x)		((~0ul / 0xff) * (x))
#define WORD_AT_A_TIME_CONSTANTS { REPEAT_BYTE(0x01), REPEAT_BYTE(0x80) }

#define hex_asc_lo(x)		hex_asc[((x) & 0x0f)]
#define hex_asc_hi(x)		hex_asc[((x) & 0xf0) >> 4]

#define get_unaligned(ptr)      __get_unaligned_t(typeof(*(ptr)), (ptr))
#define __get_next(t, insn)     ({ t r = get_unaligned((t *)(insn)->next_byte); (insn)->next_byte += sizeof(t); leXX_to_cpu(t, r); })

#define validate_next(t, insn, n) ((insn)->next_byte + sizeof(t) + n <= (insn)->end_kaddr)
#define get_next(t, insn)       ({ if (unlikely(!validate_next(t, insn, 0))) goto err_out; __get_next(t, insn); })

/* element iteration helpers */
#define for_each_element(_elem, _data, _datalen)			\
	for (_elem = (const struct element *)(_data);			\
	     (const u8 *)(_data) + (_datalen) - (const u8 *)_elem >=	\
		(int)sizeof(*_elem) &&					\
	     (const u8 *)(_data) + (_datalen) - (const u8 *)_elem >=	\
		(int)sizeof(*_elem) + _elem->datalen;			\
	     _elem = (const struct element *)(_elem->data + _elem->datalen))

#define for_each_element_id(element, _id, data, datalen)		\
	for_each_element(element, data, datalen)			\
		if (element->id == (_id))

#define mod_mem_type_is_data(type) (!mod_mem_type_is_text(type))

#define for_each_mod_mem_type(type)                     \
        for (enum mod_mem_type (type) = 0;              \
             (type) < MOD_MEM_NUM_TYPES; (type)++)

#define for_class_mod_mem_type(type, class)             \
        for_each_mod_mem_type(type)                     \
                if (mod_mem_type_is_##class(type))

#define mod_mem_type_is_core_data(type)                 \
        (mod_mem_type_is_core(type) &&                  \
         mod_mem_type_is_data(type))

#define mod_mem_type_is_init(type)      \
        ((type) == MOD_INIT_TEXT ||     \
         (type) == MOD_INIT_DATA ||     \
         (type) == MOD_INIT_RODATA)

#define mod_mem_type_is_core(type) (!mod_mem_type_is_init(type))

#define mod_mem_type_is_text(type)      \
         ((type) == MOD_TEXT ||         \
          (type) == MOD_INIT_TEXT)

#define PAGE_ALIGN(addr)        ALIGN(addr, sysconf(_SC_PAGE_SIZE))
#define ALIGN(x, a)             __ALIGN_KERNEL((x), (a))
#define SH_ENTSIZE_TYPE_BITS    4
#define SH_ENTSIZE_TYPE_SHIFT   (BITS_PER_LONG - SH_ENTSIZE_TYPE_BITS)
#define SH_ENTSIZE_TYPE_MASK    ((1UL << SH_ENTSIZE_TYPE_BITS) - 1)
#define SH_ENTSIZE_OFFSET_MASK  ((1UL << (BITS_PER_LONG - SH_ENTSIZE_TYPE_BITS)) - 1)

#define NSEC_PER_SEC 1000000000ULL
#define USEC_PER_SEC 1000000ULL
#define MSEC_PER_SEC 1000ULL

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

#define BITMAP_FIRST_WORD_MASK(start) (~0UL << ((start) & (BITS_PER_LONG - 1)))
#define BITMAP_LAST_WORD_MASK(nbits) (~0UL >> (-(nbits) & (BITS_PER_LONG - 1)))

#define bitmap_size(nbits)	(ALIGN(nbits, BITS_PER_LONG) / BITS_PER_BYTE)

/*
 * Common helper for find_bit() function family
 * @FETCH: The expression that fetches and pre-processes each word of bitmap(s)
 * @MUNGE: The expression that post-processes a word containing found bit (may be empty)
 * @size: The bitmap size in bits
 */
#define FIND_FIRST_BIT(FETCH, MUNGE, size)					\
({										\
	unsigned long idx, val, sz = (size);					\
										\
	for (idx = 0; idx * BITS_PER_LONG < sz; idx++) {			\
		val = (FETCH);							\
		if (val) {							\
			sz = min(idx * BITS_PER_LONG + __ffs(MUNGE(val)), sz);	\
			break;							\
		}								\
	}									\
										\
	sz;									\
})

/*
 * Common helper for find_next_bit() function family
 * @FETCH: The expression that fetches and pre-processes each word of bitmap(s)
 * @MUNGE: The expression that post-processes a word containing found bit (may be empty)
 * @size: The bitmap size in bits
 * @start: The bitnumber to start searching at
 */
#define FIND_NEXT_BIT(FETCH, MUNGE, size, start)				\
({										\
	unsigned long mask, idx, tmp, sz = (size), __start = (start);		\
										\
	if (unlikely(__start >= sz))						\
		goto out;							\
										\
	mask = MUNGE(BITMAP_FIRST_WORD_MASK(__start));				\
	idx = __start / BITS_PER_LONG;						\
										\
	for (tmp = (FETCH) & mask; !tmp; tmp = (FETCH)) {			\
		if ((idx + 1) * BITS_PER_LONG >= sz)				\
			goto out;						\
		idx++;								\
	}									\
										\
	sz = min(idx * BITS_PER_LONG + __ffs(MUNGE(tmp)), sz);			\
out:										\
	sz;									\
})

#define FIND_NTH_BIT(FETCH, size, num)						\
({										\
	unsigned long sz = (size), nr = (num), idx, w, tmp;			\
										\
	for (idx = 0; (idx + 1) * BITS_PER_LONG <= sz; idx++) {			\
		if (idx * BITS_PER_LONG + nr >= sz)				\
			goto out;						\
										\
		tmp = (FETCH);							\
		w = hweight_long(tmp);						\
		if (w > nr)							\
			goto found;						\
										\
		nr -= w;							\
	}									\
										\
	if (sz % BITS_PER_LONG)							\
		tmp = (FETCH) & BITMAP_LAST_WORD_MASK(sz);			\
found:										\
	sz = idx * BITS_PER_LONG + fns(tmp, nr);				\
out:										\
	sz;									\
})

/**
 * time_is_after_jiffies - return true if a is after jiffies
 * @a: time (unsigned long) to compare to jiffies
 *
 * Return: %true is time a is after jiffies, otherwise %false.
 */
#define time_is_after_jiffies(a) time_before(jiffies, a)

#ifndef PAGE_SHIFT
#define PAGE_SHIFT 12
#endif

#undef  SHF_WRITE
#undef  SHF_ALLOC
#undef  SHF_EXECINSTR

#define SHF_WRITE               0x1
#define SHF_ALLOC               0x2
#define SHF_EXECINSTR           0x4
#define SHF_RELA_LIVEPATCH      0x00100000
#define SHF_RO_AFTER_INIT       0x00200000
#define SHF_MASKPROC            0xf0000000

#undef ELF_ST_BIND
#undef ELF_ST_TYPE
#undef ELF32_ST_BIND
#undef ELF32_ST_TYPE
#undef ELF64_ST_BIND
#undef ELF64_ST_TYPE

#define ELF_ST_BIND(x)          ((x) >> 4)
#define ELF_ST_TYPE(x)          ((x) & 0xf)
#define ELF32_ST_BIND(x)        ELF_ST_BIND(x)
#define ELF32_ST_TYPE(x)        ELF_ST_TYPE(x)
#define ELF64_ST_BIND(x)        ELF_ST_BIND(x)
#define ELF64_ST_TYPE(x)        ELF_ST_TYPE(x)

/**
 * list_entry - get the struct for this entry
 * @ptr:	the &struct list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 */
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

/**
 * list_first_entry - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

/**
 * list_last_entry - get the last element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_last_entry(ptr, type, member) \
	list_entry((ptr)->prev, type, member)

/**
 * list_first_entry_or_null - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 *
 * Note that if the list is empty, it returns NULL.
 */
#define list_first_entry_or_null(ptr, type, member) ({ \
	struct list_head *head__ = (ptr); \
	struct list_head *pos__ = READ_ONCE(head__->next); \
	pos__ != head__ ? list_entry(pos__, type, member) : NULL; \
})

/**
 * list_last_entry_or_null - get the last element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 *
 * Note that if the list is empty, it returns NULL.
 */
#define list_last_entry_or_null(ptr, type, member) ({ \
	struct list_head *head__ = (ptr); \
	struct list_head *pos__ = READ_ONCE(head__->prev); \
	pos__ != head__ ? list_entry(pos__, type, member) : NULL; \
})

/**
 * list_next_entry - get the next element in list
 * @pos:	the type * to cursor
 * @member:	the name of the list_head within the struct.
 */
#define list_next_entry(pos, member) \
	list_entry((pos)->member.next, typeof(*(pos)), member)

/**
 * list_next_entry_circular - get the next element in list
 * @pos:	the type * to cursor.
 * @head:	the list head to take the element from.
 * @member:	the name of the list_head within the struct.
 *
 * Wraparound if pos is the last element (return the first element).
 * Note, that list is expected to be not empty.
 */
#define list_next_entry_circular(pos, head, member) \
	(list_is_last(&(pos)->member, head) ? \
	list_first_entry(head, typeof(*(pos)), member) : list_next_entry(pos, member))

/**
 * list_prev_entry - get the prev element in list
 * @pos:	the type * to cursor
 * @member:	the name of the list_head within the struct.
 */
#define list_prev_entry(pos, member) \
	list_entry((pos)->member.prev, typeof(*(pos)), member)

/**
 * list_prev_entry_circular - get the prev element in list
 * @pos:	the type * to cursor.
 * @head:	the list head to take the element from.
 * @member:	the name of the list_head within the struct.
 *
 * Wraparound if pos is the first element (return the last element).
 * Note, that list is expected to be not empty.
 */
#define list_prev_entry_circular(pos, head, member) \
	(list_is_first(&(pos)->member, head) ? \
	list_last_entry(head, typeof(*(pos)), member) : list_prev_entry(pos, member))

/**
 * list_entry_is_head - test if the entry points to the head of the list
 * @pos:	the type * to cursor
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_entry_is_head(pos, head, member)				\
	list_is_head(&pos->member, (head))

/**
 * list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each(pos, head) \
	for (pos = (head)->next; !list_is_head(pos, (head)); pos = pos->next)

/**
 * list_for_each_continue - continue iteration over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 *
 * Continue to iterate over a list, continuing after the current position.
 */
#define list_for_each_continue(pos, head) \
	for (pos = pos->next; !list_is_head(pos, (head)); pos = pos->next)

/**
 * list_for_each_prev	-	iterate over a list backwards
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; !list_is_head(pos, (head)); pos = pos->prev)

/**
 * list_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; \
	     !list_is_head(pos, (head)); \
	     pos = n, n = pos->next)

/**
 * list_for_each_prev_safe - iterate over a list backwards safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define list_for_each_prev_safe(pos, n, head) \
	for (pos = (head)->prev, n = pos->prev; \
	     !list_is_head(pos, (head)); \
	     pos = n, n = pos->prev)

/**
 * list_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each_entry(pos, head, member)				\
	for (pos = list_first_entry(head, typeof(*pos), member);	\
	     !list_entry_is_head(pos, head, member);			\
	     pos = list_next_entry(pos, member))

/**
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each_entry_safe(pos, n, head, member)			\
	for (pos = list_first_entry(head, typeof(*pos), member),	\
		n = list_next_entry(pos, member);			\
	     !list_entry_is_head(pos, head, member); 			\
	     pos = n, n = list_next_entry(n, member))

#define rht_dereference(p, ht) \
	rcu_dereference_protected(p, lockdep_rht_mutex_is_held(ht))

#define rht_dereference_rcu(p, ht) \
	rcu_dereference_all_check(p, lockdep_rht_mutex_is_held(ht))

#define rht_dereference_bucket(p, tbl, hash) \
	rcu_dereference_protected(p, lockdep_rht_bucket_is_held(tbl, hash))

#define rht_dereference_bucket_rcu(p, tbl, hash) \
	rcu_dereference_all_check(p, lockdep_rht_bucket_is_held(tbl, hash))

#define rht_entry(tpos, pos, member) \
	({ tpos = container_of(pos, typeof(*tpos), member); 1; })

/**
 * rhl_for_each_rcu - iterate over rcu hash table list
 * @pos:	the &struct rlist_head to use as a loop cursor.
 * @list:	the head of the list
 *
 * This hash chain list-traversal primitive should be used on the
 * list returned by rhltable_lookup.
 */
#define rhl_for_each_rcu(pos, list)					\
	for (pos = list; pos; pos = rcu_dereference_all(pos->next))

/**
 * rhl_for_each_entry_rcu - iterate over rcu hash table list of given type
 * @tpos:	the type * to use as a loop cursor.
 * @pos:	the &struct rlist_head to use as a loop cursor.
 * @list:	the head of the list
 * @member:	name of the &struct rlist_head within the hashable struct.
 *
 * This hash chain list-traversal primitive should be used on the
 * list returned by rhltable_lookup.
 */
#define rhl_for_each_entry_rcu(tpos, pos, list, member)			\
	for (pos = list; pos && rht_entry(tpos, pos, member);		\
	     pos = rcu_dereference_all(pos->next))

#define struct_group_tagged(TAG, NAME, MEMBERS...)                             \
        __struct_group(TAG, NAME, /* no attrs */, MEMBERS)
#define struct_group(NAME, MEMBERS...)                                         \
        __struct_group(/* no tag */, NAME, /* no attrs */, MEMBERS)

#define __scalar_type_to_expr_cases(type)                               		\
                unsigned type:  (unsigned type)0,                       		\
                signed type:    (signed type)0

#define __unqual_scalar_typeof(x) typeof(                               		\
                _Generic((x),                                           		\
                         char:  (char)0,                                		\
                         __scalar_type_to_expr_cases(char),             		\
                         __scalar_type_to_expr_cases(short),            		\
                         __scalar_type_to_expr_cases(int),              		\
                         __scalar_type_to_expr_cases(long),             		\
                         __scalar_type_to_expr_cases(long long),        		\
                         default: (x)))

#ifndef __READ_ONCE
        #define __READ_ONCE(x)  (*(const volatile __unqual_scalar_typeof(x) *)&(x))
#endif

#define READ_ONCE(x)                                                    		\
({                                                                      		\
        __READ_ONCE(x);                                                 		\
})

#define ___PASTE(a,b) 			a##b
#define __PASTE(a,b) 			___PASTE(a,b)

#define __UNIQUE_ID(prefix) 		__PASTE(__PASTE(__UNIQUE_ID_, prefix), __COUNTER__)

#define rcu_check_sparse(p, space)
#define __rcu_dereference_check(p, local, c, space) \
({ \
	/* Dependency order vs. p above. */ \
	typeof(*p) *local = (typeof(*p) *__force)READ_ONCE(p); \
	rcu_check_sparse(p, space); \
	((typeof(*p) __force __kernel *)(local)); \
})
#define __rcu_dereference_protected(p, local, c, space) \
({ \
	rcu_check_sparse(p, space); \
	((typeof(*p) __force __kernel *)(p)); \
})

#define rcu_dereference_check(p, c) 	__rcu_dereference_check((p), __UNIQUE_ID(rcu), c || 0, __rcu)

#define rcu_dereference(p)		rcu_dereference_check(p, 0)
#define rcu_dereference_protected(p, c)	__rcu_dereference_protected((p), __UNIQUE_ID(rcu), (c), __rcu)

#define __COUNT_ARGS(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _n, X...) _n

#define COUNT_ARGS(X...) 		__COUNT_ARGS(, ##X, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define CONCATENATE(a, b)		__CONCAT(a, b)

#define strscpy(dst, src, ...)		CONCATENATE(__strscpy, COUNT_ARGS(__VA_ARGS__))(dst, src, __VA_ARGS__)
#define __strscpy1(dst, src, size)	sized_strscpy(dst, src, size)

#define tolower(c)			__tolower(c)
#define toupper(c)			__toupper(c)

#define __ismask(x)			(_ctype[(int)(unsigned char)(x)])

#define isalnum(c)			((__ismask(c)&(_U|_L|_D)) != 0)
#define isalpha(c)			((__ismask(c)&(_U|_L)) != 0)
#define iscntrl(c)			((__ismask(c)&(_C)) != 0)

#define isgraph(c)			((__ismask(c)&(_P|_U|_L|_D)) != 0)
#define islower(c)			((__ismask(c)&(_L)) != 0)
#define isprint(c)			((__ismask(c)&(_P|_U|_L|_D|_SP)) != 0)
#define ispunct(c)			((__ismask(c)&(_P)) != 0)

/* Note: isspace() must return false for %NUL-terminator */
#define isspace(c)			((__ismask(c)&(_S)) != 0)
#define isupper(c)			((__ismask(c)&(_U)) != 0)
#define isxdigit(c)			((__ismask(c)&(_D|_X)) != 0)

#define _U				0x01	/* upper */
#define _L				0x02	/* lower */
#define _D				0x04	/* digit */
#define _C				0x08	/* cntrl */
#define _P				0x10	/* punct */
#define _S				0x20	/* white space (space/lf/tab) */
#define _X				0x40	/* hex digit */
#define _SP				0x80	/* hard space (0x20) */

/*
 * Many architecture-specific non-atomic bitops contain inline asm code and due
 * to that the compiler can't optimize them to compile-time expressions or
 * constants. In contrary, generic_*() helpers are defined in pure C and
 * compilers optimize them just well.
 * Therefore, to make `unsigned long foo = 0; __set_bit(BAR, &foo)` effectively
 * equal to `unsigned long foo = BIT(BAR)`, pick the generic C alternative when
 * the arguments can be resolved at compile time. That expression itself is a
 * constant and doesn't bring any functional changes to the rest of cases.
 * The casts to `uintptr_t` are needed to mitigate `-Waddress` warnings when
 * passing a bitmap from .bss or .data (-> `!!addr` is always true).
 */
#define bitop(op, nr, addr)						\
	((__builtin_constant_p(nr) &&					\
	  __builtin_constant_p((uintptr_t)(addr) != (uintptr_t)NULL) &&	\
	  (uintptr_t)(addr) != (uintptr_t)NULL &&			\
	  __builtin_constant_p(*(const unsigned long *)(addr))) ?	\
	 const##op(nr, addr) : op(nr, addr))

/*
 * The following macros are non-atomic versions of their non-underscored
 * counterparts.
 */
#define __set_bit(nr, addr)		bitop(___set_bit, nr, addr)
#define __clear_bit(nr, addr)		bitop(___clear_bit, nr, addr)
#define __change_bit(nr, addr)		bitop(___change_bit, nr, addr)
#define __test_and_set_bit(nr, addr)	bitop(___test_and_set_bit, nr, addr)
#define __test_and_clear_bit(nr, addr)	bitop(___test_and_clear_bit, nr, addr)
#define __test_and_change_bit(nr, addr)	bitop(___test_and_change_bit, nr, addr)

#define test_bit(nr, addr)		bitop(_test_bit, nr, addr)
#define test_bit_acquire(nr, addr)	bitop(_test_bit_acquire, nr, addr)

/* Nanosecond scalar representation for kernel time values */
typedef s64	ktime_t;

/**
 * The type used for indexing onto a disc or disc partition.
 *
 * Linux always considers sectors to be 512 bytes long independently
 * of the devices real block size.
 *
 * blkcnt_t is the type of the inode's block count.
 */
typedef u64 sector_t;

typedef struct refcount_struct {
	atomic_uint refs;
}refcount_t;

struct kref {
	refcount_t refcount;
};

enum {
	LINK_CAPA_10HD = 0,
	LINK_CAPA_10FD,
	LINK_CAPA_100HD,
	LINK_CAPA_100FD,
	LINK_CAPA_1000HD,
	LINK_CAPA_1000FD,
	LINK_CAPA_2500FD,
	LINK_CAPA_5000FD,
	LINK_CAPA_10000FD,
	LINK_CAPA_20000FD,
	LINK_CAPA_25000FD,
	LINK_CAPA_40000FD,
	LINK_CAPA_50000FD,
	LINK_CAPA_56000FD,
	LINK_CAPA_100000FD,
	LINK_CAPA_200000FD,
	LINK_CAPA_400000FD,
	LINK_CAPA_800000FD,
	LINK_CAPA_1600000FD,

	__LINK_CAPA_MAX,
};

static const unsigned char _ctype[] = {
_C,_C,_C,_C,_C,_C,_C,_C,				/* 0-7 */
_C,_C|_S,_C|_S,_C|_S,_C|_S,_C|_S,_C,_C,			/* 8-15 */
_C,_C,_C,_C,_C,_C,_C,_C,				/* 16-23 */
_C,_C,_C,_C,_C,_C,_C,_C,				/* 24-31 */
_S|_SP,_P,_P,_P,_P,_P,_P,_P,				/* 32-39 */
_P,_P,_P,_P,_P,_P,_P,_P,				/* 40-47 */
_D,_D,_D,_D,_D,_D,_D,_D,				/* 48-55 */
_D,_D,_P,_P,_P,_P,_P,_P,				/* 56-63 */
_P,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U,		/* 64-71 */
_U,_U,_U,_U,_U,_U,_U,_U,				/* 72-79 */
_U,_U,_U,_U,_U,_U,_U,_U,				/* 80-87 */
_U,_U,_U,_P,_P,_P,_P,_P,				/* 88-95 */
_P,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L,		/* 96-103 */
_L,_L,_L,_L,_L,_L,_L,_L,				/* 104-111 */
_L,_L,_L,_L,_L,_L,_L,_L,				/* 112-119 */
_L,_L,_L,_P,_P,_P,_P,_C,				/* 120-127 */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,			/* 128-143 */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,			/* 144-159 */
_S|_SP,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,	/* 160-175 */
_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,	/* 176-191 */
_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,	/* 192-207 */
_U,_U,_U,_U,_U,_U,_U,_P,_U,_U,_U,_U,_U,_U,_U,_L,	/* 208-223 */
_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,	/* 224-239 */
_L,_L,_L,_L,_L,_L,_L,_P,_L,_L,_L,_L,_L,_L,_L,_L};	/* 240-255 */

static const char hex_asc[] = "0123456789abcdef";

struct word_at_a_time {
	const unsigned long one_bits;
	const unsigned long high_bits;
};

static inline void align_rsp16_before_glibc_call(void){
#if defined(__x86_64__)
	uintptr_t sp;
	__asm__ volatile("mov %%rsp, %0" : "=r"(sp));
	/* If sp is 8 mod 16 (or anything nonzero), alloca(sp & 0xf) makes it 0 mod 16. */
	alloca(sp & 0xf);
#endif
}

static inline int ilog2(u64 n){
	return n ? (63 - __builtin_clzll(n)) : -1;
}

/**
 * generic___set_bit - Set a bit in memory
 * @nr: the bit to set
 * @addr: the address to start counting from
 *
 * Unlike set_bit(), this function is non-atomic and may be reordered.
 * If it's called on the same region of memory simultaneously, the effect
 * may be that only one operation succeeds.
 */
static inline void generic___set_bit(unsigned long nr, volatile unsigned long *addr){
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);

	*p  |= mask;
}

static inline void generic___clear_bit(unsigned long nr, volatile unsigned long *addr){
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);

	*p &= ~mask;
}

/**
 * generic___change_bit - Toggle a bit in memory
 * @nr: the bit to change
 * @addr: the address to start counting from
 *
 * Unlike change_bit(), this function is non-atomic and may be reordered.
 * If it's called on the same region of memory simultaneously, the effect
 * may be that only one operation succeeds.
 */
static inline void generic___change_bit(unsigned long nr, volatile unsigned long *addr){
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);

	*p ^= mask;
}

/**
 * generic___test_and_set_bit - Set a bit and return its old value
 * @nr: Bit to set
 * @addr: Address to count from
 *
 * This operation is non-atomic and can be reordered.
 * If two examples of this operation race, one can appear to succeed
 * but actually fail.  You must protect multiple accesses with a lock.
 */
static inline bool generic___test_and_set_bit(unsigned long nr, volatile unsigned long *addr){
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);
	unsigned long old = *p;

	*p = old | mask;
	return (old & mask) != 0;
}

/**
 * generic___test_and_clear_bit - Clear a bit and return its old value
 * @nr: Bit to clear
 * @addr: Address to count from
 *
 * This operation is non-atomic and can be reordered.
 * If two examples of this operation race, one can appear to succeed
 * but actually fail.  You must protect multiple accesses with a lock.
 */
static inline bool generic___test_and_clear_bit(unsigned long nr, volatile unsigned long *addr){
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);
	unsigned long old = *p;

	*p = old & ~mask;
	return (old & mask) != 0;
}

/* WARNING: non atomic and it can be reordered! */
static inline bool generic___test_and_change_bit(unsigned long nr, volatile unsigned long *addr){
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);
	unsigned long old = *p;

	*p = old ^ mask;
	return (old & mask) != 0;
}

/**
 * generic_test_bit - Determine whether a bit is set
 * @nr: bit number to test
 * @addr: Address to start counting from
 */
static inline bool generic_test_bit(unsigned long nr, const volatile unsigned long *addr){
	/*
	 * Unlike the bitops with the '__' prefix above, this one *is* atomic,
	 * so `volatile` must always stay here with no cast-aways. See
	 * `Documentation/atomic_bitops.txt` for the details.
	 */
	return 1UL & (addr[BIT_WORD(nr)] >> (nr & (BITS_PER_LONG-1)));
}

/**
 * ror32 - rotate a 32-bit value right
 * @word: value to rotate
 * @shift: bits to roll
 */
static inline __u32 ror32(__u32 word, unsigned int shift){
	return (word >> (shift & 31)) | (word << ((-shift) & 31));
}

/**
 * rol32 - rotate a 32-bit value left
 * @word: value to rotate
 * @shift: bits to roll
 */
static inline u32 rol32(__u32 word, unsigned int shift){
	return (word << (shift & 31)) | (word >> ((-shift) & 31));
}

static inline __sum16 csum_fold(__wsum csum){
        u32 sum = (__force u32)csum;
        return (__force __sum16)((~sum - ror32(sum, 16)) >> 16);
}

static inline int lockdep_rht_mutex_is_held(struct rhashtable *ht){
	return 1;
}

static inline int lockdep_rht_bucket_is_held(const struct bucket_table *tbl, u32 hash){
	return 1;
}

static inline bool strstarts(const char *str, const char *prefix){
        return strncmp(str, prefix, strlen(prefix)) == 0;
}

static inline int isdigit(int c){
        return '0' <= c && c <= '9';
}

static inline unsigned char __tolower(unsigned char c){
	if (isupper(c)){
		c -= 'A'-'a';
	}
	return c;
}

static inline unsigned char __toupper(unsigned char c){
	if (islower(c)){
		c -= 'a'-'A';
	}
	return c;
}

static uint64_t nsec_now_monotonic(void){
	struct timespec ts = {};

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (uint64_t)ts.tv_sec * NSEC_PER_SEC + (uint64_t)ts.tv_nsec;
}

static inline struct timespec timespec_add_ms(struct timespec t, unsigned long ms){
	t.tv_sec += (time_t)(ms / 1000);
	t.tv_nsec += (long)(ms % 1000) * 1000000L;
	if (t.tv_nsec >= 1000000000L) {
		t.tv_sec++;
		t.tv_nsec -= 1000000000L;
	}
	return t;
}

/* convert_usb_pipe_to_ep: convert a kernel pipe into an usb endpoint for using it with libusb */
static unsigned char convert_usb_pipe_to_ep(unsigned char pipe){
	unsigned char ep_num = 0;
	unsigned char epaddr = 0;

	ep_num = usb_pipeendpoint(pipe);
	epaddr = ep_num | (usb_pipein(pipe) ? USB_DIR_IN : 0);
	return epaddr;
}

static inline unsigned int __ffs(unsigned long word){
	unsigned int num = 0;
#if BITS_PER_LONG == 64
	if ((word & 0xffffffff) == 0) {
		num += 32;
		word >>= 32;
	}
#endif
	if ((word & 0xffff) == 0) {
		num += 16;
		word >>= 16;
	}
	if ((word & 0xff) == 0) {
		num += 8;
		word >>= 8;
	}
	if ((word & 0xf) == 0) {
		num += 4;
		word >>= 4;
	}
	if ((word & 0x3) == 0) {
		num += 2;
		word >>= 2;
	}
	if ((word & 0x1) == 0)
		num += 1;
	return num;
}

static inline unsigned int generic___fls(unsigned long word){
	unsigned int num = BITS_PER_LONG - 1;

#if BITS_PER_LONG == 64
	if (!(word & (~0ul << 32))) {
		num -= 32;
		word <<= 32;
	}
#endif
	if (!(word & (~0ul << (BITS_PER_LONG-16)))) {
		num -= 16;
		word <<= 16;
	}
	if (!(word & (~0ul << (BITS_PER_LONG-8)))) {
		num -= 8;
		word <<= 8;
	}
	if (!(word & (~0ul << (BITS_PER_LONG-4)))) {
		num -= 4;
		word <<= 4;
	}
	if (!(word & (~0ul << (BITS_PER_LONG-2)))) {
		num -= 2;
		word <<= 2;
	}
	if (!(word & (~0ul << (BITS_PER_LONG-1))))
		num -= 1;
	return num;
}

/**
 * fns - find N'th set bit in a word
 * @word: The word to search
 * @n: Bit to find
 */
static inline unsigned int fns(unsigned long word, unsigned int n){
	while (word && n--){
		word &= word - 1;
	}
	return word ? __ffs(word) : BITS_PER_LONG;
}

static unsigned long read_word_at_a_time(const void *addr){
        return *(unsigned long *)addr;
}

static inline unsigned long has_zero(unsigned long a, unsigned long *bits, const struct word_at_a_time *c){
	unsigned long mask = ((a - c->one_bits) & ~a) & c->high_bits;
	*bits = mask;
	return mask;
}

#if defined(__LP64__) || defined(__LLP64__)
#define create_zero_mask(bits) (bits)
#else
static inline unsigned long create_zero_mask(unsigned long bits){
	bits = (bits - 1) & ~bits;
	return bits >> 7;
}
#endif

static unsigned long zero_bytemask(unsigned long bits);

static ssize_t sized_strscpy(char *dest, const char *src, size_t count){
        const struct word_at_a_time constants           = WORD_AT_A_TIME_CONSTANTS;
        size_t max                                      = count;
        long res                                        = 0;

        if (count == 0 || (count > 99999999)){
                return -E2BIG;
        }

        if ((long)src & (sizeof(long) - 1)) {
                size_t limit = 4096 - ((long)src & (4096 - 1));
                if (limit < max){
                        max                             = limit;
                }
        }

        while (max >= sizeof(unsigned long)) {
                unsigned long c                         = 0;
                unsigned long data                      = 0;

                c                                       = read_word_at_a_time(src+res);
                if (has_zero(c, &data, &constants)) {
                        data                            = prep_zero_mask(c, data, &constants);
                        data                            = create_zero_mask(data);
                        *(unsigned long *)(dest+res)    = c & zero_bytemask(data);
                        return res + find_zero(data);
                }
                *(unsigned long *)(dest+res)            = c;
                res                                    += sizeof(unsigned long);
                count                                  -= sizeof(unsigned long);
                max                                    -= sizeof(unsigned long);
        }

        while (count) {
                char c                                  = 0;

                c                                       = src[res];
                dest[res]                               = c;

                if (! ( c ) ){
                        return res;
                }

                res++;
                count--;
        }

        if (res){
                dest[res-1]                             = '\0';
        }

        return -E2BIG;
}

/**
 * is_power_of_2() - check if a value is a power of two
 * @n: the value to check
 *
 * Determine whether some value is a power of two, where zero is
 * *not* considered a power of two.
 * Return: true if @n is a power of 2, otherwise false.
 */
static inline const bool is_power_of_2(unsigned long n){
	return (n != 0 && ((n & (n - 1)) == 0));
}

/*
 * HZ is equal to or smaller than 1000, and 1000 is a nice round
 * multiple of HZ, divide with the factor between them, but round
 * upwards:
 */
static inline unsigned long _msecs_to_jiffies(const unsigned int m){
	return (m + (MSEC_PER_SEC / HZ) - 1) / (MSEC_PER_SEC / HZ);
}

#if !(USEC_PER_SEC % HZ)
static inline unsigned long _usecs_to_jiffies(const unsigned int u){
	return (u + (USEC_PER_SEC / HZ) - 1) / (USEC_PER_SEC / HZ);
}
#else
static inline unsigned long _usecs_to_jiffies(const unsigned int u){
	return (USEC_TO_HZ_MUL32 * u + USEC_TO_HZ_ADJ32) >> USEC_TO_HZ_SHR32;
}
#endif

static bool is_a_number(unsigned char character){
	bool ret = false;
	if(character <= '1' && character >= '9'){
		ret = true;
	}
	return ret;
}

static int speed_duplex_to_capa(int speed, unsigned int duplex){
	if (duplex == DUPLEX_UNKNOWN || (speed > SPEED_1000 && duplex != DUPLEX_FULL)){
		return -EINVAL;
	}

	switch (speed) {
		case SPEED_10: return duplex == DUPLEX_FULL ? LINK_CAPA_10FD : LINK_CAPA_10HD;
		case SPEED_100: return duplex == DUPLEX_FULL ? LINK_CAPA_100FD : LINK_CAPA_100HD;
		case SPEED_1000: return duplex == DUPLEX_FULL ? LINK_CAPA_1000FD : LINK_CAPA_1000HD;
		case SPEED_2500: return LINK_CAPA_2500FD;
		case SPEED_5000: return LINK_CAPA_5000FD;
		case SPEED_10000: return LINK_CAPA_10000FD;
		case SPEED_20000: return LINK_CAPA_20000FD;
		case SPEED_25000: return LINK_CAPA_25000FD;
		case SPEED_40000: return LINK_CAPA_40000FD;
		case SPEED_50000: return LINK_CAPA_50000FD;
		case SPEED_56000: return LINK_CAPA_56000FD;
		case SPEED_100000: return LINK_CAPA_100000FD;
		case SPEED_200000: return LINK_CAPA_200000FD;
		case SPEED_400000: return LINK_CAPA_400000FD;
		case SPEED_800000: return LINK_CAPA_800000FD;
		case SPEED_1600000: return LINK_CAPA_1600000FD;
	}

	return -EINVAL;
}

/*
 * simple function which converts the vermagic into a series of numbers
 * example of a vermagic: vermagic=6.14.0-36-generic
 */
static int convert_vermagic(unsigned char *vermagic_string, u8 *version, u8 *subversion, u8 *modification){
	int ret = 0;
	unsigned char current_ver = NULL;

	if(vermagic_string == NULL){
		ret = -EINVAL;
		return ret;
	}

	current_ver = strtok(vermagic_string, '.');
	if(current_ver != NULL){
		*version = atoi(current_ver);
	}else{
		*version = 0;
	}

	return ret;
}

#endif



void __netif_napi_del_locked(struct napi_struct *napi){

}

int kernel_write(struct file_layer *file, const void *buf, size_t count, u32 *pos){
	int ret = 0;

	if(file == NULL || file->actual_fd <= 0){
		ret = -EINVAL;
		return ret;
	}
	printf("[!] position: %d\n", *pos);

	//lseek(file->actual_fd, *pos, SEEK_SET);
	ret = write(file->actual_fd, buf, count);
	return ret;
}

void seq_putc(struct seq_file *m, char c){
	if (m->count >= m->size){
		return;
	}
	m->buf[m->count++] = c;
}

void consume_skb(struct sk_buff *skb){
	if (!skb_unref(skb)){
		return;
	}

	__kfree_skb(skb);
}

void __fentry__(void){

}

void wait_for_completion(struct completion *x){
	int ret = 0;
	int cur = 0;

	for (;;) {
		if (try_consume_completion(x)){
			return;
		}

		cur = atomic_load_explicit(&x->done, memory_order_acquire);
		if (cur == 0) {
			ret = futex_wait(&x->done, 0);
			if (ret < 0 && errno == EINTR) {
				continue;
			}
		}
	}
}

void *skb_put(struct sk_buff *skb, unsigned int len){
	void *tmp = NULL;
	tmp = skb_tail_pointer(skb);
	if(skb_is_nonlinear(skb)){

	}
	skb->tail += len;
	skb->len  += len;
	if (unlikely(skb->tail > skb->end)){
		skb_over_panic(skb, len, __builtin_return_address(0));
	}
	return tmp;
}

int usb_register_driver(struct usb_driver *new_driver, struct module *owner, const char *mod_name){
	return 0;
}

char *devm_kvasprintf(struct device *dev, int gfp, const char *fmt, va_list ap){
	unsigned int len;
	char *p;
	va_list aq;

	va_copy(aq, ap);
	len = vsnprintf(NULL, 0, fmt, aq);
	va_end(aq);

	p = devm_kmalloc(dev, len+1, gfp);
	if (!p)
		return NULL;

	vsnprintf(p, len+1, fmt, ap);

	return p;
}

int ieee80211_emulate_switch_vif_chanctx(void *hw, void *vifs, int n_vifs, enum ieee80211_chanctx_switch_mode mode){
	return 0;
}

void _dev_info(const struct device *dev, const char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}

int kstrtou8(const char *s, unsigned int base, u8 *res){
	*res = strtoul_with_range(s, NULL, base, 256);
	return 0;
}

int mod_delayed_work_on(int cpu, struct workqueue_struct *wq, struct delayed_work *dwork, unsigned long delay){
	int ret = 0;
	bool was_pending = 0;
	long flags = 0;
	struct work_struct *work = NULL;

	if (wq == NULL || dwork == NULL){
		return ret;
	}

	work = &dwork->work;

	if(work_wq(work) == wq && wq_in_worker(wq) && (work_flags(work) & WORK_STRUCT_RUNNING)) {
		errno = EDEADLK;
		return ret;
	}

	pthread_mutex_lock(&wq->lock);

	flags = work_flags(work);
	was_pending = !!(flags & WORK_STRUCT_PENDING);

	if ((flags & WORK_STRUCT_PENDING) && work_is_linked(work)){
		list_del_init(&work->entry);
	}

	if (flags & WORK_STRUCT_RUNNING) {
		while (work_flags(work) & WORK_STRUCT_RUNNING){
			pthread_cond_wait(&wq->cond, &wq->lock);
		}
		flags = work_flags(work);
		was_pending = false;
	}

	if (!(flags & (WORK_STRUCT_PENDING | WORK_STRUCT_RUNNING))) {
		long expected = 0;
		long desired = work_encode(wq, WORK_STRUCT_PENDING | WORK_STRUCT_DELAYED);
		ret = atomic_compare_exchange_strong_explicit(&work->data, &expected, desired, memory_order_acq_rel, memory_order_relaxed);
		if(ret == 0) {
			pthread_mutex_unlock(&wq->lock);
			return ret;
		}
	}else{
		atomic_store_explicit(&work->data, work_encode(wq, WORK_STRUCT_PENDING | WORK_STRUCT_DELAYED), memory_order_relaxed);
	}

	dwork->expires = timespec_add_ms(wq_now(wq), delay);
	delayed_list_insert_sorted(wq, dwork);

	pthread_cond_signal(&wq->cond);
	pthread_mutex_unlock(&wq->lock);

	return was_pending;
}

int devm_request_threaded_irq(struct device *dev, unsigned int irq, irq_handler_t handler, irq_handler_t thread_fn, unsigned long irqflags, const char *devname, void *dev_id){
	return 0;
}

ssize_t memory_read_from_buffer(void *to, size_t count, loff_t *ppos, const void *from, size_t available){
	loff_t pos = *ppos;

	if (pos < 0){
		return -EINVAL;
	}
	if (pos >= available){
		return 0;
	}
	if (count > available - pos){
		count = available - pos;
	}

	memcpy(to, from + pos, count);
	*ppos = pos + count;

	return count;
}

void _dev_printk(const char *level, const void *dev, const char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}

struct sk_buff *skb_dequeue(struct sk_buff_head *list){
	struct sk_buff *result = NULL;

	result = __skb_dequeue(list);
	return result;
}

struct usb_device *usb_get_dev(struct usb_device *dev){
	if (dev){
		get_device(&dev->dev);
	}
	return dev;
}

int param_set_uint_minmax(const char *val, const struct kernel_param *kp, unsigned int min, unsigned int max){
	int ret = 0;
	unsigned int num = 0;

	if (!val){
		return -EINVAL;
	}
	ret = kstrtouint(val, 0, &num);
	if (ret){
		return ret;
	}
	if (num < min || num > max){
		return -EINVAL;
	}
	*((unsigned int *)kp->arg) = num;
	return 0;
}

void cfg80211_chandef_create(struct cfg80211_chan_def *chandef, struct ieee80211_channel *chan, enum nl80211_channel_type chan_type){
	if (chan == NULL){
		return;
	}

	*chandef = (struct cfg80211_chan_def) {
		.chan = chan,
		.freq1_offset = chan->freq_offset,
	};

	switch (chan_type) {
	case NL80211_CHAN_NO_HT:
		chandef->width = NL80211_CHAN_WIDTH_20_NOHT;
		chandef->center_freq1 = chan->center_freq;
		break;
	case NL80211_CHAN_HT20:
		chandef->width = NL80211_CHAN_WIDTH_20;
		chandef->center_freq1 = chan->center_freq;
		break;
	case NL80211_CHAN_HT40PLUS:
		chandef->width = NL80211_CHAN_WIDTH_40;
		chandef->center_freq1 = chan->center_freq + 10;
		break;
	case NL80211_CHAN_HT40MINUS:
		chandef->width = NL80211_CHAN_WIDTH_40;
		chandef->center_freq1 = chan->center_freq - 10;
		break;
	default:
		goto end;
	}
	end:
}

void get_netmem(netmem_ref netmem){


}


void validate_usercopy_range(void){

}

void __x86_indirect_thunk_r10(void){

}

void __x86_return_thunk(void){

}

bool __bitmap_subset(unsigned long *bitmap1, unsigned long *bitmap2, unsigned int bits){
	unsigned int k, lim = bits/BITS_PER_LONG;
	for (k = 0; k < lim; ++k){
		if (bitmap1[k] & ~bitmap2[k]){
			return false;
		}
	}

	if (bits % BITS_PER_LONG){
		if ((bitmap1[k] & ~bitmap2[k]) & BITMAP_LAST_WORD_MASK(bits)){
			return false;
		}
	}

	return true;
}

void pci_enable_msi(struct pci_dev *dev){

}

void __skb_zcopy_downgrade_managed(struct sk_buff *skb){
	skb_shinfo(skb)->flags &= ~SKBFL_MANAGED_FRAG_REFS;
	for (int i = 0; i < skb_shinfo(skb)->nr_frags; i++){
		skb_frag_ref(skb, i);
	}
}

void _raw_spin_lock(void *spin_lock){

}

u32 ieee80211_calc_tx_airtime(struct ieee80211_hw *hw, struct ieee80211_tx_info *info, int len){
	u32 duration = 0;
	u32 cur_duration = 0;
	struct ieee80211_tx_rate *rate = NULL;

	for (int i = 0; i < ARRAY_SIZE(info->status.rates); i++) {
		rate = &info->status.rates[i];

		cur_duration = ieee80211_calc_tx_airtime_rate(hw, rate, NULL, info->band, len);
		if (cur_duration == 0){
			break;
		}
		duration += cur_duration * rate->count;
	}

	return duration;
}

unsigned long __find_nth_and_andnot_bit(const unsigned long *addr1, const unsigned long *addr2, const unsigned long *addr3, unsigned long size, unsigned long n){
	return FIND_NTH_BIT(addr1[idx] & addr2[idx] & ~addr3[idx], size, n);
}

const struct element * cfg80211_find_elem_match(u8 eid, const u8 *ies, unsigned int len,  const u8 *match, unsigned int match_len, unsigned int match_offset){
	const struct element *elem;

	for_each_element_id(elem, eid, ies, len) {
		if (elem->datalen >= match_offset + match_len &&
		    !memcmp(elem->data + match_offset, match, match_len))
			return elem;
	}

	return NULL;
}

void *vzalloc_noprof(unsigned long size){
	return malloc(size);
}

void complete(struct completion *x){
	atomic_fetch_add_explicit(&x->done, 1, memory_order_release);
	futex_wake(&x->done, 1, 0, 0);
}

void free_netdev(struct net_device *dev){
	if(dev != NULL){
		free(dev);
	}
}

void skb_trim(struct sk_buff *skb, unsigned int len){
	if (skb->len > len){
		__skb_trim(skb, len);
	}
}

int device_set_wakeup_enable(struct device *dev, bool enable){
	return 0;
}

unsigned long __find_nth_andnot_bit(const unsigned long *addr1, const unsigned long *addr2, unsigned long size, unsigned long n){
	return FIND_NTH_BIT(addr1[idx] & ~addr2[idx], size, n);
}

void kfree_const(const void *x){
	if(x != NULL){
		free(x);
	}
}

void __ubsan_handle_divrem_overflow(void *_data, void *lhs, void *rhs){

}

union acpi_object *acpi_evaluate_dsm(void *handle, ...){
	return NULL;
}


void sk_skb_reason_drop(struct sock *sk, struct sk_buff *skb, int reason){
	__kfree_skb(skb);
}

void vfree(void *addr){
	if(addr != NULL){
		free(addr);
	}
}

void __mutex_init(pthread_mutex_t *lock, const char *name, void *key){
	pthread_mutex_init(lock, NULL);
}

int seq_open(void *file, const struct seq_operations *op){
	int ret = 0;
	struct seq_file *p = NULL;

	//p = kmem_cache_zalloc(sizeof(struct );
	if (p == NULL){
		ret = -ENOMEM;
		return ret;
	}


	return ret;
}

unsigned char *skb_tail_pointer(const struct sk_buff *skb){
	#if BITS_PER_LONG > 32
	return skb->head + skb->tail;
	#else
	return skb->tail;
	#endif
}


void __x86_indirect_thunk_r14(void){

}

unsigned long __usecs_to_jiffies(const unsigned int u){
	if (u > jiffies_to_usecs(MAX_JIFFY_OFFSET)){
		return MAX_JIFFY_OFFSET;
	}
	return _usecs_to_jiffies(u);
}

int skb_copy_ubufs(struct sk_buff *skb, int gfp_mask){
	int ret = 0;
	unsigned int old_len = 0;
	unsigned int nr_frags = 0;
	unsigned int extra_tail = 0;
	unsigned int linearized_len = 0;
	unsigned char *dst = NULL;
	struct skb_shared_info *shinfo = NULL;

	if (skb == NULL){
		ret = -EINVAL;
		return ret;
	}

	if (skb_shared(skb) || skb_unclone(skb, gfp_mask)){
		ret = -EINVAL;
		return ret;
	}

	if (!skb_frags_readable(skb)){
		ret = -EFAULT;
		return ret;
	}

	shinfo = skb_shinfo(skb);
	nr_frags = shinfo->nr_frags;

	if (skb_has_frag_list(skb)){
		ret = -EOPNOTSUPP;
		return ret;
	}

	linearized_len = skb->data_len;
	if (nr_frags == 0 || linearized_len == 0){
		goto release;
	}

	extra_tail = 0;

	if (skb_tailroom(skb) < linearized_len){
		extra_tail = linearized_len - skb_tailroom(skb);
	}

	if (extra_tail) {
		ret = pskb_expand_head(skb, 0, extra_tail, gfp_mask);
		if (ret){
			return ret;
		}
	}

	shinfo = skb_shinfo(skb);
	dst = skb_tail_pointer(skb);
	old_len = skb->len;

	skb_put(skb, linearized_len);
	skb->len = old_len;
	skb->data_len = 0;

	for (int i = 0; i < nr_frags; i++){
		//skb_frag_unref(skb, i);
	}

	memset(shinfo->frags, 0, sizeof(shinfo->frags));
	shinfo->nr_frags = 0;

	release:
	skb_zcopy_clear(skb, false);
	return ret;
}

void dev_mc_init(struct net_device *dev){
	__hw_addr_init(&dev->mc);
}

struct sk_buff *alloc_skb_for_msg(struct sk_buff *first){
	struct sk_buff *n = NULL;

	n = alloc_skb(0, GFP_ATOMIC);
	if (n == NULL){
		return NULL;
	}

	n->len = first->len;
	n->data_len = first->len;
	n->truesize = first->truesize;

	skb_shinfo(n)->frag_list = first;

	__copy_skb_header(n, first);
	n->destructor = NULL;

	return n;
}

int usb_interrupt_msg(struct usb_device *usb_dev, unsigned int pipe, void *data, int len, int *actual_length, int timeout){
	int ret = 0;
	u8 ep = 0;

	ep = convert_usb_pipe_to_ep(pipe);

        ret = libusb_interrupt_transfer(
                &director.usb_dev,
		ep,
                data,
                len,
                actual_length,
                timeout
        );
	return ret;
}

void usleep_range_state(unsigned long min, unsigned long max, unsigned int state){
	usleep((min + max) / 2);
}

void cfg80211_bss_iter(struct wiphy *wiphy, struct cfg80211_chan_def *chandef, void (*iter)(struct wiphy *wiphy, struct cfg80211_bss *bss, void *data), void *iter_data){
	struct cfg80211_registered_device *rdev = NULL;
	struct cfg80211_internal_bss *bss;
	struct ieee80211_local *local = &director.ieee80211_local;

	pthread_mutex_lock(&local->bss_lock);
	list_for_each_entry(bss, &rdev->bss_list, list) {
		if (!chandef || cfg80211_is_sub_chan(chandef, bss->pub.channel, false)){
			iter(wiphy, &bss->pub, iter_data);
		}
	}

	pthread_mutex_unlock(&local->bss_lock);
}

void cdev_init(struct cdev *cdev, const struct file_operations *fops){
	memset(cdev, 0, sizeof(struct cdev));
	INIT_LIST_HEAD(&cdev->list);
	cdev->ops = fops;
}

void sort(void *base, size_t num, size_t size, cmp_func_t cmp_func, swap_func_t swap_func){
	qsort(base, num, size, cmp_func);
}

void ieee80211_key_mic_failure(struct ieee80211_key_conf *keyconf){
	struct ieee80211_key *key = NULL;

	key = container_of(keyconf, struct ieee80211_key, conf);

	switch (key->conf.cipher) {
	case WLAN_CIPHER_SUITE_AES_CMAC:
	case WLAN_CIPHER_SUITE_BIP_CMAC_256:
		key->u.aes_cmac.icverrors++;
		break;
	case WLAN_CIPHER_SUITE_BIP_GMAC_128:
	case WLAN_CIPHER_SUITE_BIP_GMAC_256:
		key->u.aes_gmac.icverrors++;
		break;
	default:
		/* ignore the others for now, we don't keep counters now */
		break;
	}
}

void pci_disable_device(struct pci_dev *dev){

}

void dev_uc_init(struct net_device *dev){
	__hw_addr_init(&dev->uc);
}

void __seq_puts(struct seq_file *m, const char *s){
	seq_write(m, s, strlen(s));
}

int hex_dump_to_buffer(const void *buf, size_t len, int rowsize, int groupsize, char *linebuf, size_t linebuflen, bool ascii){
	const u8 *ptr = buf;
	int ngroups;
	u8 ch;
	int j, lx = 0;
	int ascii_column;
	int ret;

	if (rowsize != 16 && rowsize != 32)
		rowsize = 16;

	if (len > rowsize)		/* limit to one line at a time */
		len = rowsize;
	if (!is_power_of_2(groupsize) || groupsize > 8)
		groupsize = 1;
	if ((len % groupsize) != 0)	/* no mixed size output */
		groupsize = 1;

	ngroups = len / groupsize;
	ascii_column = rowsize * 2 + rowsize / groupsize + 1;

	if (!linebuflen)
		goto overflow1;

	if (!len)
		goto nil;

	if (groupsize == 8) {
		const u64 *ptr8 = buf;

		for (j = 0; j < ngroups; j++) {
			ret = snprintf(linebuf + lx, linebuflen - lx,
				       "%s%16.16llx", j ? " " : "",
				       get_unaligned(ptr8 + j));
			if (ret >= linebuflen - lx)
				goto overflow1;
			lx += ret;
		}
	} else if (groupsize == 4) {
		const u32 *ptr4 = buf;

		for (j = 0; j < ngroups; j++) {
			ret = snprintf(linebuf + lx, linebuflen - lx,
				       "%s%8.8x", j ? " " : "",
				       get_unaligned(ptr4 + j));
			if (ret >= linebuflen - lx)
				goto overflow1;
			lx += ret;
		}
	} else if (groupsize == 2) {
		const u16 *ptr2 = buf;

		for (j = 0; j < ngroups; j++) {
			ret = snprintf(linebuf + lx, linebuflen - lx,
				       "%s%4.4x", j ? " " : "",
				       get_unaligned(ptr2 + j));
			if (ret >= linebuflen - lx)
				goto overflow1;
			lx += ret;
		}
	} else {
		for (j = 0; j < len; j++) {
			if (linebuflen < lx + 2)
				goto overflow2;
			ch = ptr[j];
			linebuf[lx++] = hex_asc_hi(ch);
			if (linebuflen < lx + 2)
				goto overflow2;
			linebuf[lx++] = hex_asc_lo(ch);
			if (linebuflen < lx + 2)
				goto overflow2;
			linebuf[lx++] = ' ';
		}
		if (j)
			lx--;
	}
	if (!ascii)
		goto nil;

	while (lx < ascii_column) {
		if (linebuflen < lx + 2)
			goto overflow2;
		linebuf[lx++] = ' ';
	}
	for (j = 0; j < len; j++) {
		if (linebuflen < lx + 2)
			goto overflow2;
		ch = ptr[j];
		linebuf[lx++] = (isascii(ch) && isprint(ch)) ? ch : '.';
	}
nil:
	linebuf[lx] = '\0';
	return lx;
overflow2:
	linebuf[lx++] = '\0';
overflow1:
	return ascii ? ascii_column + len : (groupsize * 2 + 1) * ngroups - 1;
}

u32 ieee80211_channel_to_freq_khz(int chan, enum nl80211_band band)
{
	/* see 802.11 17.3.8.3.2 and Annex J
	 * there are overlapping channel numbers in 5GHz and 2GHz bands */
	if (chan <= 0)
		return 0; /* not supported */
	switch (band) {
	case NL80211_BAND_2GHZ:
	case NL80211_BAND_LC:
		if (chan == 14)
			return MHZ_TO_KHZ(2484);
		else if (chan < 14)
			return MHZ_TO_KHZ(2407 + chan * 5);
		break;
	case NL80211_BAND_5GHZ:
		if (chan >= 182 && chan <= 196)
			return MHZ_TO_KHZ(4000 + chan * 5);
		else
			return MHZ_TO_KHZ(5000 + chan * 5);
		break;
	case NL80211_BAND_6GHZ:
		/* see 802.11ax D6.1 27.3.23.2 */
		if (chan == 2)
			return MHZ_TO_KHZ(5935);
		if (chan <= 233)
			return MHZ_TO_KHZ(5950 + chan * 5);
		break;
	case NL80211_BAND_60GHZ:
		if (chan < 7)
			return MHZ_TO_KHZ(56160 + chan * 2160);
		break;
	case NL80211_BAND_S1GHZ:
		return 902000 + chan * 500;
	default:
		;
	}
	return 0; /* not supported */
}

int request_firmware(struct firmware **firmware_p, const char *name, void *device){
	int ret = 0;
	unsigned int size = 0;
	struct stat fs = {};

	ret = open(name, O_RDONLY);
	if(ret < 0){
		ret = -EINVAL;
		return ret;
	}
	fstat(&fs, ret);
	size = fs.st_size;
	memset(&fs, 0, sizeof(struct stat));
	(*firmware_p)->size = size;
	(*firmware_p)->data = malloc(size);

	return ret;
}


void *dmam_alloc_attrs(void *dev, size_t size, void *dma_handle, int gfp, unsigned long attrs){
	return NULL;
}

int cancel_work_sync(struct work_struct *work){
	long ret = 0;
	int cancel = 0;
	struct workqueue_struct *wq = NULL;

	if (work == NULL){
		return ret;
	}

	ret = atomic_load_explicit(&work->data, memory_order_acquire);
	if (ret == 0){
		return ret;
	}

	wq = (struct workqueue_struct *)(uintptr_t)(ret & ~(long)WORK_STRUCT_FLAG_MASK);

	if (wq == NULL){
		return ret;
	}

	pthread_mutex_lock(&wq->lock);

	if (work_flags(work) & WORK_STRUCT_PENDING) {
		if (work_is_linked(work)){
			list_del_init(&work->entry);
		}
		atomic_store_explicit(&work->data, 0, memory_order_release);
		pthread_cond_broadcast(&wq->cond);
		cancel = true;
	}

	pthread_mutex_unlock(&wq->lock);
	flush_work(work);
	return cancel;
}

void sock_efree(struct sk_buff *skb){

}

void schedule(void){
	struct task_struct *current_task = NULL;
	current_task = current;

	if(current_task == NULL){
		goto out;
	}

	pthread_mutex_lock(&current_task->lock);

	while (atomic_load(&current_task->__state) != TASK_RUNNING && !atomic_load(&current_task->should_stop)) {
		pthread_cond_wait(&current_task->cond, &current_task->lock);
	}

	pthread_mutex_unlock(&current_task->lock);
	out:
}

unsigned long _find_next_and_bit(const unsigned long *addr1, const unsigned long *addr2, unsigned long nbits, unsigned long start){
	return FIND_NEXT_BIT(addr1[idx] & addr2[idx], /* nop */, nbits, start);
}

int usb_bulk_msg(struct usb_device *usb_dev, unsigned int pipe, void *data, int len, int *actual_length, int timeout){
	int ret = 0;
	u8 ep = 0;

	ep = convert_usb_pipe_to_ep(pipe);

	ret = libusb_bulk_transfer(
		&director.usb_dev,
		ep,
		data,
		len,
		actual_length,
		timeout
	);
	return ret;
}

int kernel_read(struct file_layer *file, void *buf, size_t count, int *pos){
	int ret = 0;

	if(file == NULL || file->actual_fd <= 0){
		ret = -EINVAL;
		return ret;
	}

	lseek(file->actual_fd, *pos, SEEK_SET);
	ret = read(file->actual_fd, buf, count);
	return ret;
}

int pcie_capability_read_word(struct pci_dev *dev, int pos, u16 *val){
	return 0;
}

void *__kmalloc_cache_noprof(void *s, int gfpflags, size_t size){
	return (void *)malloc(size);
}

void wiphy_work_cancel(struct wiphy *wiphy, struct wiphy_work *work){
	struct ieee80211_local *local = NULL;

	local = &director.ieee80211_local;

	pthread_mutex_lock(&local->wiphy_work_queue_mutex);

	if (!list_empty(&work->entry)){
		list_del_init(&work->entry);
	}
	pthread_mutex_unlock(&local->wiphy_work_queue_mutex);
}

int param_set_charp(const char *val, const struct kernel_param *kp){
	size_t len = 0;
	size_t maxlen = 1024;

	len = strnlen(val, maxlen + 1);
	if (len == maxlen + 1) {
		pr_err("%s: string parameter too long\n", kp->name);
		return -EINVAL;
	}

	/*
	 * This is a hack. We can't kmalloc() in early boot, and we
	 * don't need to; this mangled commandline is preserved.
	 */
	*(const char **)kp->arg = val;

	return 0;
}

void bpf_trace_run2(void *link, u64 arg1, u64 arg2){

}

void __x86_indirect_thunk_r8(void){

}

void *devm_hwmon_device_register_with_groups(void *dev, const char *name, void *drvdata, void **groups) {
	return NULL;
}

int param_set_bint(const char *val, const struct kernel_param *kp){
	int ret = 0;
	bool v = false;
	/* Match bool exactly, by re-using it. */
	struct kernel_param boolkp = *kp;

	boolkp.arg = &v;

	ret = param_set_bool(val, &boolkp);
	if (ret == 0){
		*(int *)kp->arg = v;
	}
	return ret;
}

void ieee80211_key_replay(struct ieee80211_key_conf *keyconf){
	struct ieee80211_key *key = NULL;

	key = container_of(keyconf, struct ieee80211_key, conf);

	switch (key->conf.cipher) {
	case WLAN_CIPHER_SUITE_CCMP:
	case WLAN_CIPHER_SUITE_CCMP_256:
		key->u.ccmp.replays++;
		break;
	case WLAN_CIPHER_SUITE_AES_CMAC:
	case WLAN_CIPHER_SUITE_BIP_CMAC_256:
		key->u.aes_cmac.replays++;
		break;
	case WLAN_CIPHER_SUITE_BIP_GMAC_128:
	case WLAN_CIPHER_SUITE_BIP_GMAC_256:
		key->u.aes_gmac.replays++;
		break;
	case WLAN_CIPHER_SUITE_GCMP:
	case WLAN_CIPHER_SUITE_GCMP_256:
		key->u.gcmp.replays++;
		break;
	}
}

struct rhash_lock_head __rcu **__rht_bucket_nested(struct bucket_table *tbl, unsigned int hash){
	const unsigned int shift = PAGE_SHIFT - ilog2(sizeof(void *));
	unsigned int index = hash & ((1 << tbl->nest) - 1);
	unsigned int size = tbl->size >> tbl->nest;
	unsigned int subhash = hash;
	union nested_table *ntbl = NULL;

	ntbl = nested_table_top(tbl);
	ntbl = rht_dereference_bucket_rcu(ntbl[index].table, tbl, hash);
	subhash >>= tbl->nest;

	while (ntbl && size > (1 << shift)) {
		index = subhash & ((1 << shift) - 1);
		ntbl = rht_dereference_bucket_rcu(ntbl[index].table, tbl, hash);
		size >>= shift;
		subhash >>= shift;
	}

	if (ntbl == NULL){
		return NULL;
	}

	return &ntbl[subhash].bucket;

}

int pci_write_config_dword(const struct pci_dev *dev, int where, u32 val){
	return 0;
}

int param_get_invbool(char *buffer, const struct kernel_param *kp){
	return sprintf(buffer, "%c\n", (*(bool *)kp->arg) ? 'N' : 'Y');
}

struct wakeup_source *wakeup_source_register(struct device *dev, const char *name){
	int ret = 0;
	struct wakeup_source *ws = NULL;

	/* inline here the function 'wakeup_source_create' */
	ws = (struct wakeup_source *)malloc(sizeof(struct wakeup_source));
	memset(ws, 0, sizeof(struct wakeup_source));
	ws->name = strdup(name);

	if (ws != NULL) {
		/* inline also 'wakeup_source_add' */
		ws->active = false;
		list_add_rcu(&ws->entry, &director.wakeup_sources);
	}
	return ws;
}

int param_get_charp(char *buffer, const struct kernel_param *kp){
	return scnprintf(buffer, PAGE_SIZE, "%s\n", *((char **)kp->arg));
}

void skb_prepare_seq_read(struct sk_buff *skb, unsigned int from, unsigned int to, struct skb_seq_state *st){
	st->lower_offset = from;
	st->upper_offset = to;
	st->root_skb = st->cur_skb = skb;
	st->frag_idx = st->stepped_offset = 0;
	st->frag_data = NULL;
	st->frag_off = 0;
}

int pskb_expand_head(struct sk_buff *skb, int nhead, int ntail, int gfp_mask){
	long off = 0;
	unsigned int osize = 0;
	unsigned int size = 0;
	u8 *data = NULL;

	osize = skb_end_offset(skb);
	size = osize + nhead + ntail;

	if(nhead < 0){
		return -EINVAL;
	}

	if(skb_shared(skb)){
		return -ENOMEM;
	}

	skb_zcopy_downgrade_managed(skb);

	data = kmalloc_reserve(&size, gfp_mask, NUMA_NO_NODE, NULL);
	if (!data){
		goto nodata;
	}
	size = SKB_WITH_OVERHEAD(size);

	/* Copy only real data... and, alas, header. This should be
	 * optimized for the cases when header is void.
	 */
	memcpy(data + nhead, skb->head, skb_tail_pointer(skb) - skb->head);
	memcpy((struct skb_shared_info *)(data + size), skb_shinfo(skb), offsetof(struct skb_shared_info, frags[skb_shinfo(skb)->nr_frags]));

	/*
	 * if shinfo is shared we must drop the old head gracefully, but if it
	 * is not we can just drop the old head and let the existing refcount
	 * be since all we did is relocate the values
	 */
	if (skb_cloned(skb)) {
		if (skb_orphan_frags(skb, gfp_mask)){
			goto nofrags;
		}
		if (skb_zcopy(skb)){
			refcount_inc(&skb_uarg(skb)->refcnt);
		}
		for (int i = 0; i < skb_shinfo(skb)->nr_frags; i++){
			skb_frag_ref(skb, i);
		}

		if (skb_has_frag_list(skb)){
			skb_clone_fraglist(skb);
		}
		//skb_release_data(skb, SKB_CONSUMED);
	} else {
		skb_free_head(skb);
	}
	off = (data + nhead) - skb->head;

	skb->head     = data;
	skb->head_frag = 0;
	skb->data    += off;

	skb_set_end_offset(skb, size);
#ifdef NET_SKBUFF_DATA_USES_OFFSET
	off           = nhead;
#endif
	skb->tail	      += off;
	skb_headers_offset_update(skb, nhead);
	skb->cloned   = 0;
	skb->hdr_len  = 0;
	skb->nohdr    = 0;
	atomic_store(&skb_shinfo(skb)->dataref, 1);

	/* It is not generally safe to change skb->truesize.
	 * For the moment, we really care of rx path, or
	 * when skb is orphaned (not attached to a socket).
	 */
	if (!skb->sk || skb->destructor == sock_edemux){
		skb->truesize += size - osize;
	}
	return 0;

nofrags:
	if(data != NULL){
		kfree(data);
	}
nodata:
	return -ENOMEM;
}

struct sk_buff *napi_build_skb(void *data, unsigned int frag_size){
	struct sk_buff *skb = NULL;

	skb = __napi_build_skb(data, frag_size);

	if (likely(skb) && frag_size) {
		skb->head_frag = 1;
	}

	return skb;
}

int cfg80211_get_ies_channel_number(const u8 *ie, size_t ielen, enum nl80211_band band){
	const struct element *tmp;

	if (band == NL80211_BAND_6GHZ) {
		struct ieee80211_he_operation *he_oper;

		tmp = cfg80211_find_ext_elem(WLAN_EID_EXT_HE_OPERATION, ie,
					     ielen);
		if (tmp && tmp->datalen >= sizeof(*he_oper) &&
		    tmp->datalen >= ieee80211_he_oper_size(&tmp->data[1])) {
			const struct ieee80211_he_6ghz_oper *he_6ghz_oper;

			he_oper = (void *)&tmp->data[1];

			he_6ghz_oper = ieee80211_he_6ghz_oper(he_oper);
			if (!he_6ghz_oper)
				return -1;

			return he_6ghz_oper->primary;
		}
	} else if (band == NL80211_BAND_S1GHZ) {
		tmp = cfg80211_find_elem(WLAN_EID_S1G_OPERATION, ie, ielen);
		if (tmp && tmp->datalen >= sizeof(struct ieee80211_s1g_oper_ie)) {
			struct ieee80211_s1g_oper_ie *s1gop = (void *)tmp->data;

			return s1gop->oper_ch;
		}
	} else {
		tmp = cfg80211_find_elem(WLAN_EID_DS_PARAMS, ie, ielen);
		if (tmp && tmp->datalen == 1)
			return tmp->data[0];

		tmp = cfg80211_find_elem(WLAN_EID_HT_OPERATION, ie, ielen);
		if (tmp &&
		    tmp->datalen >= sizeof(struct ieee80211_ht_operation)) {
			struct ieee80211_ht_operation *htop = (void *)tmp->data;

			return htop->primary_chan;
		}
	}

	return -1;
}

void __kfree_skb(struct sk_buff *skb){
	int old = 0;
	bool free_head = false;
	if(skb == NULL){
		goto end;
	}

	if (skb->head != NULL) {
		old = atomic_fetch_sub(&skb_shinfo(skb)->dataref, 1);
		if(old == 1){
			free_head = true;
		}

		if (free_head && !skb->head_frag) {
			free(skb->head);
		}

		skb->head = NULL;
		skb->data = NULL;
		skb_set_tail_offset(skb, 0);
		skb_set_end_offset(skb, 0);
	}
	free(skb);
	end:
}

unsigned long __sw_hweight64(__u64 w){
#if BITS_PER_LONG == 32
	return __sw_hweight32((unsigned int)(w >> 32)) +
	       __sw_hweight32((unsigned int)w);
#elif BITS_PER_LONG == 64
#ifdef CONFIG_ARCH_HAS_FAST_MULTIPLIER
	w -= (w >> 1) & 0x5555555555555555ul;
	w =  (w & 0x3333333333333333ul) + ((w >> 2) & 0x3333333333333333ul);
	w =  (w + (w >> 4)) & 0x0f0f0f0f0f0f0f0ful;
	return (w * 0x0101010101010101ul) >> 56;
#else
	__u64 res = w - ((w >> 1) & 0x5555555555555555ul);
	res = (res & 0x3333333333333333ul) + ((res >> 2) & 0x3333333333333333ul);
	res = (res + (res >> 4)) & 0x0F0F0F0F0F0F0F0Ful;
	res = res + (res >> 8);
	res = res + (res >> 16);
	return (res + (res >> 32)) & 0x00000000000000FFul;
#endif
#endif
}

void iounmap(volatile void __iomem *addr){


}

void *kmemdup_noprof(const void *src, size_t len, int gfp){
	void *p = NULL;
	p = malloc(len);

	if(p != NULL){
		memcpy(p, src, len);
	}
}

void init_wait_entry(struct wait_queue_entry *wq_entry, int flags){
	wq_entry->flags = flags;
	wq_entry->private = 0;
	wq_entry->func = autoremove_wake_function;
	INIT_LIST_HEAD(&wq_entry->entry);
}

void __x86_indirect_thunk_rcx(void){

}

struct sk_buff *__alloc_skb(unsigned int size, int gfp_mask, int flags, int node){
	int ret = 0;
	unsigned char *head = NULL;
	struct sk_buff *allocated_skb = NULL;

	allocated_skb = (struct sk_buff *)malloc(SK_BUFF_STRUCT_SIZE);
	if(allocated_skb == NULL){
		ret = -ENOMEM;
		return ERR_PTR(ret);
	}

	memset(allocated_skb, 0, size);
	head = (unsigned char *)malloc(size + sizeof(struct skb_shared_info));
	if (head == NULL) {
		free(allocated_skb);
		ret = -ENOMEM;
		return ERR_PTR(ret);
	}

	allocated_skb->head = head;
	allocated_skb->data = head;
	skb_set_tail_offset(allocated_skb, 0);
	skb_set_end_offset(allocated_skb, size);

	allocated_skb->len = 0;
	allocated_skb->data_len = 0;
	allocated_skb->mac_len = 0;
	allocated_skb->hdr_len = 0;
	allocated_skb->cloned = 0;
	allocated_skb->nohdr = 0;
	allocated_skb->head_frag = 0;

	memset(skb_shinfo(allocated_skb), 0, sizeof(*skb_shinfo(allocated_skb)));
	atomic_store(&skb_shinfo(allocated_skb)->dataref, 1);
	refcount_set(&allocated_skb->users, 1);

	allocated_skb->truesize = (unsigned int)(SK_BUFF_STRUCT_SIZE + size + sizeof(struct skb_shared_info));

	return allocated_skb;
}


void __local_bh_enable_ip(unsigned long ip, unsigned int cnt){

}

char *mangle_path(char *s, const char *p, const char *esc){
	while (s <= p) {
		char c = *p++;
		if (!c) {
			return s;
		} else if (!strchr(esc, c)) {
			*s++ = c;
		} else if (s + 4 > p) {
			break;
		} else {
			*s++ = '\\';
			*s++ = '0' + ((c & 0300) >> 6);
			*s++ = '0' + ((c & 070) >> 3);
			*s++ = '0' + (c & 07);
		}
	}
	return NULL;
}

int64_t ktime_get(void){
	return (int64_t)nsec_now_monotonic();
}

char *devm_kasprintf(struct device *dev, int gfp, const char *fmt, ...){
	va_list ap;
	char *p;

	va_start(ap, fmt);
	p = devm_kvasprintf(dev, gfp, fmt, ap);
	va_end(ap);

	return p;
}

void cfg80211_report_wowlan_wakeup(void *vid, struct cfg80211_wowlan_wakeup *wowlan, int gfp){

}

void ieee80211_connection_loss(struct ieee80211_vif *vif){
	struct ieee80211_sub_if_data *sdata = NULL;
	struct ieee80211_local *local = NULL;

	local = &director.ieee80211_local;
	sdata = local->sdata;

	sdata->u.mgd.connection_loss = true;
	//wiphy_work_queue(hw->wiphy, &sdata->u.mgd.beacon_connection_loss_work);
}

struct workqueue_struct *alloc_workqueue_noprof(const char *fmt, unsigned int flags, int max_active, ...){
	struct workqueue_struct *wq = NULL;
	wq = alloc_workqueue_noprof(fmt, flags, max_active);
	return wq;
}

void wiphy_rfkill_start_polling(struct wiphy *wiphy){
	void (*rfkill_poll_cb)(struct rfkill *rfkill, void *data) = NULL;

	rfkill_poll_cb = search_similar_function("_rfkill_poll");

	if(rfkill_poll_cb == NULL){
		return;
	}

	director.rdev->rfkill_ops.poll = rfkill_poll_cb;
	director.rfkill->polling_paused = false;
	if (director.rfkill->suspended == true){
		return;
	}
	//director.rdev->rfkill_ops.poll();
}

void *devm_kmemdup(void *dev, const void *src, size_t len, int gfp){
	void *p = NULL;

	p = devm_kmalloc(dev, len, gfp);
	if (p != NULL){
		memcpy(p, src, len);
	}
	return p;
}

unsigned int ieee80211_hdrlen(__le16 fc){
	unsigned int hdrlen = 24;

	if (ieee80211_is_ext(fc)) {
		hdrlen = 4;
		goto out;
	}

	if (ieee80211_is_data(fc)) {
		if (ieee80211_has_a4(fc))
			hdrlen = 30;
		if (ieee80211_is_data_qos(fc)) {
			hdrlen += IEEE80211_QOS_CTL_LEN;
			if (ieee80211_has_order(fc))
				hdrlen += IEEE80211_HT_CTL_LEN;
		}
		goto out;
	}

	if (ieee80211_is_mgmt(fc)) {
		if (ieee80211_has_order(fc))
			hdrlen += IEEE80211_HT_CTL_LEN;
		goto out;
	}

	if (ieee80211_is_ctl(fc)) {
		/*
		 * ACK and CTS are 10 bytes, all others 16. To see how
		 * to get this condition consider
		 *   subtype mask:   0b0000000011110000 (0x00F0)
		 *   ACK subtype:    0b0000000011010000 (0x00D0)
		 *   CTS subtype:    0b0000000011000000 (0x00C0)
		 *   bits that matter:         ^^^      (0x00E0)
		 *   value of those: 0b0000000011000000 (0x00C0)
		 */
		if ((fc & cpu_to_le16(0x00E0)) == cpu_to_le16(0x00C0))
			hdrlen = 10;
		else
			hdrlen = 16;
	}
out:
	return hdrlen;
}

void *__kmalloc_noprof(size_t size, int flags){
	return malloc(size);
}

int __pci_register_driver(struct pci_driver *drv, struct module *owner, const char *mod_name){
	return 0;
}

void *pskb_put(struct sk_buff *skb, struct sk_buff *tail, int len){
	if (tail != skb) {
		skb->data_len += len;
		skb->len += len;
	}
	return skb_put(tail, len);
}

unsigned long hweight_long(unsigned long w){
	return sizeof(w) == 4 ? __sw_hweight32(w) : __sw_hweight64((__u64)w);
}


int simple_read_from_buffer(void __user *to, size_t count, loff_t *ppos, const void *from, size_t available){
	int ret = 0;
	loff_t pos = *ppos;

	if (pos < 0){
		ret = -EINVAL;
		return ret;
	}
	if (pos >= available || !count){
		return ret;
	}
	if (count > available - pos){
		count = available - pos;
	}
	ret = copy_to_user(to, from + pos, count);
	if (ret == count){
		ret = -EFAULT;
		return ret;
	}
	count -= ret;
	*ppos = pos + count;
	return count;
}

bool skb_partial_csum_set(struct sk_buff *skb, u16 start, u16 off){
	u32 csum_end = 0;
	u32 csum_start = 0;

	csum_end = (u32)start + (u32)off + sizeof(__sum16);
	csum_start = (skb->data - skb->head) + (u32)start;

	if (unlikely(csum_start >= (u16)~0U || csum_end > (skb->len - skb->data_len))) {
		return false;
	}
	skb->ip_summed = CHECKSUM_PARTIAL;
	skb->csum_start = csum_start;
	skb->csum_offset = off;
	skb->transport_header = csum_start;
	return true;
}

int pci_read_config_dword(const struct pci_dev *dev, int where, u32 *val){
	return 0;
}

void dev_kfree_skb_any_reason(struct sk_buff *skb, int reason){
	kfree_skb_reason(skb, reason);
}

void __x86_indirect_thunk_r11(void){

}

void __x86_indirect_thunk_rbx(void){

}

void *kmalloc_reserve(unsigned int *size, int flags, int node, bool *pfmemalloc){
	return __kmalloc_cache_noprof(NULL, flags, *size);
}

unsigned int skb_zerocopy_headlen(const struct sk_buff *from){
	unsigned int hlen = 0;

	if (!from->head_frag || skb_headlen(from) < L1_CACHE_BYTES || skb_shinfo(from)->nr_frags >= MAX_SKB_FRAGS) {
		hlen = skb_headlen(from);
		if (hlen == 0){
			hlen = from->len;
		}
	}

	if (skb_has_frag_list(from)){
		hlen = from->len;
	}
	return hlen;
}

void __warn_flushing_systemwide_wq(void){
	printk("WARNING: Flushing system-wide workqueues will be prohibited in near future.\n");
}

void param_free_charp(void *arg){

}

struct kobject *kobject_get(struct kobject *kobj){
	if (kobj) {
		kref_get(&kobj->kref);
	}
	return kobj;
}

void seq_pad(struct seq_file *m, char c){
	int size = m->pad_until - m->count;
	if (size > 0) {
		if (size + m->count > m->size) {
			m->count = m->size;
			return;
		}
		memset(m->buf + m->count, ' ', size);
		m->count += size;
	}
	if (c){
		seq_putc(m, c);
	}
}

void sock_edemux(struct sk_buff *skb){
	sock_gen_put(skb->sk);
}

__sum16 csum_ipv6_magic(struct in6_addr *saddr, struct in6_addr *daddr, __u32 len, __u8 proto, __wsum csum){
	int carry = 0;
	__u32 ulen = 0;
	__u32 uproto = 0;
	__u32 sum = (__force u32)csum;

	sum += (__force u32)saddr->s6_addr32[0];
	carry = (sum < (__force u32)saddr->s6_addr32[0]);
	sum += carry;

	sum += (__force u32)saddr->s6_addr32[1];
	carry = (sum < (__force u32)saddr->s6_addr32[1]);
	sum += carry;

	sum += (__force u32)saddr->s6_addr32[2];
	carry = (sum < (__force u32)saddr->s6_addr32[2]);
	sum += carry;

	sum += (__force u32)saddr->s6_addr32[3];
	carry = (sum < (__force u32)saddr->s6_addr32[3]);
	sum += carry;

	sum += (__force u32)daddr->s6_addr32[0];
	carry = (sum < (__force u32)daddr->s6_addr32[0]);
	sum += carry;

	sum += (__force u32)daddr->s6_addr32[1];
	carry = (sum < (__force u32)daddr->s6_addr32[1]);
	sum += carry;

	sum += (__force u32)daddr->s6_addr32[2];
	carry = (sum < (__force u32)daddr->s6_addr32[2]);
	sum += carry;

	sum += (__force u32)daddr->s6_addr32[3];
	carry = (sum < (__force u32)daddr->s6_addr32[3]);
	sum += carry;

	ulen = (__force u32)htonl((__u32) len);
	sum += ulen;
	carry = (sum < ulen);
	sum += carry;

	uproto = (__force u32)htonl(proto);
	sum += uproto;
	carry = (sum < uproto);
	sum += carry;

	return csum_fold((__force __wsum)sum);
}

bool is_acpi_device_node(void *fwnode){
	return false;
}

unsigned long _find_next_zero_bit(const unsigned long *addr, unsigned long nbits, unsigned long start){
	return FIND_NEXT_BIT(~addr[idx], /* nop */, nbits, start);
}

void __init_swait_queue_head(struct swait_queue_head *q, const char *name, void *key){
	INIT_LIST_HEAD(&q->task_list);
}

void *vmalloc_noprof(unsigned long size){
	return (void *)malloc(size);
}

int ieee80211_freq_khz_to_channel(u32 freq)
{
	/* TODO: just handle MHz for now */
	freq = KHZ_TO_MHZ(freq);

	/* see 802.11 17.3.8.3.2 and Annex J */
	if (freq == 2484)
		return 14;
	else if (freq < 2484)
		return (freq - 2407) / 5;
	else if (freq >= 4910 && freq <= 4980)
		return (freq - 4000) / 5;
	else if (freq < 5925)
		return (freq - 5000) / 5;
	else if (freq == 5935)
		return 2;
	else if (freq <= 45000) /* DMG band lower limit */
		/* see 802.11ax D6.1 27.3.22.2 */
		return (freq - 5950) / 5;
	else if (freq >= 58320 && freq <= 70200)
		return (freq - 56160) / 2160;
	else
		return 0;
}

struct net_device *alloc_netdev_dummy(int sizeof_priv){
	return alloc_netdev_mqs(sizeof_priv, "dummy#", 0, init_dummy_netdev, 1, 1);
}

int _printk(const char *fmt, ...){
	int ret = 0;
	va_list ap;

	if(fmt[0] == '0' || fmt[0] == '1' || fmt[0] == '2' || fmt[0] == '3' || fmt[0] == '4' || fmt[0] == '5' || fmt[0] == '6' || fmt[0] == '7' || fmt[1] == '0' ||
	   fmt[1] == '1' || fmt[1] == '2' || fmt[1] == '3' || fmt[1] == '4' || fmt[1] == '5' || fmt[1] == '6' || fmt[1] == '7'){
		fmt += 2;
	}

	va_start(ap, fmt);
	ret = vprintf(fmt, ap);
	va_end(ap);
	return ret;
}


void wiphy_free(void *wiphy){
	if(wiphy != NULL){
		free(wiphy);
	}
}

void rfkill_pause_polling(struct rfkill *rfkill){
	if (director.rdev->rfkill_ops.poll == NULL){
		return;
	}

	director.rfkill->polling_paused = true;
	//cancel_delayed_work_sync(&rfkill->poll_work);
}

void put_netmem(netmem_ref netmem){

}

struct sk_buff *build_skb_around(struct sk_buff *skb, void *data, unsigned int frag_size){
	if (unlikely(!skb)){
		return NULL;
	}
	__build_skb_around(skb, data, frag_size);

	if (frag_size) {
		skb->head_frag = 1;
	}
	return skb;
}

void __x86_indirect_thunk_r9(void){

}

void wiphy_work_flush(struct wiphy *wiphy, struct wiphy_work *work){
	bool run = false;
	struct ieee80211_local *local = NULL;

	local = &director.ieee80211_local;

	pthread_mutex_lock(&local->wiphy_work_queue_mutex);
	run = (work == NULL) || !list_empty(&work->entry);
	pthread_mutex_unlock(&local->wiphy_work_queue_mutex);

	if (run){
//		cfg80211_process_wiphy_works(NULL, work);
	}
}

int usb_control_msg_send(struct usb_device *dev, __u8 endpoint, __u8 request, __u8 requesttype, __u16 value, __u16 index, const void *driver_data, __u16 size, int timeout, int memflags){
	int ret = 0;
	unsigned int pipe = 0;
	u8 *data = NULL;

	data = kmemdup_noprof(driver_data, size, memflags);
	ret = libusb_control_transfer(
		&director.usb_dev,
		requesttype,
		request,
		value,
		index,
		data,
		size,
		timeout
	);

	kfree(data);
	return ret;
}


int cancel_delayed_work(struct delayed_work *dwork){
	int ret = 0;
	long flags = 0;
	bool cancel = false;
	struct work_struct *work = NULL;
	struct workqueue_struct *wq = NULL;

	if (dwork == NULL){
		return ret;
	}

	work = &dwork->work;
	wq = work_wq(work);

	if (wq == NULL){
		return ret;
	}

	pthread_mutex_lock(&wq->lock);
	flags = work_flags(work);

	if (flags & WORK_STRUCT_PENDING) {
		if (work_is_linked(work)){
			list_del_init(&work->entry);
		}
		atomic_store_explicit(&work->data, 0, memory_order_release);
		pthread_cond_broadcast(&wq->cond);
		cancel = true;
	}

	pthread_mutex_unlock(&wq->lock);
	return cancel;
}

struct ieee80211_sta *ieee80211_find_sta(struct ieee80211_vif *vif, const u8 *addr){
	struct sta_info *sta = NULL;

	if (vif == NULL){
		return NULL;
	}

	sta = sta_info_get_bss(NULL, addr);
	if (sta == NULL){
		return NULL;
	}

	if (sta->uploaded == NULL){
		return NULL;
	}

	return sta->sta;
}

unsigned long __find_nth_and_bit(const unsigned long *addr1, const unsigned long *addr2, unsigned long size, unsigned long n){
	return FIND_NTH_BIT(addr1[idx] & addr2[idx], size, n);
}

void refcount_warn_saturate(void *r, int t){

}

void __ubsan_handle_out_of_bounds(void *_data, void *index){

}

void _raw_spin_lock_bh(void *spin_lock){

}

void cfg80211_cqm_beacon_loss_notify(struct net_device *dev, int gfp){

}

void tasklet_setup(struct tasklet_struct *t, void (*callback)(struct tasklet_struct *)){
	t->next = NULL;
	t->state = 0;
	__atomic_store_n(&t->count, false, __ATOMIC_RELEASE);
	t->callback = callback;
	t->use_callback = true;
	t->data = 0;
}

int hex2bin(u8 *dst, const char *src, size_t count){
	while (count--) {
		int hi = hex_to_bin(*src++);
		int lo = hex_to_bin(*src++);

		if ((hi < 0) || (lo < 0))
			return -EINVAL;

		*dst++ = (hi << 4) | lo;
	}
	return 0;
}

void seq_vprintf(struct seq_file *m, const char *f, va_list args){
	int len = 0;

	if (m->count < m->size) {
		len = vsnprintf(m->buf + m->count, m->size - m->count, f, args);
		if (m->count + len < m->size) {
			m->count += len;
			return;
		}
	}
	m->count = m->size;
}


void napi_enable(struct napi_struct *n){

}

unsigned long _find_next_bit(const unsigned long *addr, unsigned long nbits, unsigned long start){
	return FIND_NEXT_BIT(addr[idx], /* nop */, nbits, start);
}

void bpf_trace_run1(void *link, u64 arg1){

}

struct file_layer *filp_open(const char *filename, int flags, u16 mode){
	int fd = 0;
	struct file_layer *ptr_file_table = NULL;
	struct file_layer *current_file_layer = NULL;

	fd = open(filename, flags, mode);
	if(fd < 0){
		printf("[!] failed to open file: %s\n", filename);
		return NULL;
	}

	/* associate 'current_file_layer' with a file ptr in the director */
	for(u8 current_file_idx = 0; current_file_idx < MAX_FILE_IDX; current_file_idx++){
		ptr_file_table = &director.file_table[current_file_idx];
		if(ptr_file_table->actual_fd != 0xFF){
			current_file_layer = ptr_file_table;
			current_file_layer->actual_fd = fd;
			break;
		}
	}

	return current_file_layer;
}

void sk_free(struct sock *sock){
	/*
	 * We subtract one from sk_wmem_alloc and can know if
	 * some packets are still in some tx queue.
	 * If not null, sock_wfree() will call __sk_free(sk) later
	 */
	if (refcount_dec_and_test(&sock->sk_wmem_alloc)){
		//__sk_free(sock);
		free(sock);
	}

}

void __x86_indirect_thunk_rdx(void){

}

void print_hex_dump(const char *level, const char *prefix_str, int prefix_type, int rowsize, int groupsize, const void *buf, size_t len, bool ascii){
	const u8 *ptr = buf;
	int i, linelen, remaining = len;
	unsigned char linebuf[32 * 3 + 2 + 32 + 1];

	if (rowsize != 16 && rowsize != 32)
		rowsize = 16;

	for (i = 0; i < len; i += rowsize) {
		linelen = min(remaining, rowsize);
		remaining -= rowsize;

		hex_dump_to_buffer(ptr + i, linelen, rowsize, groupsize,
				   linebuf, sizeof(linebuf), ascii);

		switch (prefix_type) {
		case DUMP_PREFIX_ADDRESS:
			printk("%s%s%p: %s\n",
			       level, prefix_str, ptr + i, linebuf);
			break;
		case DUMP_PREFIX_OFFSET:
			printk("%s%s%.8x: %s\n", level, prefix_str, i, linebuf);
			break;
		default:
			printk("%s%s%s\n", level, prefix_str, linebuf);
			break;
		}
	}
}

void init_completion(struct completion *x){
    atomic_store_explicit(&x->done, 0, memory_order_relaxed);
}

int __wake_up(struct wait_queue_head *wq_head, unsigned int mode, int nr_exclusive, void *key){
	int ret = 0;
	int wake_up_executed = 0;
	unsigned long flags = 0;
	struct wait_queue_entry *pos = NULL;
	struct wait_queue_entry *n = NULL;

	spin_lock(&wq_head->lock);

	list_for_each_entry_safe(pos, n, &wq_head->head, entry) {
		flags = pos->flags;

		if (pos->func != NULL){
			ret = pos->func(pos, mode, 0, key);
		}
		if (ret > 0 && (flags & WQ_FLAG_EXCLUSIVE)){
			wake_up_executed++;
		}
		if (nr_exclusive > 0 && wake_up_executed >= nr_exclusive){
			break;
		}
	}

	spin_unlock(&wq_head->lock);
	return wake_up_executed;
}

void _raw_spin_unlock_bh(void *spin_lock){

}

void __udelay(unsigned long usecs){
	__const_udelay(usecs * 0x10C7UL); /* 2**32 / 1000000 (rounded up) */
}


int kthread_stop(struct task_struct *kthread){
	int ret = 0;

	if (kthread == NULL){
		ret = -EINVAL;
		return ret;
	}

	atomic_store(&kthread->should_stop, true);
	wake_up_process(kthread);

	pthread_join(kthread->tid, NULL);
	ret = kthread->exit_code;
	return ret;
}

int queue_delayed_work_on(int cpu, struct workqueue_struct *wq, struct delayed_work *dwork, unsigned long delay){
	int ret = 0;
	long expected = 0;
	long desired = 0;

	if (wq == NULL || dwork == NULL){
		return ret;
	}

	if(delay == 0){
		ret = queue_work_on(cpu, wq, &dwork->work);
		return ret;
	}

	desired = work_encode(wq, WORK_STRUCT_PENDING | WORK_STRUCT_DELAYED);
	ret = atomic_compare_exchange_strong_explicit(&dwork->work.data, &expected, desired, memory_order_acq_rel, memory_order_relaxed);

	if(ret == 0){
		return ret;
	}

	pthread_mutex_lock(&wq->lock);

	if (wq->stop) {
		pthread_mutex_unlock(&wq->lock);
		atomic_store_explicit(&dwork->work.data, 0, memory_order_release);
		return ret;
	}

	dwork->expires = timespec_add_ms(wq_now(wq), delay);

	delayed_list_insert_sorted(wq, dwork);

	pthread_cond_signal(&wq->cond);
	pthread_mutex_unlock(&wq->lock);
	ret = true;
	return ret;
}

void __stack_chk_fail(void){

}

int param_set_invbool(const char *val, const struct kernel_param *kp){
	int ret = 0;
	bool boolval = false;
	struct kernel_param dummy = {};

	dummy.arg = &boolval;
	ret = param_set_bool(val, &dummy);
	if (ret == 0){
		*(bool *)kp->arg = !boolval;
	}
	return ret;
}

int pci_write_config_word(const struct pci_dev *dev, int where, u16 val){
	return 0;
}

int cancel_delayed_work_sync(struct delayed_work *dwork){
	int ret = 0;

	if (dwork == NULL){
		return ret;
	}

	ret = cancel_delayed_work(dwork);
	flush_work(&dwork->work);
	return ret;
}

void __tasklet_schedule(struct tasklet_struct *t){
	struct tasklet_head *head = NULL;

	head = &tasklet_global_queue;
	t->next = NULL;
	*head->tail = t;
	head->tail = &(t->next);

}


bool phy_check_valid(int speed, int duplex, unsigned long *features){
	/* inline 'phy_caps_valid' here */
	int capa = 0;
	struct link_capabilities {
		int speed;
		unsigned int duplex;
		__ETHTOOL_DECLARE_LINK_MODE_MASK(linkmodes);
	}link_caps[__LINK_CAPA_MAX] = {
		{ SPEED_10, DUPLEX_HALF, {0} }, /* LINK_CAPA_10HD */
		{ SPEED_10, DUPLEX_FULL, {0} }, /* LINK_CAPA_10FD */
		{ SPEED_100, DUPLEX_HALF, {0} }, /* LINK_CAPA_100HD */
		{ SPEED_100, DUPLEX_FULL, {0} }, /* LINK_CAPA_100FD */
		{ SPEED_1000, DUPLEX_HALF, {0} }, /* LINK_CAPA_1000HD */
		{ SPEED_1000, DUPLEX_FULL, {0} }, /* LINK_CAPA_1000FD */
		{ SPEED_2500, DUPLEX_FULL, {0} }, /* LINK_CAPA_2500FD */
		{ SPEED_5000, DUPLEX_FULL, {0} }, /* LINK_CAPA_5000FD */
		{ SPEED_10000, DUPLEX_FULL, {0} }, /* LINK_CAPA_10000FD */
		{ SPEED_20000, DUPLEX_FULL, {0} }, /* LINK_CAPA_20000FD */
		{ SPEED_25000, DUPLEX_FULL, {0} }, /* LINK_CAPA_25000FD */
		{ SPEED_40000, DUPLEX_FULL, {0} }, /* LINK_CAPA_40000FD */
		{ SPEED_50000, DUPLEX_FULL, {0} }, /* LINK_CAPA_50000FD */
		{ SPEED_56000, DUPLEX_FULL, {0} }, /* LINK_CAPA_56000FD */
		{ SPEED_100000, DUPLEX_FULL, {0} }, /* LINK_CAPA_100000FD */
		{ SPEED_200000, DUPLEX_FULL, {0} }, /* LINK_CAPA_200000FD */
		{ SPEED_400000, DUPLEX_FULL, {0} }, /* LINK_CAPA_400000FD */
		{ SPEED_800000, DUPLEX_FULL, {0} }, /* LINK_CAPA_800000FD */
		{ SPEED_1600000, DUPLEX_FULL, {0} }, /* LINK_CAPA_1600000FD */
	};

	capa = speed_duplex_to_capa(speed, duplex);

	if (capa < 0){
		return false;
	}
	return linkmode_intersects(link_caps[capa].linkmodes, features);
}

void skb_append(struct sk_buff *old, struct sk_buff *newsk, struct sk_buff_head *list){
	__skb_queue_after(list, old, newsk);
}

void _dev_err(const struct device *dev, const char *fmt, ...){
	fmt += 2;
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}

void sock_gen_put(struct sock *sock){
	if (!refcount_dec_and_test(&sock->sk_refcnt)){
		return;
	}

	if (sock != NULL){
		sk_free(sock);
	}
}

ssize_t cfg80211_defragment_element(const struct element *elem, const u8 *ies, size_t ieslen, u8 *data, size_t data_len, u8 frag_id) {
	const struct element *next = NULL;
	ssize_t copied;
	u8 elem_datalen;

	if (!elem){
		return -EINVAL;
	}

	/* elem might be invalid after the memmove */
	next = (void *)(elem->data + elem->datalen);
	elem_datalen = elem->datalen;

	if (elem->id == WLAN_EID_EXTENSION) {
		copied = elem->datalen - 1;

		if (data) {
			if (copied > data_len)
				return -ENOSPC;

			memmove(data, elem->data + 1, copied);
		}
	} else {
		copied = elem->datalen;

		if (data) {
			if (copied > data_len)
				return -ENOSPC;

			memmove(data, elem->data, copied);
		}
	}

	/* Fragmented elements must have 255 bytes */
	if (elem_datalen < 255)
		return copied;

	for (elem = next;
	     elem->data < ies + ieslen &&
		elem->data + elem->datalen <= ies + ieslen;
	     elem = next) {
		/* elem might be invalid after the memmove */
		next = (void *)(elem->data + elem->datalen);

		if (elem->id != frag_id)
			break;

		elem_datalen = elem->datalen;

		if (data) {
			if (copied + elem_datalen > data_len)
				return -ENOSPC;

			memmove(data + copied, elem->data, elem_datalen);
		}

		copied += elem_datalen;

		/* Only the last fragment may be short */
		if (elem_datalen != 255)
			break;
	}

	return copied;
}

u32 ieee80211_mandatory_rates(struct ieee80211_supported_band *sband){
	struct ieee80211_rate *bitrates = NULL;
	u32 mandatory_rates = 0;
	enum ieee80211_rate_flags mandatory_flag = 0;

	if (!sband){
		return 1;
	}

	if (sband->band == NL80211_BAND_2GHZ){
		mandatory_flag = IEEE80211_RATE_MANDATORY_B;
	}else{
		mandatory_flag = IEEE80211_RATE_MANDATORY_A;
	}
	bitrates = sband->bitrates;
	for (int i = 0; i < sband->n_bitrates; i++){
		if (bitrates[i].flags & mandatory_flag){
			mandatory_rates |= BIT(i);
		}
	}
	return mandatory_rates;
}

int pci_request_regions(struct pci_dev *pdev, const char *name){
	return 0;
}

struct sk_buff *skb_dequeue_tail(struct sk_buff_head *list){
	struct sk_buff *result = NULL;

	result = __skb_dequeue_tail(list);
	return result;
}

void skb_coalesce_rx_frag(struct sk_buff *skb, int i, int size, unsigned int truesize){
	skb_frag_t *frag = &skb_shinfo(skb)->frags[i];

	skb_frag_size_add(frag, size);
	skb->len += size;
	skb->data_len += size;
	skb->truesize += truesize;
}

struct sk_buff *skb_realloc_headroom(struct sk_buff *skb, unsigned int headroom){
	int delta = 0;
	struct sk_buff *skb2 = NULL;

	delta = headroom - skb_headroom(skb);

	if (delta <= 0){
		skb2 = pskb_copy(skb, GFP_ATOMIC);
	}else {
		skb2 = skb_clone(skb, GFP_ATOMIC);
		if (skb2 && pskb_expand_head(skb2, SKB_DATA_ALIGN(delta), 0, GFP_ATOMIC)) {
			kfree_skb(skb2);
			skb2 = NULL;
		}
	}
	return skb2;
}

void finish_wait(struct wait_queue_head *wq_head, struct wait_queue_entry *wq_entry){
	struct task_struct *task_struct = NULL;
	task_struct = current;

	spin_lock(&wq_head->lock);
	if (wq_entry_is_linked(wq_entry) == true){
		list_del_init(&wq_entry->entry);
	}
	spin_unlock(&wq_head->lock);

	if (task_struct != NULL){
		atomic_store(&task_struct->__state, TASK_RUNNING);
	}
}

void complete_all(struct completion *x){
        atomic_store_explicit(&x->done, UINT_MAX, memory_order_release);
        futex_wake(&x->done, 0, 0, 0);
}

void ieee80211_get_key_rx_seq(struct ieee80211_key_conf *keyconf, int tid, struct ieee80211_key_seq *seq){
	const u8 *pn = NULL;
	struct ieee80211_key *key = NULL;

	key = container_of(keyconf, struct ieee80211_key, conf);

	switch (key->conf.cipher) {
	case WLAN_CIPHER_SUITE_TKIP:
		if (tid < 0 || tid >= IEEE80211_NUM_TIDS){
			return;
		}
		seq->tkip.iv32 = key->u.tkip.rx[tid].iv32;
		seq->tkip.iv16 = key->u.tkip.rx[tid].iv16;
		break;
	case WLAN_CIPHER_SUITE_CCMP:
	case WLAN_CIPHER_SUITE_CCMP_256:
		if (tid < -1 || tid >= IEEE80211_NUM_TIDS){
			return;
		}
		if (tid < 0){
			pn = key->u.ccmp.rx_pn[IEEE80211_NUM_TIDS];
		}else{
			pn = key->u.ccmp.rx_pn[tid];
		}
		memcpy(seq->ccmp.pn, pn, IEEE80211_CCMP_PN_LEN);
		break;
	case WLAN_CIPHER_SUITE_AES_CMAC:
	case WLAN_CIPHER_SUITE_BIP_CMAC_256:
		if(tid != 0){
			return;
		}
		pn = key->u.aes_cmac.rx_pn;
		memcpy(seq->aes_cmac.pn, pn, IEEE80211_CMAC_PN_LEN);
		break;
	case WLAN_CIPHER_SUITE_BIP_GMAC_128:
	case WLAN_CIPHER_SUITE_BIP_GMAC_256:
		if (tid != 0){
			return;
		}
		pn = key->u.aes_gmac.rx_pn;
		memcpy(seq->aes_gmac.pn, pn, IEEE80211_GMAC_PN_LEN);
		break;
	case WLAN_CIPHER_SUITE_GCMP:
	case WLAN_CIPHER_SUITE_GCMP_256:
		if (tid < -1 || tid >= IEEE80211_NUM_TIDS){
			return;
		}
		if (tid < 0){
			pn = key->u.gcmp.rx_pn[IEEE80211_NUM_TIDS];
		}else{
			pn = key->u.gcmp.rx_pn[tid];
		}
		memcpy(seq->gcmp.pn, pn, IEEE80211_GCMP_PN_LEN);
		break;
	}
}

void __x86_indirect_thunk_rax(void){

}

int pci_alloc_irq_vectors(struct pci_dev *dev, unsigned int min_vecs, unsigned int max_vecs, unsigned int flags){
	return 0;
}

struct workqueue_struct *alloc_workqueue(const char *name, unsigned int flags, int max_active){
	int ret = 0;
	struct workqueue_struct *wq = NULL;
	pthread_condattr_t cattr = {};

	wq = (struct workqueue_struct *)malloc(sizeof(struct workqueue_struct));
	if (wq == NULL){
		return NULL;
	}

	memset(wq, 0, sizeof(struct workqueue_struct));
	snprintf(wq->name, sizeof(wq->name), "%s", name ? name : "wq");

	pthread_mutex_init(&wq->lock, NULL);

	pthread_condattr_init(&cattr);
	wq->clock_id = CLOCK_MONOTONIC;
	pthread_condattr_setclock(&cattr, wq->clock_id);
	pthread_cond_init(&wq->cond, &cattr);
	pthread_condattr_destroy(&cattr);

	INIT_LIST_HEAD(&wq->work_list);
	INIT_LIST_HEAD(&wq->delayed_list);

	ret = pthread_create(&wq->thread, NULL, worker_thread, wq);
	if(ret < 0){
		pthread_cond_destroy(&wq->cond);
		pthread_mutex_destroy(&wq->lock);
		free(wq);
		return NULL;
	}else{
		return wq;
	}
}

void devm_kfree(void *dev, void *p){
	if(p != NULL){
		for(u8 j = 0; j < MAX_KMEM_ARRAY_SIZE; j++){
			if(director.kmem_array[j] == p){
				director.kmem_array[j] = NULL;
				free(p);
			}
		}
	}
}

void *memchr_inv(const void *start, int c, size_t bytes){
	u8 value = c;
	u64 value64;
	unsigned int words, prefix;

	if (bytes <= 16)
		return check_bytes8(start, value, bytes);

	value64 = value;
#if defined(ARCH_HAS_FAST_MULTIPLIER) && BITS_PER_LONG == 64
	value64 *= 0x0101010101010101;
#elif defined(ARCH_HAS_FAST_MULTIPLIER)
	value64 *= 0x01010101;
	value64 |= value64 << 32;
#else
	value64 |= value64 << 8;
	value64 |= value64 << 16;
	value64 |= value64 << 32;
#endif

	prefix = (unsigned long)start % 8;
	if (prefix) {
		u8 *r;

		prefix = 8 - prefix;
		r = check_bytes8(start, value, prefix);
		if (r)
			return r;
		start += prefix;
		bytes -= prefix;
	}

	words = bytes / 8;

	while (words) {
		if (*(u64 *)start != value64)
			return check_bytes8(start, value, 8);
		start += 8;
		words--;
	}

	return check_bytes8(start, value, bytes % 8);
}

void __bitmap_xor(unsigned long *dst, unsigned long *bitmap1, unsigned long *bitmap2, unsigned int bits){
	unsigned int k;
	unsigned int nr = BITS_TO_LONGS(bits);

	for (k = 0; k < nr; k++){
		dst[k] = bitmap1[k] ^ bitmap2[k];
	}
}

void cfg80211_cqm_rssi_notify(struct net_device *dev, enum nl80211_cqm_rssi_threshold_event rssi_event, s32 rssi_level, int gfp){
	struct cfg80211_cqm_config *cqm_config = NULL;

	if (rssi_event != NL80211_CQM_RSSI_THRESHOLD_EVENT_LOW && rssi_event != NL80211_CQM_RSSI_THRESHOLD_EVENT_HIGH){
		return;
	}

	cqm_config = &director.ieee80211_local->cqm_config;
	if (cqm_config != NULL) {
		cqm_config->last_rssi_event_value = rssi_level;
		cqm_config->last_rssi_event_type = rssi_event;
		//wiphy_work_queue(wdev->wiphy, &wdev->cqm_rssi_work);
	}
}

void usb_enable_autosuspend(struct usb_device *udev){

}

unsigned long _inline_copy_from_user(void *to, const void *from, unsigned long n){
	memcpy(to, from, n);
}

int param_set_bool(const char *val, const struct kernel_param *kp){
	/* No equals means "set"... */
	if (!val){
		val = "1";
	}
	/* One of =[yYnN01] */
	return kstrtobool(val, kp->arg);
}

void ieee80211_gtk_rekey_notify(struct ieee80211_vif *vif, const u8 *bssid, const u8 *replay_ctr, int gfp){
	cfg80211_gtk_rekey_notify(NULL, bssid, replay_ctr, gfp);
}

int vscnprintf(char *buf, size_t size, const char *fmt, va_list args){
	int i;

	if (unlikely(!size))
		return 0;

	i = vsnprintf(buf, size, fmt, args);

	if (likely(i < size))
		return i;

	return size - 1;
}

u32 cfg80211_calculate_bitrate(struct rate_info *rate){
	if (rate->flags & RATE_INFO_FLAGS_MCS){
		return cfg80211_calculate_bitrate_ht(rate);
	}
	if (rate->flags & RATE_INFO_FLAGS_DMG){
		return cfg80211_calculate_bitrate_dmg(rate);
	}
	if (rate->flags & RATE_INFO_FLAGS_EXTENDED_SC_DMG){
		return cfg80211_calculate_bitrate_extended_sc_dmg(rate);
	}
	if (rate->flags & RATE_INFO_FLAGS_EDMG){
		return cfg80211_calculate_bitrate_edmg(rate);
	}
	if (rate->flags & RATE_INFO_FLAGS_VHT_MCS){
		return cfg80211_calculate_bitrate_vht(rate);
	}
	if (rate->flags & RATE_INFO_FLAGS_HE_MCS){
		return cfg80211_calculate_bitrate_he(rate);
	}
	if (rate->flags & RATE_INFO_FLAGS_EHT_MCS){
		return cfg80211_calculate_bitrate_eht(rate);
	}
	if (rate->flags & RATE_INFO_FLAGS_S1G_MCS){
		return cfg80211_calculate_bitrate_s1g(rate);
	}
	return rate->legacy;
}

struct ieee80211_hw *ieee80211_alloc_hw_nm(size_t priv_data_len, const struct ieee80211_ops *ops, const char *requested_name){
	int priv_size = 0;
	bool emulate_chanctx = false;
	struct wiphy *wiphy = NULL;
	struct ieee80211_local *local = NULL;

	if (!ops->tx || !ops->start || !ops->stop || !ops->config ||!ops->add_interface || !ops->remove_interface || !ops->configure_filter || !ops->wake_tx_queue){
		return NULL;
	}

	if (ops->sta_state && (ops->sta_add || ops->sta_remove)){
		return NULL;
	}

	if (!!ops->link_info_changed != !!ops->vif_cfg_changed || (ops->link_info_changed && ops->bss_info_changed)){
		return NULL;
	}

	/* check all or no channel context operations exist */
	if (!ops->add_chanctx || !ops->remove_chanctx || !ops->change_chanctx || !ops->assign_vif_chanctx || !ops->unassign_vif_chanctx){
		return NULL;
	}

	/* Ensure 32-byte alignment of our private data and hw private data.
	 * We use the wiphy priv data for both our ieee80211_local and for
	 * the driver's private data
	 *
	 * In memory it'll be like this:
	 *
	 * +-------------------------+
	 * | struct wiphy	    |
	 * +-------------------------+
	 * | struct ieee80211_local  |
	 * +-------------------------+
	 * | driver's private data   |
	 * +-------------------------+
	 *
	 */
	priv_size = ALIGN(sizeof(*local), 32) + priv_data_len; /* 'NETDEV_ALIGN' = 32 */

	//wiphy = wiphy_new_nm(&mac80211_config_ops, priv_size, requested_name);

	if (wiphy == NULL){
		return NULL;
	}

	wiphy->flags |= WIPHY_FLAG_NETNS_OK |
			WIPHY_FLAG_4ADDR_AP |
			WIPHY_FLAG_4ADDR_STATION |
			WIPHY_FLAG_REPORTS_OBSS |
			WIPHY_FLAG_OFFCHAN_TX;

	if (emulate_chanctx || ops->remain_on_channel){
		wiphy->flags |= WIPHY_FLAG_HAS_REMAIN_ON_CHANNEL;
	}

	wiphy->bss_param_support = WIPHY_BSS_PARAM_CTS_PROT |
				   WIPHY_BSS_PARAM_SHORT_PREAMBLE |
				   WIPHY_BSS_PARAM_SHORT_SLOT_TIME |
				   WIPHY_BSS_PARAM_BASIC_RATES |
				   WIPHY_BSS_PARAM_AP_ISOLATE |
				   WIPHY_BSS_PARAM_HT_OPMODE |
				   WIPHY_BSS_PARAM_P2P_CTWINDOW |
				   WIPHY_BSS_PARAM_P2P_OPPPS;
	wiphy->features |= NL80211_FEATURE_SK_TX_STATUS |
			   NL80211_FEATURE_SAE |
			   NL80211_FEATURE_HT_IBSS |
			   NL80211_FEATURE_VIF_TXPOWER |
			   NL80211_FEATURE_MAC_ON_CREATE |
			   NL80211_FEATURE_USERSPACE_MPM |
			   NL80211_FEATURE_FULL_AP_CLIENT_STATE;
	wiphy_ext_feature_set(wiphy, NL80211_EXT_FEATURE_FILS_STA);
	wiphy_ext_feature_set(wiphy,
			      NL80211_EXT_FEATURE_CONTROL_PORT_OVER_NL80211);
	wiphy_ext_feature_set(wiphy,
			      NL80211_EXT_FEATURE_CONTROL_PORT_NO_PREAUTH);
	wiphy_ext_feature_set(wiphy,
			      NL80211_EXT_FEATURE_CONTROL_PORT_OVER_NL80211_TX_STATUS);
	wiphy_ext_feature_set(wiphy,
			      NL80211_EXT_FEATURE_SCAN_FREQ_KHZ);
	wiphy_ext_feature_set(wiphy,
			      NL80211_EXT_FEATURE_POWERED_ADDR_CHANGE);

	if (!ops->hw_scan) {
		wiphy->features |= NL80211_FEATURE_LOW_PRIORITY_SCAN |
				   NL80211_FEATURE_AP_SCAN;
		/*
		 * if the driver behaves correctly using the probe request
		 * (template) from mac80211, then both of these should be
		 * supported even with hw scan - but let drivers opt in.
		 */
		wiphy_ext_feature_set(wiphy,
				      NL80211_EXT_FEATURE_SCAN_RANDOM_SN);
		wiphy_ext_feature_set(wiphy,
				      NL80211_EXT_FEATURE_SCAN_MIN_PREQ_CONTENT);
	}

	if (!ops->set_key) {
		wiphy->flags |= WIPHY_FLAG_IBSS_RSN;
	//	wiphy_ext_feature_set(wiphy, NL80211_EXT_FEATURE_SPP_AMSDU_SUPPORT);
	}

	wiphy_ext_feature_set(wiphy, NL80211_EXT_FEATURE_TXQS);
	wiphy_ext_feature_set(wiphy, NL80211_EXT_FEATURE_RRM);

	wiphy->bss_priv_size = sizeof(struct ieee80211_bss);

	//local = wiphy->priv;
	local = &director.ieee80211_local;

	if (sta_info_init(local)){
		goto err_free;
	}

	local->hw.wiphy = wiphy;

	local->hw.priv = (char *)local + ALIGN(sizeof(*local), 32); /** 'NETDEV_ALIGN' = 32 */

	local->ops = ops;
	local->emulate_chanctx = emulate_chanctx;

	/*
	 * We need a bit of data queued to build aggregates properly, so
	 * instruct the TCP stack to allow more than a single ms of data
	 * to be queued in the stack. The value is a bit-shift of 1
	 * second, so 7 is ~8ms of queued data. Only affects local TCP
	 * sockets.
	 * This is the default, anyhow - drivers may need to override it
	 * for local reasons (longer buffers, longer completion time, or
	 * similar).
	 */
	local->hw.tx_sk_pacing_shift = 7;

	/* set up some defaults */
	local->hw.queues = 1;
	local->hw.max_rates = 1;
	local->hw.max_report_rates = 0;
	local->hw.max_rx_aggregation_subframes = IEEE80211_MAX_AMPDU_BUF_HT;
	local->hw.max_tx_aggregation_subframes = IEEE80211_MAX_AMPDU_BUF_HT;
	local->hw.offchannel_tx_hw_queue = IEEE80211_INVAL_HW_QUEUE;
	local->hw.conf.long_frame_max_tx_count = wiphy->retry_long;
	local->hw.conf.short_frame_max_tx_count = wiphy->retry_short;
	local->hw.radiotap_mcs_details = IEEE80211_RADIOTAP_MCS_HAVE_MCS |
					 IEEE80211_RADIOTAP_MCS_HAVE_GI |
					 IEEE80211_RADIOTAP_MCS_HAVE_BW;
	local->hw.radiotap_vht_details = IEEE80211_RADIOTAP_VHT_KNOWN_GI |
					 IEEE80211_RADIOTAP_VHT_KNOWN_BANDWIDTH;
	local->hw.uapsd_queues = IEEE80211_DEFAULT_UAPSD_QUEUES;
	local->hw.uapsd_max_sp_len = IEEE80211_DEFAULT_MAX_SP_LEN;
	local->hw.max_mtu = IEEE80211_MAX_DATA_LEN;
	local->user_power_level = IEEE80211_UNSET_POWER_LEVEL;

	local->ext_capa[7] = WLAN_EXT_CAPA8_OPMODE_NOTIF;

	INIT_LIST_HEAD(&local->interfaces);
	INIT_LIST_HEAD(&local->mon_list);

	pthread_mutex_init(&local->iflist_mtx, NULL);

	local->airtime_flags = AIRTIME_USE_TX | AIRTIME_USE_RX;
	local->aql_threshold = IEEE80211_AQL_THRESHOLD;
	atomic_store(&local->aql_total_pending_airtime, 0);

	INIT_LIST_HEAD(&local->chanctx_list);

	for (int i = 0; i < IEEE80211_MAX_QUEUES; i++) {
		skb_queue_head_init(&local->pending[i]);
		atomic_store(&local->agg_queue_stop[i], 0);
	}

	skb_queue_head_init(&local->skb_queue);
	skb_queue_head_init(&local->skb_queue_unreliable);

	local->hw.radiotap_timestamp.units_pos = -1;
	local->hw.radiotap_timestamp.accuracy = -1;

	return &local->hw;
 err_free:
	wiphy_free(wiphy);
	return NULL;
}

void __check_object_size(const void *ptr, unsigned long n, bool to_user){

}

void ieee80211_report_wowlan_wakeup(void *vif, struct cfg80211_wowlan_wakeup *wakeup, int gfp){
	cfg80211_report_wowlan_wakeup(NULL, wakeup, gfp);
}

int simple_write_to_buffer(void *to, size_t available, loff_t *ppos, const void __user *from, size_t count){
	int ret = 0;
	loff_t pos = *ppos;

	if (pos < 0){
		ret = -EINVAL;
		return ret;
	}
	if (pos >= available || !count){
		return ret;
	}
	if (count > available - pos){
		count = available - pos;
	}
	ret = copy_from_user(to + pos, from, count);
	if (ret == count){
		ret = -EFAULT;
		return ret;
	}
	count -= ret;
	*ppos = pos + count;
	return count;
}

int kstrtobool(const char *s, bool *res){
	if (!s){
		return -EINVAL;
	}
	switch (s[0]) {
	case 'e':
	case 'E':
	case 'y':
	case 'Y':
	case 't':
	case 'T':
	case '1':
		*res = true;
		return 0;
	case 'd':
	case 'D':
	case 'n':
	case 'N':
	case 'f':
	case 'F':
	case '0':
		*res = false;
		return 0;
	case 'o':
	case 'O':
		switch (s[1]) {
		case 'n':
		case 'N':
			*res = true;
			return 0;
		case 'f':
		case 'F':
			*res = false;
			return 0;
		default:
			break;
		}
		break;
	default:
		break;
	}

	return -EINVAL;
}

unsigned int __sw_hweight32(unsigned int w){
#ifdef CONFIG_ARCH_HAS_FAST_MULTIPLIER
	w -= (w >> 1) & 0x55555555;
	w =  (w & 0x33333333) + ((w >> 2) & 0x33333333);
	w =  (w + (w >> 4)) & 0x0f0f0f0f;
	return (w * 0x01010101) >> 24;
#else
	unsigned int res = w - ((w >> 1) & 0x55555555);
	res = (res & 0x33333333) + ((res >> 2) & 0x33333333);
	res = (res + (res >> 4)) & 0x0F0F0F0F;
	res = res + (res >> 8);
	return (res + (res >> 16)) & 0x000000FF;
#endif
}

int nl80211_chan_width_to_mhz(enum nl80211_chan_width chan_width){
	int mhz = 0;

	switch (chan_width) {
	case NL80211_CHAN_WIDTH_1:
		mhz = 1;
		break;
	case NL80211_CHAN_WIDTH_2:
		mhz = 2;
		break;
	case NL80211_CHAN_WIDTH_4:
		mhz = 4;
		break;
	case NL80211_CHAN_WIDTH_8:
		mhz = 8;
		break;
	case NL80211_CHAN_WIDTH_16:
		mhz = 16;
		break;
	case NL80211_CHAN_WIDTH_5:
		mhz = 5;
		break;
	case NL80211_CHAN_WIDTH_10:
		mhz = 10;
		break;
	case NL80211_CHAN_WIDTH_20:
	case NL80211_CHAN_WIDTH_20_NOHT:
		mhz = 20;
		break;
	case NL80211_CHAN_WIDTH_40:
		mhz = 40;
		break;
	case NL80211_CHAN_WIDTH_80P80:
	case NL80211_CHAN_WIDTH_80:
		mhz = 80;
		break;
	case NL80211_CHAN_WIDTH_160:
		mhz = 160;
		break;
	case NL80211_CHAN_WIDTH_320:
		mhz = 320;
		break;
	default:
		return -1;
	}
	return mhz;
}

void get_random_bytes(void *buf, size_t len){
	u32 val = 0;

	if(buf == NULL || len == 0){
		return;
	}

	for(u16 j = 0; j < len; j += sizeof(u32)){
		val = get_random_u32();
		memcpy(buf + j, &val, sizeof(u32));
	}
}

unsigned int jiffies_to_usecs(const unsigned long j){
#if !(USEC_PER_SEC % HZ)
	return (USEC_PER_SEC / HZ) * j;
#else
# if BITS_PER_LONG == 32
	return (HZ_TO_USEC_MUL32 * j) >> HZ_TO_USEC_SHR32;
# else
	return (j * HZ_TO_USEC_NUM) / HZ_TO_USEC_DEN;
# endif
#endif
}

unsigned long __msecs_to_jiffies(const unsigned int m){
	if ((int)m < 0){
		return MAX_JIFFY_OFFSET;
	}
	return _msecs_to_jiffies(m);
}

void rb_insert_color(struct rb_node *node, struct rb_root *root){
	__rb_insert(node, root, NULL);
}

unsigned long _copy_to_user(void __user *to, const void *from, unsigned long n){
	return _inline_copy_to_user(to, from, n);
}

int printk(const char *fmt, ...){
	int ret = 0;

        if(fmt[1] == '0' || fmt[1] == '1' || fmt[1] == '2' || fmt[1] == '3' || fmt[1] == '4' || fmt[1] == '5' || fmt[1] == '6' || fmt[1] == '7'){
                fmt += 2;
        }

	va_list ap;
	va_start(ap, fmt);
	ret = vprintf(fmt, ap);
	va_end(ap);
	return ret;
}

void __const_udelay(unsigned long xloops){
	arch__const_udelay(xloops);
}

void usb_autopm_put_interface(struct usb_interface *intf){

}

void *__kmalloc_large_noprof(size_t size, int flags){
	return (void *)malloc(size);
}

int kstrtouint(const char *s, unsigned int base, unsigned int *res){
	*res = strtol(s, NULL, base);
	return 0;
}

int futex_wait(atomic_int *addr, int expected){
	return (int)syscall(SYS_futex, (int *)addr, FUTEX_WAIT, expected, NULL, NULL, 0);
}

int filp_close(struct file_layer *filp, void *id){
	int ret = 0;
	if(filp != NULL){
		close(filp->actual_fd);
		filp->actual_fd = 0xFF;
		filp->actual_offset = 0;
		filp->original_file_ptr = NULL;
		return ret;
	}else{
		ret = -EINVAL;
		return ret;
	}
}

void kfree(void *addr){
	if(addr != NULL){
		free(addr);
	}
}

int kstrtoint(const char *s, unsigned int base, int *res){
	*res = strtol(s, NULL, base);
	return 0;
}

void __x86_indirect_thunk_rbp(void){

}

struct rhash_lock_head __rcu **rht_bucket_nested(struct bucket_table *tbl, unsigned int hash){
	static struct rhash_lock_head __rcu *rhnull = NULL;

	if (rhnull == NULL){
		INIT_RHT_NULLS_HEAD(rhnull);
	}
	return __rht_bucket_nested(tbl, hash) ?: &rhnull;
}

void init_timer_key(struct timer_list *timer, void (*func)(struct timer_list *), unsigned int flags, const char *name, void *key){
	timer->entry.pprev = NULL;
	timer->function = func;
	if ((flags & ~TIMER_INIT_FLAGS)){
		flags &= TIMER_INIT_FLAGS;
	}
}

struct urb *usb_get_urb(struct urb *urb){
	if (urb != NULL){
		kref_get(&urb->kref);
	}
	return urb;
}

size_t cfg80211_merge_profile(const u8 *ie, size_t ielen, const struct element *mbssid_elem, const struct element *sub_elem, u8 *merged_ie, size_t max_copy_len){
	size_t copied_len = sub_elem->datalen;
	const struct element *next_mbssid = NULL;

	if (sub_elem->datalen > max_copy_len){
		return 0;
	}

	memcpy(merged_ie, sub_elem->data, sub_elem->datalen);

	while ((next_mbssid = cfg80211_get_profile_continuation(ie, ielen, mbssid_elem, sub_elem))) {
		const struct element *next_sub = (void *)&next_mbssid->data[1];

		if (copied_len + next_sub->datalen > max_copy_len){
			break;
		}
		memcpy(merged_ie + copied_len, next_sub->data, next_sub->datalen);
		copied_len += next_sub->datalen;
	}
	return copied_len;
}

void _dev_warn(const struct device *dev, const char *fmt, ...){
	fmt += 2;
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}

void __x86_indirect_thunk_rdi(void){

}

void destroy_workqueue(struct workqueue_struct *wq){
	struct list_head *pos = NULL;
	struct list_head *n = NULL;
	struct work_struct *work = NULL;

	if(wq == NULL){
		goto out;
	}

	pthread_mutex_lock(&wq->lock);

	list_for_each_safe(pos, n, &wq->work_list) {
		work = container_of(pos, struct work_struct, entry);
		list_del_init(&work->entry);
		atomic_store_explicit(&work->data, 0, memory_order_release);
	}

	list_for_each_safe(pos, n, &wq->delayed_list) {
		work = container_of(pos, struct work_struct, entry);
		list_del_init(&work->entry);
		atomic_store_explicit(&work->data, 0, memory_order_release);
	}

	wq->stop = true;
	pthread_cond_broadcast(&wq->cond);
	pthread_mutex_unlock(&wq->lock);

	pthread_join(wq->thread, NULL);

	pthread_cond_destroy(&wq->cond);
	pthread_mutex_destroy(&wq->lock);
	free(wq);

	out:
}

void msleep(unsigned int msecs){
	struct timespec req = {};
	struct timespec rem = {};

	req.tv_sec  = (time_t)(msecs / 1000U);
	req.tv_nsec = (long)((msecs % 1000U) * 1000000UL);

	while (nanosleep(&req, &rem) == -1 && errno == EINTR) {
		req = rem;
	}
}

int param_get_bool(char *buffer, const struct kernel_param *kp){
	/* Y and N chosen as being relatively non-coder friendly */
	return sprintf(buffer, "%c\n", *(bool *)kp->arg ? 'Y' : 'N');
}

unsigned int __sw_hweight16(unsigned int w){
	unsigned int res = w - ((w >> 1) & 0x5555);
	res = (res & 0x3333) + ((res >> 2) & 0x3333);
	res = (res + (res >> 4)) & 0x0F0F;
	return (res + (res >> 8)) & 0x00FF;
}

void usb_disable_autosuspend(struct usb_device *udev){

}

unsigned long _find_first_zero_bit(const unsigned long *addr, unsigned long size){
	return FIND_FIRST_BIT(~addr[idx], /* nop */, size);
}

void delayed_work_timer_fn(struct workqueue_struct *wq){
        struct timespec now = {};

        now = wq_now(wq);

        while (!list_empty(&wq->delayed_list)) {
                struct delayed_work *dwork = NULL;
                struct work_struct *work = NULL;

                work = list_first_entry(&wq->delayed_list, struct work_struct, entry);
                dwork = container_of(work, struct delayed_work, work);

                if (timespec_cmp(&dwork->expires, &now) > 0){
                        break;
                }

                list_del_init(&work->entry);

                atomic_store_explicit(&work->data, work_encode(wq, WORK_STRUCT_PENDING), memory_order_relaxed);
                list_add_tail(&work->entry, &wq->work_list);
        }
}

unsigned long _inline_copy_to_user(void *to, void *from, unsigned long n){
	int ret = 0;
	if(n < sizeof(director.driver_to_userspace_buffer)){
		memcpy(&director.driver_to_userspace_buffer, from, n);
		return ret;
	}else{
		memset(&director.driver_to_userspace_buffer, 0xFF, sizeof(director.driver_to_userspace_buffer));
		ret = -EINVAL;
		return ret;
	}
}

void mutex_unlock(pthread_mutex_t *lock){
	pthread_mutex_unlock(lock);
}

void wiphy_work_queue(struct wiphy *wiphy, struct wiphy_work *work){
	struct ieee80211_local *local = NULL;

	local = &director.ieee80211_local;

	pthread_mutex_lock(&local->wiphy_work_queue_mutex);
	if (list_empty(&work->entry)){
		list_add_tail(&work->entry, &local->wiphy_work_list);
	}
	pthread_mutex_unlock(&local->wiphy_work_queue_mutex);

	queue_work(&local->system_dfl_wq, work);
}

const void *dmi_first_match(const void *list){
	return NULL;
}

void mutex_lock(pthread_mutex_t *lock){
	pthread_mutex_lock(lock);
}

struct task_struct *kthread_create_on_node(int (*kthread_function)(void *data), void *data, int node, unsigned char *thread_name){
	int ret = 0;
	pthread_condattr_t current_cond_attr = {};
	struct task_struct *current_thread = NULL;

	current_thread = (struct task_struct *)malloc(sizeof(struct task_struct));

	if (current_thread == NULL){
		return ERR_PTR(-ENOMEM);
	}

	pthread_mutex_init(&current_thread->lock, NULL);

	pthread_condattr_init(&current_cond_attr);
	pthread_condattr_setclock(&current_cond_attr, CLOCK_MONOTONIC);
	pthread_cond_init(&current_thread->cond, &current_cond_attr);
	pthread_condattr_destroy(&current_cond_attr);

	atomic_store(&current_thread->__state, TASK_RUNNING);
	atomic_store(&current_thread->started, false);
	atomic_store(&current_thread->should_stop, false);

	current_thread->threadfn = kthread_function;
	current_thread->data = data;

	strncpy(current_thread->name, thread_name, sizeof(current_thread->name));
	printf("current thread name: %s\n", current_thread->name);

	/* do not call directly the callback but instead use a special function which acts as a 'trampoline' for invoking the cb */
	ret = pthread_create(&current_thread->tid, NULL, kthread_trampoline, current_thread);
	if(ret != 0){
		pthread_cond_destroy(&current_thread->cond);
		pthread_mutex_destroy(&current_thread->lock);
		memset(current_thread, 0, sizeof(struct task_struct));
		free(current_thread);
        	return ERR_PTR(-EINVAL);
	}

	return current_thread;
}

__wsum csum_partial(const void *buff, int len, __wsum wsum){
	unsigned int sum = (__force unsigned int)wsum;
	unsigned int result = do_csum(buff, len);

	/* add in old sum, and carry.. */
	result += sum;
	if (sum > result){
		result += 1;
	}
	return (__force __wsum)result;
}

struct sk_buff *build_skb(void *data, unsigned int frag_size){
	struct sk_buff *skb = NULL;

	skb = __build_skb(data, frag_size);

	if (likely(skb && frag_size)) {
		skb->head_frag = 1;
	}
	return skb;
}

int scnprintf(char *buf, size_t size, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vscnprintf(buf, size, fmt, args);
	va_end(args);

	return i;
}

int register_chrdev_region(u32 from, unsigned count, const char *name){
	int ret = 0;
	return ret;
}

void __dynamic_dev_dbg(void *descriptor, void *dev, const char *fmt, ...){
	va_list ap;

	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}

int vfs_llseek(struct file_layer *file, u32 offset, int whence){
	int ret = 0;
	if(file == NULL || file->actual_fd <= 0){
		ret = -EINVAL;
		return ret;
	}

	ret = lseek(file->actual_fd, offset, whence);
	return ret;
}

int futex_wake(atomic_int *addr, int flags, int nr_wake, u32 bitset){
	return (int)syscall(SYS_futex, (int *)addr, FUTEX_WAKE, flags, NULL, NULL, 0);
}

void __ubsan_handle_load_invalid_value(void *_data, void *val){

}

void pci_release_regions(struct pci_dev *pdev){

}

struct sk_buff *skb_clone_sk(struct sk_buff *skb){
	struct sk_buff *clone;

	clone = skb_clone(skb, GFP_ATOMIC);

	clone->destructor = sock_efree;

	return clone;
}

void __x86_indirect_thunk_rsi(void){

}

int acpi_get_handle(void *parent, const char *pathname, void *ret_handle){
	return 0;
}


void _raw_spin_lock_irq(void *lock){

}

void ieee80211_set_key_rx_seq(struct ieee80211_key_conf *keyconf, int tid, struct ieee80211_key_seq *seq){
	u8 *pn = NULL;
	struct ieee80211_key *key = NULL;

	key = container_of(keyconf, struct ieee80211_key, conf);

	switch (key->conf.cipher) {
	case WLAN_CIPHER_SUITE_TKIP:
		if (tid < 0 || tid >= IEEE80211_NUM_TIDS){
			return;
		}
		key->u.tkip.rx[tid].iv32 = seq->tkip.iv32;
		key->u.tkip.rx[tid].iv16 = seq->tkip.iv16;
		break;
	case WLAN_CIPHER_SUITE_CCMP:
	case WLAN_CIPHER_SUITE_CCMP_256:
		if (tid < -1 || tid >= IEEE80211_NUM_TIDS){
			return;
		}
		if (tid < 0){
			pn = key->u.ccmp.rx_pn[IEEE80211_NUM_TIDS];
		}else{
			pn = key->u.ccmp.rx_pn[tid];
		}
		memcpy(pn, seq->ccmp.pn, IEEE80211_CCMP_PN_LEN);
		break;
	case WLAN_CIPHER_SUITE_AES_CMAC:
	case WLAN_CIPHER_SUITE_BIP_CMAC_256:
		if (tid != 0){
			return;
		}
		pn = key->u.aes_cmac.rx_pn;
		memcpy(pn, seq->aes_cmac.pn, IEEE80211_CMAC_PN_LEN);
		break;
	case WLAN_CIPHER_SUITE_BIP_GMAC_128:
	case WLAN_CIPHER_SUITE_BIP_GMAC_256:
		if (tid != 0){
			return;
		}
		pn = key->u.aes_gmac.rx_pn;
		memcpy(pn, seq->aes_gmac.pn, IEEE80211_GMAC_PN_LEN);
		break;
	case WLAN_CIPHER_SUITE_GCMP:
	case WLAN_CIPHER_SUITE_GCMP_256:
		if (tid < -1 || tid >= IEEE80211_NUM_TIDS){
			return;
		}
		if (tid < 0){
			pn = key->u.gcmp.rx_pn[IEEE80211_NUM_TIDS];
		}else{
			pn = key->u.gcmp.rx_pn[tid];
		}
		memcpy(pn, seq->gcmp.pn, IEEE80211_GCMP_PN_LEN);
		break;
	default:
		break;
	}
}

int hex_to_bin(char ch){
	if ((ch >= '0') && (ch <= '9'))
		return ch - '0';
	ch = tolower(ch);
	if ((ch >= 'a') && (ch <= 'f'))
		return ch - 'a' + 10;
	return -1;
}

bool __bitmap_intersects(unsigned long *bitmap1, unsigned long *bitmap2, unsigned int bits){
	unsigned int k, lim = bits/BITS_PER_LONG;
	for (k = 0; k < lim; ++k){
		if (bitmap1[k] & bitmap2[k]){
			return true;
		}
	}

	if (bits % BITS_PER_LONG){
		if ((bitmap1[k] & bitmap2[k]) & BITMAP_LAST_WORD_MASK(bits)){
			return true;
		}
	}

	return false;
}

void timer_init_key(struct timer_list *timer, void (*func)(struct timer_list *), unsigned int flags, const char *name, struct lock_class_key *key){
	/* inline 'do_init_timer' here */
	timer->entry.pprev = NULL;
	timer->function = func;
	if (flags & ~TIMER_INIT_FLAGS){
		flags &= TIMER_INIT_FLAGS;
	}
	timer->flags = flags;
}

void pci_set_master(struct pci_dev *dev){

}

unsigned long __find_nth_bit(const unsigned long *addr, unsigned long size, unsigned long n){
	return FIND_NTH_BIT(addr[idx], size, n);
}

void ieee80211_disconnect(struct ieee80211_vif *vif, bool reconnect){
	struct ieee80211_local *local = &director.ieee80211_local;
	struct ieee80211_sub_if_data *sdata = NULL;

	sdata = local->sdata;
	if (sdata->vif->type != NL80211_IFTYPE_STATION){
		return;
	}

	sdata->u.mgd.driver_disconnect = true;
	sdata->u.mgd.reconnect = reconnect;
	//wiphy_work_queue(hw->wiphy, &sdata->u.mgd.beacon_connection_loss_work);
}

bool __bitmap_andnot(unsigned long *dst, unsigned long *bitmap1, unsigned long *bitmap2, unsigned int bits){
	unsigned int k = 0;
	unsigned int lim = bits/BITS_PER_LONG;
	unsigned long result = 0;

	for (k = 0; k < lim; k++){
		result |= (dst[k] = bitmap1[k] & ~bitmap2[k]);
	}
	if (bits % BITS_PER_LONG){
		result |= (dst[k] = bitmap1[k] & ~bitmap2[k] & BITMAP_LAST_WORD_MASK(bits));
	}

	return result != 0;
}


void pci_disable_msi(struct pci_dev *dev){

}

int cdev_add(struct cdev *p, u32 dev, unsigned count){
	int ret = 0;

	p->dev = dev;
	p->count = count;

	if (dev == WHITEOUT_DEV) {
		ret = -EBUSY;
		goto err;
	}

	kobject_get(p->kobj.parent);

	return ret;
err:
	kfree_const(p->kobj.name);
	p->kobj.name = NULL;
	return ret;
}

void *skb_pull(struct sk_buff *skb, unsigned int len){
	return skb_pull_inline(skb, len);
}

void *skb_push(struct sk_buff *skb, unsigned int len){
	skb->data -= len;
	skb->len  += len;

	if (unlikely(skb->data < skb->head)){
		skb_under_panic(skb, len, __builtin_return_address(0));
	}

	return skb->data;
}

void __skb_trim(struct sk_buff *skb, unsigned int len){
	__skb_set_length(skb, len);
}


void bitmap_cut(unsigned long *dst, unsigned long *src, unsigned int first, unsigned int cut, unsigned int nbits){
	unsigned int len = BITS_TO_LONGS(nbits);
	unsigned long keep = 0, carry;
	int i;

	if (first % BITS_PER_LONG) {
		keep = src[first / BITS_PER_LONG] & (~0UL >> (BITS_PER_LONG - first % BITS_PER_LONG));
	}

	memmove(dst, src, len * sizeof(*dst));

	while (cut--) {
		for (i = first / BITS_PER_LONG; i < len; i++) {
			if (i < len - 1){
				carry = dst[i + 1] & 1UL;
			}else{
				carry = 0;
			}
			dst[i] = (dst[i] >> 1) | (carry << (BITS_PER_LONG - 1));
		}
	}

	dst[first / BITS_PER_LONG] &= ~0UL << (first % BITS_PER_LONG);
	dst[first / BITS_PER_LONG] |= keep;
}

void release_firmware(const struct firmware *fw){
	if (fw != NULL) {
		free(fw);
	}
}

void skb_add_rx_frag_netmem(struct sk_buff *skb, int i, netmem_ref netmem, int off, int size, unsigned int truesize){
	skb_fill_netmem_desc(skb, i, netmem, off, size);
	skb->len += size;
	skb->data_len += size;
	skb->truesize += truesize;
}

void skb_headers_offset_update(struct sk_buff *skb, int off){
	/* Only adjust this if it actually is csum_start rather than csum */
	if (skb->ip_summed == CHECKSUM_PARTIAL){
		skb->csum_start += off;
	}
	/* {transport,network,mac}_header and tail are relative to skb->head */
	skb->transport_header += off;
	skb->network_header   += off;
	if (skb_mac_header_was_set(skb)){
		skb->mac_header += off;
	}
	skb->inner_transport_header += off;
	skb->inner_network_header += off;
	skb->inner_mac_header += off;
}

int ieee80211_emulate_add_chanctx(struct ieee80211_hw *hw, struct ieee80211_chanctx_conf *ctx){
	return 0;
}

void pci_unregister_driver(struct pci_driver *drv){

}

void ieee80211_cqm_rssi_notify(struct ieee80211_vif *vif, enum nl80211_cqm_rssi_threshold_event rssi_event, s32 rssi_level, int gfp){
	cfg80211_cqm_rssi_notify(NULL, rssi_event, rssi_level, gfp);
}

int pci_write_config_byte(const struct pci_dev *dev, int where, u8 val){
	return 0;
}

void __bitmap_replace(unsigned long *dst, unsigned long *old, const unsigned long *new, unsigned long *mask, unsigned int nbits){
	unsigned int k;
	unsigned int nr = BITS_TO_LONGS(nbits);

	for (k = 0; k < nr; k++){
		dst[k] = (old[k] & ~mask[k]) | (new[k] & mask[k]);
	}
}

unsigned long _find_last_bit(const unsigned long *addr, unsigned long size){
	if (size) {
		unsigned long val = BITMAP_LAST_WORD_MASK(size);
		unsigned long idx = (size-1) / BITS_PER_LONG;

		do {
			val &= addr[idx];
			if (val)
				return idx * BITS_PER_LONG + __fls(val);

			val = ~0ul;
		} while (idx--);
	}
	return size;
}

void skb_copy_header(struct sk_buff *new, const struct sk_buff *old){
	__copy_skb_header(new, old);

	skb_shinfo(new)->gso_size = skb_shinfo(old)->gso_size;
	skb_shinfo(new)->gso_segs = skb_shinfo(old)->gso_segs;
	skb_shinfo(new)->gso_type = skb_shinfo(old)->gso_type;
}

void _raw_spin_unlock_irqrestore(void *spin_lock, unsigned long flags){

}

struct sk_buff *skb_copy(const struct sk_buff *skb, int gfp_mask){
	unsigned int tail_off = 0;
	unsigned int headroom = 0;
	unsigned int data_capacity = 0;
	struct sk_buff *copied_skb = NULL;

	if (skb == NULL){
		return NULL;
	}

	if (!skb_frags_readable(skb)){
		return NULL;
	}

	if (skb->data_len != 0 || skb_is_nonlinear(skb)){
		return NULL;
	}

	headroom      = skb_headroom(skb);
	data_capacity = skb_end_offset(skb);
	tail_off      = skb_tail_offset(skb);

	/* always ignore the last argument of '__alloc_skb' */
	copied_skb = __alloc_skb(data_capacity, gfp_mask, skb_alloc_rx_flag(skb), 0);
	if (copied_skb == NULL){
		return NULL;
	}
	memcpy(copied_skb->head, skb->head, tail_off);
	copied_skb->data = copied_skb->head + headroom;
	skb_set_tail_offset(copied_skb, tail_off);
	copied_skb->len      = skb->len;
	copied_skb->data_len = 0;
	skb_copy_header(copied_skb, skb);
	copied_skb->cloned = 0;
	copied_skb->nohdr = 0;
	copied_skb->data = copied_skb->head + headroom;
	skb_set_tail_offset(copied_skb, tail_off);
	skb_set_end_offset(copied_skb, data_capacity);
	atomic_store(&skb_shinfo(copied_skb)->dataref, 1);

	return copied_skb;
}

void ieee80211_scan_completed(struct ieee80211_hw *hw, struct cfg80211_scan_info *info){
	set_bit(SCAN_COMPLETED, &director.ieee80211_local->scanning);
	if (info->aborted){
		set_bit(SCAN_ABORTED, &director.ieee80211_local->scanning);
	}

	memcpy(&director.ieee80211_local->scan_info, info, sizeof(*info));

//	wiphy_delayed_work_queue(local->hw.wiphy, &local->scan_work, 0);
}

void skb_queue_tail(struct sk_buff_head *list, struct sk_buff *newsk){
	__skb_queue_tail(list, newsk);
}

void __hw_addr_init(struct netdev_hw_addr_list *list){
	INIT_LIST_HEAD(&list->list);
	list->count = 0;
}

bool kthread_should_stop(void){
	bool ret = false;
	struct task_struct *t = NULL;

	t = current;
	if (t == NULL){
		return ret;
	}
	ret = atomic_load(&t->should_stop);
	return ret;
}

void usb_anchor_urb(struct urb *urb, struct usb_anchor *anchor){
	usb_get_urb(urb);
	list_add_tail(&urb->anchor_list, &anchor->urb_list);
	*(struct usb_anchor **)urb->anchor = anchor;
}

void dev_coredumpv(void *dev, void *data, size_t datalen, int gfp){

}

void skb_unlink(struct sk_buff *skb, struct sk_buff_head *list){
	__skb_unlink(skb, list);
}

void udp6_set_csum(bool nocheck, struct sk_buff *skb, struct in6_addr *saddr, struct in6_addr *daddr, int len){
	struct udphdr *uh = udp_hdr(skb);

	if (nocheck){
		uh->check = 0;
	}else if (skb_is_gso(skb)){
		uh->check = ~udp_v6_check(len, saddr, daddr, 0);
	}else if (skb->ip_summed == CHECKSUM_PARTIAL) {
		uh->check = 0;
		uh->check = udp_v6_check(len, saddr, daddr, lco_csum(skb));
		if (uh->check == 0){
			uh->check = CSUM_MANGLED_0;
		}
	}else{
		skb->ip_summed = CHECKSUM_PARTIAL;
		skb->csum_start = skb_transport_header(skb) - skb->head;
		skb->csum_offset = offsetof(struct udphdr, check);
		uh->check = ~udp_v6_check(len, saddr, daddr, 0);
	}
}

int usb_control_msg(struct usb_device *dev, unsigned int pipe, __u8 request, __u8 requesttype, __u16 value, __u16 index, void *data, __u16 size, int timeout){
	int ret = 0;
        ret = libusb_control_transfer(
                &director.usb_dev,
                requesttype,
                request,
                value,
                index,
                data,
                size,
                timeout
        );
	return ret;
}

void __x86_indirect_thunk_r15(void){

}

void tasklet_init(struct tasklet_struct *t, void (*func)(unsigned long), unsigned long data){
	t->next = NULL;
	t->state = 0;
	__atomic_store_n(&t->count, false, __ATOMIC_RELEASE);
	t->func = func;
	t->use_callback = false;
	t->data = data;
}

int __flush_workqueue(struct workqueue_struct *wq){
	int ret = 0;

	if (!wq == NULL){
		return ret;
	}

	if (wq_in_worker(wq) == true) {
		return ret;
	}

	pthread_mutex_lock(&wq->lock);
	for (;;) {
		delayed_work_timer_fn(wq);

		if (list_empty(&wq->work_list) && list_empty(&wq->delayed_list) && wq->nr_active == 0){
			break;
		}

		pthread_cond_broadcast(&wq->cond);

		if (!list_empty(&wq->work_list) || wq->nr_active != 0) {
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

	ret = true;
	return ret;
}

int pci_read_config_word(const struct pci_dev *dev, int where, u16 *val){
	return 0;
}

void __dynamic_pr_debug(void *descriptor, const char *fmt, ...){
	va_list ap;

	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}

void dev_kfree_skb_irq_reason(struct sk_buff *skb, int reason){
	if (unlikely(!skb)){
		return;
	}

	if (likely(refcount_read(&skb->users) == 1)) {
		refcount_set(skb->users, 0);
	} else if (likely(!refcount_dec_and_test(skb->users))) {
		return;
	}
	get_kfree_skb_cb(skb)->reason = reason;
}

const struct element *cfg80211_find_vendor_elem(unsigned int oui, int oui_type, const u8 *ies, unsigned int len) {
	const struct element *elem = NULL;
	u8 match[] = { oui >> 16, oui >> 8, oui, oui_type };
	int match_len = (oui_type < 0) ? 3 : sizeof(match);

	if (oui_type > 0xff){
		return NULL;
	}

	elem = cfg80211_find_elem_match(WLAN_EID_VENDOR_SPECIFIC, ies, len, match, match_len, 0);

	if (!elem || elem->datalen < 4){
		return NULL;
	}
	return elem;
}

void __raw_spin_lock_irq(void *lock){

}

u64 get_random_u64(void){
	struct pcg32_random {
                u64 state;
                u64 inc;
        }prng_seed = {
                .state = 0,
                .inc = 0
        };

        return (u64)pcg32_random_r(&prng_seed);
}

void pci_clear_master(struct pci_dev *dev){

}

int seq_write(struct seq_file *seq, const void *data, size_t len){
	if (seq->count + len < seq->size) {
		memcpy(seq->buf + seq->count, data, len);
		seq->count += len;
		return 0;
	}
	seq->count = seq->size;
	return -1;
}

int flush_work(struct work_struct *work){
	long ret = 0;
	struct workqueue_struct *wq = NULL;

	if(work == NULL){
		return ret;
	}

	ret = atomic_load_explicit(&work->data, memory_order_acquire);
	if(ret == 0){
		return ret;
	}

	wq = (struct workqueue_struct *)(uintptr_t)(ret & ~(long)WORK_STRUCT_FLAG_MASK);

	if(wq == NULL){
		return ret;
	}

	ret = wq_in_worker(wq);

	if(ret){
		errno = EDEADLK;
		return ret;
	}

	pthread_mutex_lock(&wq->lock);

	while(atomic_load_explicit(&work->data, memory_order_acquire)){
		pthread_cond_wait(&wq->cond, &wq->lock);
	}

	pthread_mutex_unlock(&wq->lock);
	return ret;
}

void synchronize_rcu(void){

}

unsigned int ieee80211_get_hdrlen_from_skb(const struct sk_buff *skb){
	unsigned int hdrlen = 0;
	const struct ieee80211_hdr *hdr = NULL;

	hdr = (const struct ieee80211_hdr *)skb->data;

	if (unlikely(skb->len < 10)){
		return 0;
	}
	hdrlen = ieee80211_hdrlen(hdr->frame_control);
	if (unlikely(hdrlen > skb->len)){
		return 0;
	}
	return hdrlen;
}

int acpi_evaluate_object(void *obj, ...){
	return 0;
}

void seq_printf(struct seq_file *m, const char *f, ...){
	va_list args;

	va_start(args, f);
	seq_vprintf(m, f, args);
	va_end(args);
}

unsigned long _copy_from_user(void *to, const void __user *from, unsigned long n){
	return _inline_copy_from_user(to, from, n);
}

void __init_waitqueue_head(struct wait_queue_head *wq_head, const char *name, void *key){
	spin_lock_init(&wq_head->lock);
	INIT_LIST_HEAD(&wq_head->head);
}

void __ubsan_handle_shift_out_of_bounds(void *_data, void *lhs, void *rhs){

}

void *pci_ioremap_bar(struct pci_dev *pdev, int bar){
	return NULL;
}

void cfg80211_gtk_rekey_notify(struct net_device *dev, const u8 *bssid, const u8 *replay_ctr, int gfp){
	nl80211_gtk_rekey_notify(dev, dev, bssid, replay_ctr, gfp);
}

void unregister_chrdev_region(u32 from, unsigned count){
	u32 to = from + count;
}

void *skb_pull_data(struct sk_buff *skb, size_t len){
	void *data = NULL;
	data = skb->data;

	if (skb->len < len){
		return NULL;
	}

	skb_pull(skb, len);
	return data;
}

unsigned int __sw_hweight8(unsigned int w){
	unsigned int res = w - ((w >> 1) & 0x55);
	res = (res & 0x33) + ((res >> 2) & 0x33);
	return (res + (res >> 4)) & 0x0F;
}

void _raw_spin_lock_irqsave(void *spin_lock){

}

void _raw_spin_unlock_irq(void *lock){

}

void ieee80211_emulate_change_chanctx(struct ieee80211_hw *hw, struct ieee80211_chanctx_conf *ctx, u32 changed){

}

void __x86_indirect_thunk_r12(void){

}

void napi_disable(struct napi_struct *n){

}

struct usb_interface *usb_ifnum_to_if(const struct usb_device *udev, unsigned ifnum){
	struct usb_host_config *config = NULL; //udev->actconfig;
	int i = 0;

	if (!config){
		return NULL;
	}

	for (i = 0; i < config->desc.bNumInterfaces; i++){
		if (config->interface[i]->altsetting[0].desc.bInterfaceNumber == ifnum){
			return config->interface[i];
		}
	}
	return NULL;
}

const void *free_irq(unsigned int irq, void *dev_id){

}

void skb_queue_purge_reason(struct sk_buff_head *list, int reason){
	struct sk_buff_head tmp = {};

	if (skb_queue_empty_lockless(list)){
		return;
	}

	__skb_queue_head_init(&tmp);
	skb_queue_splice_init(list, &tmp);
	__skb_queue_purge_reason(&tmp, reason);
}

int queue_work_on(int cpu, struct workqueue_struct *wq, struct work_struct *work){
	int ret = 0;
	long expected = 0;
	long desired = 0;

	if (wq == NULL || work == NULL){
		return 0;
	}

	desired = work_encode(wq, WORK_STRUCT_PENDING);

	ret = atomic_compare_exchange_strong_explicit(&work->data, &expected, desired, memory_order_acq_rel, memory_order_relaxed);
	if(ret == 0){
		return ret;
	}

	pthread_mutex_lock(&wq->lock);
	if (wq->stop == true) {
		pthread_mutex_unlock(&wq->lock);
		atomic_store_explicit(&work->data, 0, memory_order_release);
		return ret;
	}

	list_add_tail(&work->entry, &wq->work_list);
	pthread_cond_signal(&wq->cond);
	pthread_mutex_unlock(&wq->lock);

	ret = true;
	return ret;
}

int __warn_printk(const char *fmt, ...){
	int ret = 0;
	va_list ap;
	va_start(ap, fmt);
	ret = vprintf(fmt, ap);
	va_end(ap);
	return ret;
}

void _raw_spin_unlock(void *spin_lock){

}

unsigned long _find_next_or_bit(const unsigned long *addr1, const unsigned long *addr2, unsigned long nbits, unsigned long start){
	return FIND_NEXT_BIT(addr1[idx] | addr2[idx], /* nop */, nbits, start);
}

void *devm_kmalloc(void *dev, size_t size, int gfp){
	void *p = NULL;
	p = malloc(size);

	if(p != NULL){
		for(u8 j = 0; j < MAX_KMEM_ARRAY_SIZE; j++){
			if(director.kmem_array[j] != NULL){
				director.kmem_array[j] = p;
			}
		}
	}
	return p;
}

u32 get_random_u32(void){
	struct pcg32_random {
                u64 state;
                u64 inc;
        }prng_seed = {
		.state = 0,
		.inc = 0
	};

	return pcg32_random_r(&prng_seed);
}

struct device *get_device(struct device *dev){
	return dev; // ? kobj_to_dev(kobject_get(&dev->kobj)) : NULL;
}

struct sk_buff *skb_clone(struct sk_buff *skb, int gfp_mask){
	struct sk_buff *n = NULL;

	if (skb_orphan_frags(skb, gfp_mask)){
		return NULL;
	}

	if (skb->fclone == SKB_FCLONE_ORIG) {
		n = __kmalloc_large_noprof(SK_BUFF_STRUCT_SIZE, gfp_mask);
		if (n == NULL){
			return NULL;
		}
		n->fclone = SKB_FCLONE_CLONE;
	} else {
		n = __kmalloc_large_noprof(SK_BUFF_STRUCT_SIZE, gfp_mask);
		if (n == NULL){
			return NULL;
		}
		n->fclone = SKB_FCLONE_UNAVAILABLE;
	}

	return __skb_clone(n, skb);
}

void bpf_trace_run3(void *link, u64 arg1, u64 arg2, u64 arg3){

}

void *check_bytes8(const u8 *start, u8 value, unsigned int bytes){
	while (bytes) {
		if (*start != value)
			return (void *)start;
		start++;
		bytes--;
	}
	return NULL;
}

void __SCT__might_resched(void){

}

void ieee80211_emulate_remove_chanctx(struct ieee80211_hw *hw, struct ieee80211_chanctx_conf *ctx){

}

void __fortify_panic(const u8 reason, const size_t avail, const size_t size){

}

struct net_device *alloc_netdev_mqs(int sizeof_priv, char *name, unsigned char name_assign_type, void (*setup)(struct net_device *), unsigned int txqs, unsigned int rxqs){
	struct net_device *netdev = NULL;
	size_t napi_config_sz = 0;
	unsigned int maxqs = 0;

	if (txqs < 1) {
		pr_err("alloc_netdev: Unable to allocate device with zero queues\n");
		return NULL;
	}

	if (rxqs < 1) {
		pr_err("alloc_netdev: Unable to allocate device with zero RX queues\n");
		return NULL;
	}

	maxqs = max(txqs, rxqs);

	netdev = malloc(NET_DEVICE_STRUCT_SIZE);
	if (netdev == NULL){
		return NULL;
	}

	netdev->priv_len = sizeof_priv;

	refcount_set(&netdev->dev_refcnt, 1);

	if (dev_addr_init(netdev)){
		goto free_all;
	}

	dev_mc_init(netdev);
	dev_uc_init(netdev);

	INIT_LIST_HEAD(&netdev->unreg_list);
	INIT_LIST_HEAD(&netdev->close_list);
	INIT_LIST_HEAD(&netdev->ptype_all);
	INIT_LIST_HEAD(&netdev->ptype_specific);
	INIT_LIST_HEAD(&netdev->net_notifier_list);

	netdev->gso_max_size = GSO_MAX_SIZE;
	netdev->gso_max_segs = GSO_MAX_SEGS;

	netdev->priv_flags = IFF_XMIT_DST_RELEASE | IFF_XMIT_DST_RELEASE_PERM;
	setup(netdev);

	#if 0
	if (netdev->tx_queue_len == 0) {
		netdev->priv_flags |= IFF_NO_QUEUE;
		netdev->tx_queue_len = DEFAULT_TX_QUEUE_LEN;
	}

	netdev->num_tx_queues = txqs;
	netdev->real_num_tx_queues = txqs;
	if (netif_alloc_netdev_queues(netdev)){
		goto free_all;
	}

	netdev->num_rx_queues = rxqs;
	netdev->real_num_rx_queues = rxqs;

	if (netif_alloc_rx_queues(netdev)){
		goto free_all;
	}

	netdev->ethtool = malloc(sizeof(*dev->ethtool));
	if (netdev->ethtool == NULL){
		goto free_all;
	}

	netdev->cfg = malloc(sizeof(*dev->cfg));
	if (netdev->cfg == NULL){
		goto free_all;
	}
	netdev->cfg_pending = netdev->cfg;

	netdev->num_napi_configs = maxqs;
	#endif

	strncpy(netdev->name, name, IFNAMSIZ);
	netdev->name_assign_type = name_assign_type;
	netdev->group = INIT_NETDEV_GROUP;

	return netdev;

free_all:
	free(netdev);
	return NULL;
}

void __x86_indirect_thunk_r13(void){

}

int regulatory_hint(struct wiphy *wiphy, const char *alpha2){
	struct regulatory_request *request = NULL;

	if (!alpha2 || !wiphy){
		return -EINVAL;
	}

	wiphy->regulatory_flags &= ~REGULATORY_CUSTOM_REG;

	request = __kmalloc_large_noprof(sizeof(struct regulatory_request), GFP_KERNEL);
	if (!request){
		return -ENOMEM;
	}
	//request->wiphy_idx = get_wiphy_idx(wiphy);

	request->alpha2[0] = alpha2[0];
	request->alpha2[1] = alpha2[1];
	request->initiator = NL80211_REGDOM_SET_BY_DRIVER;

	/* Allow calling CRDA again */
	director.ieee80211_local->reg_crda_timeouts = 0;

	/* inline here '' */
	request->alpha2[0] = toupper(request->alpha2[0]);
	request->alpha2[1] = toupper(request->alpha2[1]);

	pthread_mutex_lock(&director.ieee80211_local->regulatory_req_mutex);
	list_add_tail(&request->list, &director.ieee80211_local->reg_requests_list);
	pthread_mutex_unlock(&director.ieee80211_local->regulatory_req_mutex);

	return 0;
}

struct sk_buff *__build_skb(void *data, unsigned int frag_size){
	struct sk_buff *skb = NULL;

	skb = (struct sk_buff *)malloc(SK_BUFF_STRUCT_SIZE);
	if (unlikely(!skb)){
		return NULL;
	}
	memset(skb, 0, SK_BUFF_STRUCT_SIZE);
	__build_skb_around(skb, data, frag_size);

	return skb;
}


void usb_autopm_put_interface_async(struct usb_interface *intf){

}

int autoremove_wake_function(struct wait_queue_entry *wq_entry, unsigned mode, int sync, void *key){
	int ret = true;
	struct task_struct *task_struct = NULL;

	if (wq_entry_is_linked(wq_entry) == true){
		list_del_init(&wq_entry->entry);
	}

	task_struct = (struct task_struct *)wq_entry->private;

	if (task_struct != NULL){
		wake_up_process(task_struct);
	}

	return ret;
}

void __dma_sync_single_for_device(struct device *dev, long addr, size_t size, int dir){

}

long prepare_to_wait_event(struct wait_queue_head *wq_head, struct wait_queue_entry *wq_entry, int state){
	int ret = 0;
	struct task_struct *task_struct = NULL;

	task_struct = current;
	if (task_struct == NULL){
		ret = -ENOMEM;
		return ret;
	}

	if ((state & TASK_INTERRUPTIBLE) && atomic_load(&task_struct->should_stop)){
		ret = -ERESTART;
		return ret;
	}

	spin_lock(&wq_head->lock);

	if (!wq_entry_is_linked(wq_entry)){
		list_add_tail(&wq_entry->entry, &wq_head->head);
	}

	atomic_store(&task_struct->__state, state);

	spin_unlock(&wq_head->lock);
	return ret;
}

int wake_up_process(struct task_struct *task_struct){
	int ret = 0;
	bool process_woke_up = false;

	if (task_struct == NULL){
		ret = -EINVAL;
		return ret;
	}

	pthread_mutex_lock(&task_struct->lock);

	if (atomic_load(&task_struct->started) == false) {
		atomic_store(&task_struct->started, true);
		process_woke_up = true;
	}

	if (atomic_load(&task_struct->__state) != TASK_RUNNING) {
		atomic_store(&task_struct->__state, TASK_RUNNING);
		process_woke_up = true;
	}

	pthread_cond_broadcast(&task_struct->cond);
	pthread_mutex_unlock(&task_struct->lock);
	return process_woke_up;
}

struct sk_buff *__pskb_copy_fclone(struct sk_buff *skb, int headroom, int gfp_mask, bool fclone){
	int i = 0;
	int flags = 0;
	unsigned int size = 0;
	struct sk_buff *n = NULL;

	size = skb_headlen(skb) + headroom;
	flags = skb_alloc_rx_flag(skb) | (fclone ? SKB_ALLOC_FCLONE : 0);
	n = __alloc_skb(size, gfp_mask, flags, 0);

	if (n == NULL){
		goto out;
	}

	/* Set the data pointer */
	skb_reserve(n, headroom);
	/* Set the tail pointer and length */
	skb_put(n, skb_headlen(skb));
	/* Copy the bytes */
	skb_copy_from_linear_data(skb, n->data, n->len);

	n->truesize += skb->data_len;
	n->data_len  = skb->data_len;
	n->len	     = skb->len;

	if (skb_shinfo(skb)->nr_frags) {
		if (skb_orphan_frags(skb, gfp_mask) || skb_zerocopy_clone(n, skb, gfp_mask)) {
			kfree_skb(n);
			n = NULL;
			goto out;
		}
		for (i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
			skb_shinfo(n)->frags[i] = skb_shinfo(skb)->frags[i];
			skb_frag_ref(skb, i);
		}
		skb_shinfo(n)->nr_frags = i;
	}

	if (skb_has_frag_list(skb)) {
		skb_shinfo(n)->frag_list = skb_shinfo(skb)->frag_list;
		skb_clone_fraglist(n);
	}

	skb_copy_header(n, skb);
out:
	return n;
}

int pci_enable_device(struct pci_dev *dev){
	return 0;
}

