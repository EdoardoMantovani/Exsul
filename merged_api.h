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

#include "utils.h"

/* defines the maximun number of usb devices which can be present in the current kernel module */
#define MAX_USB_DEV_LIST	36

/* array size of the module memory allocated by the functions 'kmem_xxx' */
#define MAX_KMEM_ARRAY_SIZE	36

/* define the number of maximun lkm which can be loaded in memory for one istance of this application */
#define MAX_ALLOCATED_LKM_NUMBER 12

#define NR_KMALLOC_TYPES	8

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

struct device;
struct pci_dev;
struct net_device;
struct pci_driver;
struct work_struct;

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

struct sk_buff_head {
	/* These two members must be first. */
	struct sk_buff	*next;
	struct sk_buff	*prev;

	__u32		qlen;
};

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

struct module_controller;
extern struct module_controller director;

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

static struct task_struct *tls_current;

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

#include <module.h>
#include <skb.h>
#include <ieee80211.h>
#include <prototypes.h>
#include <mod_controller.h>

#endif

#ifndef __UTILS_H__
#define __UTILS_H__

#define MODULE_INPUT_MIN_LEN 4

#define UL(x) (_UL(x))
#define ULL(x) (_ULL(x))
#define BIT(nr) (UL(1) << (nr))
#define BIT_ULL(nr) (ULL(1) << (nr))

#define BIT_MASK(nr) (UL(1) << ((nr) % BITS_PER_LONG))
#define BIT_WORD(nr) ((nr) / BITS_PER_LONG)
#define BIT_ULL_MASK(nr) (ULL(1) << ((nr) % BITS_PER_LONG_LONG))
#define BIT_ULL_WORD(nr) ((nr) / BITS_PER_LONG_LONG)
#define BITS_PER_BYTE 8
#define BITS_PER_TYPE(type) (sizeof(type) * BITS_PER_BYTE)

#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))

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

/*
 *       even if it isn't "correct", let's simplify 'usleep_range' in this way
 */
#define usleep_range(min_sleep, max_sleep) usleep((min_sleep + max_sleep) / 2)

#define ms_to_us(ms) ((useconds_t)((ms) * 1000))
#define udelay(us) usleep(us)
#define prep_zero_mask(a, bits, c) (bits)
#define __fls(word) generic___fls(word)
#define find_zero(bits) (__ffs(bits) >> 3)

#define __packed __attribute__((packed))
#define __force __attribute__((force))
#define __aligned(x) __attribute__((aligned(x)))
#define fallthrough __attribute__((fallthrough))

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

#ifndef htonl
#if __BYTE_ORDER == __BIG_ENDIAN
#define htonl(x) x
#else
#define htonl(x) __builtin_bswap32(x)
#endif
#endif

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

#define __branch_check__(x, expect, is_constant)                               \
        ({                                                                     \
                long ______r;                                                  \
                ______r = __builtin_expect(!!(x), expect);                     \
                ______r;                                                       \
        })

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

#define REPEAT_BYTE(x)	((~0ul / 0xff) * (x))
#define WORD_AT_A_TIME_CONSTANTS { REPEAT_BYTE(0x01), REPEAT_BYTE(0x80) }

#define hex_asc_lo(x)	hex_asc[((x) & 0x0f)]
#define hex_asc_hi(x)	hex_asc[((x) & 0xf0) >> 4]

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

#define __rcu_dereference_check(p, local)	 					\
({ 											\
	typeof(*p) *local = (typeof(*p) *__force)READ_ONCE(p); 				\
	((typeof(*p) __force  *)(local)); 						\
})

#define ___PASTE(a,b) 			a##b
#define __PASTE(a,b) 			___PASTE(a,b)

#define __UNIQUE_ID(prefix) 		__PASTE(__PASTE(__UNIQUE_ID_, prefix), __COUNTER__)

#define rcu_dereference_check(p, c) 	__rcu_dereference_check((p), __UNIQUE_ID(rcu))

#define rcu_dereference(p)		rcu_dereference_check(p, 0)

#define __COUNT_ARGS(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _n, X...) _n

#define COUNT_ARGS(X...) 		__COUNT_ARGS(, ##X, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define CONCATENATE(a, b)		__CONCAT(a, b)

#define strscpy(dst, src, ...)		CONCATENATE(__strscpy, COUNT_ARGS(__VA_ARGS__))(dst, src, __VA_ARGS__)
#define __strscpy1(dst, src, size)	sized_strscpy(dst, src, size)

#define tolower(c) __tolower(c)
#define toupper(c) __toupper(c)

#define __ismask(x) (_ctype[(int)(unsigned char)(x)])

#define isalnum(c)	((__ismask(c)&(_U|_L|_D)) != 0)
#define isalpha(c)	((__ismask(c)&(_U|_L)) != 0)
#define iscntrl(c)	((__ismask(c)&(_C)) != 0)

#define isgraph(c)	((__ismask(c)&(_P|_U|_L|_D)) != 0)
#define islower(c)	((__ismask(c)&(_L)) != 0)
#define isprint(c)	((__ismask(c)&(_P|_U|_L|_D|_SP)) != 0)
#define ispunct(c)	((__ismask(c)&(_P)) != 0)
/* Note: isspace() must return false for %NUL-terminator */
#define isspace(c)	((__ismask(c)&(_S)) != 0)
#define isupper(c)	((__ismask(c)&(_U)) != 0)
#define isxdigit(c)	((__ismask(c)&(_D|_X)) != 0)

#define _U	0x01	/* upper */
#define _L	0x02	/* lower */
#define _D	0x04	/* digit */
#define _C	0x08	/* cntrl */
#define _P	0x10	/* punct */
#define _S	0x20	/* white space (space/lf/tab) */
#define _X	0x40	/* hex digit */
#define _SP	0x80	/* hard space (0x20) */

typedef struct refcount_struct {
	atomic_uint refs;
}refcount_t;

struct kref {
	refcount_t refcount;
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

#endif



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

int usb_register_driver(struct usb_driver *new_driver, struct module *owner, const char *mod_name){
	return 0;
}

int ieee80211_emulate_switch_vif_chanctx(struct ieee80211_hw *hw, struct ieee80211_vif_chanctx_switch *vifs, int n_vifs, enum ieee80211_chanctx_switch_mode mode){
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

void _dev_printk(const char *level, const void *dev, const char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}

void __x86_indirect_thunk_r10(void){

}

void __x86_return_thunk(void){

}

void pci_enable_msi(struct pci_dev *dev){

}

void _raw_spin_lock(void *spin_lock){

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


void vfree(void *addr){
	if(addr != NULL){
		free(addr);
	}
}

void __mutex_init(pthread_mutex_t *lock, const char *name, void *key){
	pthread_mutex_init(lock, NULL);
}

void __x86_indirect_thunk_r14(void){

}

unsigned long __usecs_to_jiffies(const unsigned int u){
	if (u > jiffies_to_usecs(MAX_JIFFY_OFFSET)){
		return MAX_JIFFY_OFFSET;
	}
	return _usecs_to_jiffies(u);
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

void sort(void *base, size_t num, size_t size, cmp_func_t cmp_func, swap_func_t swap_func){
	qsort(base, num, size, cmp_func);
}

void pci_disable_device(struct pci_dev *dev){

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

ssize_t kernel_read(void *file, void *buf, size_t count, int *pos){

}

int pcie_capability_read_word(struct pci_dev *dev, int pos, u16 *val){
	return 0;
}

void *__kmalloc_cache_noprof(void *s, int gfpflags, size_t size){
	return (void *)malloc(size);
}

void __x86_indirect_thunk_r8(void){

}

int pci_write_config_dword(const struct pci_dev *dev, int where, u32 val){
	return 0;
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
	if(skb != NULL){
		free(skb);
	}
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

void __local_bh_enable_ip(unsigned long ip, unsigned int cnt){

}

int64_t ktime_get(void){
	return (int64_t)nsec_now_monotonic();
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

	director.rdev.rfkill_ops.poll = rfkill_poll_cb;
	director.rfkill.polling_paused = false;
	if (director.rfkill.suspended == true){
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

unsigned long hweight_long(unsigned long w){
	return sizeof(w) == 4 ? __sw_hweight32(w) : __sw_hweight64((__u64)w);
}


int pci_read_config_dword(const struct pci_dev *dev, int where, u32 *val){
	return 0;
}

void __x86_indirect_thunk_r11(void){

}

void __x86_indirect_thunk_rbx(void){

}

void __warn_flushing_systemwide_wq(void){
	printk("WARNING: Flushing system-wide workqueues will be prohibited in near future.\n");
}

bool is_acpi_device_node(void *fwnode){
	return false;
}

unsigned long _find_next_zero_bit(const unsigned long *addr, unsigned long nbits, unsigned long start){
	return FIND_NEXT_BIT(~addr[idx], /* nop */, nbits, start);
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

int _printk(const char *fmt, ...){
	int ret = 0;
	va_list ap;

	if(fmt[0] == "\001" || (fmt[1] >= '0' && fmt[1] <= '7') ){
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
	if (director.rdev.rfkill_ops.poll == NULL){
		return;
	}

	director.rfkill.polling_paused = true;
	//cancel_delayed_work_sync(&rfkill->poll_work);
}

void __x86_indirect_thunk_r9(void){

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

unsigned long __find_nth_and_bit(const unsigned long *addr1, const unsigned long *addr2, unsigned long size, unsigned long n){
	return FIND_NTH_BIT(addr1[idx] & addr2[idx], size, n);
}

void refcount_warn_saturate(void *r, int t){

}

void __ubsan_handle_out_of_bounds(void *_data, void *index){

}

void _raw_spin_lock_bh(void *spin_lock){

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

void napi_enable(struct napi_struct *n){

}

unsigned long _find_next_bit(const unsigned long *addr, unsigned long nbits, unsigned long start){
	return FIND_NEXT_BIT(addr[idx], /* nop */, nbits, start);
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

void _dev_err(const struct device *dev, const char *fmt, ...){
	fmt += 2;
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}

int pci_request_regions(struct pci_dev *pdev, const char *name){
	return 0;
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

void usb_enable_autosuspend(struct usb_device *udev){

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

void __check_object_size(const void *ptr, unsigned long n, bool to_user){

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

int printk(const char *fmt, ...){
	int ret = 0;
	if(fmt[0] == "\001"){
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

void mutex_unlock(pthread_mutex_t *lock){
	pthread_mutex_unlock(lock);
}

const void *dmi_first_match(const void *list){
	return NULL;
}

void mutex_lock(pthread_mutex_t *lock){
	pthread_mutex_lock(lock);
}

struct task_struct *kthread_create_on_node(int (*kthread_function)(void *data), void *data, int node, const char namefmt[], ...){
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

	/* unsure if mantain the variable arg list or just mantain as the name a single string (which is very common in lkms) */
	va_list ap;
	va_start(ap, namefmt);
	vsnprintf(current_thread->name, sizeof(current_thread->name), namefmt, ap);
	va_end(ap);

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

int scnprintf(char *buf, size_t size, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vscnprintf(buf, size, fmt, args);
	va_end(args);

	return i;
}

void __dynamic_dev_dbg(struct _ddebug *descriptor, const struct device *dev, const char *fmt, ...){
	va_list ap;

	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}

int futex_wake(atomic_int *addr, int flags, int nr_wake, u32 bitset){
	return (int)syscall(SYS_futex, (int *)addr, FUTEX_WAKE, flags, NULL, NULL, 0);
}

void __ubsan_handle_load_invalid_value(void *_data, void *val){

}

void pci_release_regions(struct pci_dev *pdev){

}

void __x86_indirect_thunk_rsi(void){

}

int acpi_get_handle(void *parent, const char *pathname, void *ret_handle){
	return 0;
}


void _raw_spin_lock_irq(void *lock){

}

int hex_to_bin(char ch){
	if ((ch >= '0') && (ch <= '9'))
		return ch - '0';
	ch = tolower(ch);
	if ((ch >= 'a') && (ch <= 'f'))
		return ch - 'a' + 10;
	return -1;
}

void pci_set_master(struct pci_dev *dev){

}

unsigned long __find_nth_bit(const unsigned long *addr, unsigned long size, unsigned long n){
	return FIND_NTH_BIT(addr[idx], size, n);
}

void pci_disable_msi(struct pci_dev *dev){

}

void release_firmware(const struct firmware *fw){
	if (fw != NULL) {
		free(fw);
	}
}

int ieee80211_emulate_add_chanctx(struct ieee80211_hw *hw, struct ieee80211_chanctx_conf *ctx){
	return 0;
}

void pci_unregister_driver(struct pci_driver *drv){

}

int pci_write_config_byte(const struct pci_dev *dev, int where, u8 val){
	return 0;
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

void _raw_spin_unlock_irqrestore(void *spin_lock, unsigned long flags){

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
	urb->anchor = anchor;
}

void dev_coredumpv(void *dev, void *data, size_t datalen, int gfp){

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

void __dynamic_pr_debug(struct _ddebug *descriptor, const char *fmt, ...){
	va_list ap;

	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
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

int acpi_evaluate_object(void *obj, ...){
	return 0;
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

struct usb_interface *usb_ifnum_to_if(const struct usb_device *dev, unsigned ifnum){
	struct usb_host_config *config = NULL; //dev->actconfig;
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

void __x86_indirect_thunk_r13(void){

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

int pci_enable_device(struct pci_dev *dev){
	return 0;
}

