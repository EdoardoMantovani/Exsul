#ifndef __LAYER_H__
#define __LAYER_H__

#ifdef __linux__
#define _GNU_SOURCE
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
#include <link.h>
#include <fcntl.h>
#include <poll.h>
#include <execinfo.h>
#include <libusb-1.0/libusb.h>
#include <linux/elf-em.h>
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
#include <sys/resource.h>
#include <unistd.h>
#include <stdatomic.h>
#include <netinet/ether.h>
#include <dwarf.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <libdwarf/libdwarf.h>
#ifdef ENABLE_UNWIND_DEBUGGING
#include <libunwind.h>
#endif
#else
#include <windows.h>
#include <synchapi.h>
#include <stddef.h>
#include <bcrypt.h>
#include <direct.h>
#include <elf.h>
#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <signal.h>
#include <process.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <sys/mman.h>
#include <linux/const.h>
#include <linux/types.h>
#include <linux/stddef.h>
#include <linux/in6.h>
#include <linux/udp.h>
#include <linux/elf-em.h>
#include <linux/if_ether.h>
#include <linux/nl80211.h>
#include <linux/ethtool.h>
#include <linux/usb/ch9.h>
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <asm-generic/param.h>
#include <dwarf.h>
#include <libdwarf/libdwarf.h>

#include <libusb-1.0/libusb.h>

typedef off_t	loff_t;

#ifndef __cplusplus
#define __struct_group_tag(TAG) TAG
#else
#define __struct_group_tag(TAG)
#endif

#define __struct_group(TAG, NAME, ATTRS, MEMBERS...)                           \
        union {                                                                \
                struct {                                                       \
                        MEMBERS                                                \
                } ATTRS;                                                       \
                struct __struct_group_tag(TAG){MEMBERS} ATTRS NAME;            \
        } ATTRS

/* overwrite the 'pthread_xxx' types with windows based definitions */
typedef int		   pthread_attr_t;
typedef SRWLOCK            pthread_mutex_t;
typedef CONDITION_VARIABLE pthread_cond_t;
typedef HANDLE             pthread_t;
/* 'pthread_condattr_t' hasn't a direct translation, is just an attribute to 'pthread_cond_t' */
typedef int		   pthread_condattr_t;
typedef volatile LONG	   pthread_once_t;
#define PTHREAD_ONCE_INIT  0

typedef void (*pthread_once_fn)(void);

/* for windows also '_UL' and '_ULL' needs to be defined... */
#ifdef __ASSEMBLY__
#define _AC(X,Y)        X
#define _AT(T,X)        X
#else
#define __AC(X,Y)       (X##Y)
#define _AC(X,Y)        __AC(X,Y)
#define _AT(T,X)        ((T)(X))
#endif

#define _UL(x)          (_AC(x, UL))
#define _ULL(x)		(_AC(x, ULL))

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

#define __always_inline inline
#ifndef NL80211_SCAN_FLAG_RANDOM_ADDR
#define NL80211_SCAN_FLAG_RANDOM_ADDR 1<<3
#endif

#ifdef __TINYC__
#define __ATOMIC_ACQUIRE 2
#define __ATOMIC_RELEASE 3
#define _Atomic(T)                                                             \
        struct {                                                               \
                T volatile __val;                                              \
        }
typedef _Atomic(unsigned int) atomic_uint;
#endif

#endif /* __linux__ */

#if __BYTE_ORDER == __LITTLE_ENDIAN
#include <linux/byteorder/little_endian.h>
#else
#include <linux/byteorder/big_endian.h>
#endif

#if UINTPTR_MAX == 0xffffffff
        #define Elf(field)      Elf32_##field
        #define BITS_PER_LONG   32
        #define RAND_BASE_ADDR  uintptr_t
#elif UINTPTR_MAX == 0xffffffffffffffff
        #define Elf(field)      Elf64_##field
        #define BITS_PER_LONG   64
        #define RAND_BASE_ADDR  uintptr_t
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

#ifdef TCC_SCRIPTING
#include <libtcc.h>
#endif

/* defines the maximun size of the buffer data which can be transferred from the user to the kernel stub */
#define MAX_DEBUGFS_SIZE		128

/* defines the maximun number of usb devices which can be present in the current kernel module */
#define MAX_USB_DEV_LIST		36

/* diagnostic scan cache used by the userspace mac80211 translation */
#define EXSUL_MAX_SCAN_RESULTS		128
#define EXSUL_MAX_SCAN_SSID_LEN		32

/* array size of the module memory allocated by the functions 'kmem_xxx' */
#define MAX_KMEM_ARRAY_SIZE		36

/* define the number of maximun lkm which can be loaded in memory for one istance of this application */
#define MAX_ALLOCATED_LKM_NUMBER	12

/* size for the sk_buff struct, usually is around 384 bytes */
#define SK_BUFF_STRUCT_SIZE		512

/* size for the 'net_device' struct, usually around 2600 bytes */
#define NET_DEVICE_STRUCT_SIZE		3 * 1024

/* maximun number of files that can be opened by the kernel module */
#define MAX_FILE_IDX			32

/* maximun size of the backtrace, called only in case of crash */
#define BACKTRACE_MAX_DEPTH     	24

/* maximun number of usb endpoints possible */
#define USB_MAX_ENDPOINT_NUM    	30

/* minimun area size for tcc jit */
#define TCC_MINIMUN_AREA_SIZE		512

/* path for the firmware files, this will be used mainly in Linux, for Windows expect a local folder */
#define DEFAULT_FW_PATH			"/lib/firmware/"

/* magic number used for identifying the header of crash dump */
#define DUMP_MAGIC			"DCCCCB"

/* name of the crashdump file */
#define DUMP_FILENAME			"crashreport-%d-%02d-%02d_%02d:%02d:%02d.bin"

/* socket name for the ipc socket used by the standalone ELF */
#define SOCKET_PATH			"/tmp/exsul_socket"

/* size of the socket buffer sent from a client to the ipc server initialized by the standalone ELF */
#define BUFF_SIZE			1024 * 32 * 4

/* maximun socket connections that can be accepted by the standalone ELF */
#define MAXIMUN_SOCKET_CONNECTIONS	2

/* include here the errno.h header data */
#define ERESTARTSYS             512
#define ERESTARTNOINTR          513
#define ERESTARTNOHAND          514     /* restart if no handler.. */
#define ENOIOCTLCMD             515     /* No ioctl command */
#define ERESTART_RESTARTBLOCK   516 /* restart by calling sys_restart_syscall */
#define EPROBE_DEFER            517     /* Driver requests probe retry */
#define EOPENSTALE              518     /* open found a stale dentry */
#define ENOPARAM                519     /* Parameter not supported */

/* Defined for the NFSv3 protocol */
#define EBADHANDLE              521     /* Illegal NFS file handle */
#define ENOTSYNC                522     /* Update synchronization mismatch */
#define EBADCOOKIE              523     /* Cookie is stale */
#define ENOTSUPP                524     /* Operation is not supported */
#define ETOOSMALL               525     /* Buffer or request is too small */
#define ESERVERFAULT            526     /* An untranslatable error occurred */
#define EBADTYPE                527     /* Type not supported by server */
#define EJUKEBOX                528     /* Request initiated, but will not complete before timeout */
#define EIOCBQUEUED             529     /* iocb queued, will get completion event */
#define ERECALLCONFLICT         530     /* conflict with recalled state */
#define ENOGRACE                531     /* NFS file lock reclaim refused */

#define NR_KMALLOC_TYPES	8
#define	NUMA_NO_NODE		(-1)

#define MAX_ERRNO		4095

#define GFP_KERNEL		0
#define GFP_ATOMIC		0
#define __GFP_NOWARN		0
#define __GFP_ZERO		0

#define POISON_POINTER_DELTA	0
#define LIST_POISON1		((void *) 0x100 + POISON_POINTER_DELTA)
#define LIST_POISON2		((void *) 0x122 + POISON_POINTER_DELTA)

#define L1_CACHE_SHIFT		5
#define L1_CACHE_BYTES		(1 << L1_CACHE_SHIFT)

#define PAGE_SIZE		4096
#define SMP_CACHE_BYTES		L1_CACHE_BYTES
#define L1_CACHE_ALIGN(x)	__ALIGN_KERNEL(x, L1_CACHE_BYTES)

#define WHITEOUT_MODE		0
#define WHITEOUT_DEV		0

#define NOTIFY_DONE		0x0000		/* Don't care */
#define NOTIFY_OK		0x0001		/* Suits me */
#define NOTIFY_STOP_MASK	0x8000		/* Don't call further */
#define NOTIFY_BAD		(NOTIFY_STOP_MASK|0x0002) /* Bad/Veto action */
/*
 * Clean way to return from the notifier and stop further calls.
 */
#define NOTIFY_STOP		(NOTIFY_OK|NOTIFY_STOP_MASK)

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

/* macros used for waking up a task */
#define wake_up(x)                      __wake_up(x, TASK_NORMAL, 1, NULL)
#define wake_up_nr(x, nr)               __wake_up(x, TASK_NORMAL, nr, NULL)
#define wake_up_all(x)                  __wake_up(x, TASK_NORMAL, 0, NULL)
#define wake_up_locked(x)               __wake_up_locked((x), TASK_NORMAL, 1)
#define wake_up_all_locked(x)           __wake_up_locked((x), TASK_NORMAL, 0)
#define wake_up_sync(x)                 __wake_up_sync(x, TASK_NORMAL)

#define wake_up_interruptible(x)        __wake_up(x, TASK_INTERRUPTIBLE, 1, NULL)
#define wake_up_interruptible_nr(x, nr) __wake_up(x, TASK_INTERRUPTIBLE, nr, NULL)
#define wake_up_interruptible_all(x)    __wake_up(x, TASK_INTERRUPTIBLE, 0, NULL)
#define wake_up_interruptible_sync(x)   __wake_up_sync((x), TASK_INTERRUPTIBLE)

/* sections for the crashdump */
enum {
	CRASH_SECTION_SYMBOL_TABLE,
	/* ... */
	CRASH_SECTION_MAX,
};

/* memory allocation type for the generic list used for tracking the allocated memory */
enum {
	ALLOCATION_TYPE_MMAP,
	ALLOCATION_TYPE_MALLOC, /* used only for the kernel module segments in memory */

	ALLOCATION_TYPE_MAX,
};

enum {
	DUMP_PREFIX_NONE,
	DUMP_PREFIX_ADDRESS,
	DUMP_PREFIX_OFFSET
};

/* values associated with 'ieee80211_ops' callback invocations */
enum fast_run_ops {
	IEEE80211_START,
	IEEE80211_STOP,
	IEEE80211_ADD_INTERFACE,
	IEEE80211_REMOVE_INTERFACE,
	IEEE80211_CONFIG,
	IEEE80211_CONFIGURE_FILTER,
	IEEE80211_HW_SCAN,
	IEEE80211_CANCEL_HW_SCAN,
	IEEE80211_START_AP,
	IEEE80211_STOP_AP,
	IEEE80211_MONITOR,
};

/* ktime_t based interfaces */
enum tk_offsets {
	TK_OFFS_REAL,
	TK_OFFS_BOOT,
	TK_OFFS_TAI,
	TK_OFFS_MAX,
};

/**
 * enum probe_type - device driver probe type to try
 *	Device drivers may opt in for special handling of their
 *	respective probe routines. This tells the core what to
 *	expect and prefer.
 *
 * @PROBE_DEFAULT_STRATEGY: Used by drivers that work equally well
 *	whether probed synchronously or asynchronously.
 * @PROBE_PREFER_ASYNCHRONOUS: Drivers for "slow" devices which
 *	probing order is not essential for booting the system may
 *	opt into executing their probes asynchronously.
 * @PROBE_FORCE_SYNCHRONOUS: Use this to annotate drivers that need
 *	their probe routines to run synchronously with driver and
 *	device registration (with the exception of -EPROBE_DEFER
 *	handling - re-probing always ends up being done asynchronously).
 *
 * Note that the end goal is to switch the kernel to use asynchronous
 * probing by default, so annotating drivers with
 * %PROBE_PREFER_ASYNCHRONOUS is a temporary measure that allows us
 * to speed up boot process while we are validating the rest of the
 * drivers.
 */
enum probe_type {
	PROBE_DEFAULT_STRATEGY,
	PROBE_PREFER_ASYNCHRONOUS,
	PROBE_FORCE_SYNCHRONOUS,
};

/*
 * Workqueue flags and constants.  For details, please refer to
 * Documentation/core-api/workqueue.rst.
 */
enum wq_flags {
	WQ_BH			= 1 << 0, /* execute in bottom half (softirq) context */
	WQ_UNBOUND		= 1 << 1, /* not bound to any cpu */
	WQ_FREEZABLE		= 1 << 2, /* freeze during suspend */
	WQ_MEM_RECLAIM		= 1 << 3, /* may be used for memory reclaim */
	WQ_HIGHPRI		= 1 << 4, /* high priority */
	WQ_CPU_INTENSIVE	= 1 << 5, /* cpu intensive workqueue */
	WQ_SYSFS		= 1 << 6, /* visible in sysfs, see workqueue_sysfs_register() */

	/*
	 * Per-cpu workqueues are generally preferred because they tend to
	 * show better performance thanks to cache locality.  Per-cpu
	 * workqueues exclude the scheduler from choosing the CPU to
	 * execute the worker threads, which has an unfortunate side effect
	 * of increasing power consumption.
	 *
	 * The scheduler considers a CPU idle if it doesn't have any task
	 * to execute and tries to keep idle cores idle to conserve power;
	 * however, for example, a per-cpu work item scheduled from an
	 * interrupt handler on an idle CPU will force the scheduler to
	 * execute the work item on that CPU breaking the idleness, which in
	 * turn may lead to more scheduling choices which are sub-optimal
	 * in terms of power consumption.
	 *
	 * Workqueues marked with WQ_POWER_EFFICIENT are per-cpu by default
	 * but become unbound if workqueue.power_efficient kernel param is
	 * specified.  Per-cpu workqueues which are identified to
	 * contribute significantly to power-consumption are identified and
	 * marked with this flag and enabling the power_efficient mode
	 * leads to noticeable power saving at the cost of small
	 * performance disadvantage.
	 *
	 * http://thread.gmane.org/gmane.linux.kernel/1480396
	 */
	WQ_POWER_EFFICIENT	= 1 << 7,
	WQ_PERCPU		= 1 << 8, /* bound to a specific cpu */

	__WQ_DESTROYING		= 1 << 15, /* internal: workqueue is destroying */
	__WQ_DRAINING		= 1 << 16, /* internal: workqueue is draining */
	__WQ_ORDERED		= 1 << 17, /* internal: workqueue is ordered */
	__WQ_LEGACY		= 1 << 18, /* internal: create*_workqueue() */

	/* BH wq only allows the following flags */
	__WQ_BH_ALLOWS		= WQ_BH | WQ_HIGHPRI | WQ_PERCPU,
};

enum {
	TASKLET_STATE_SCHED,	/* Tasklet is scheduled for execution */
	TASKLET_STATE_RUN	/* Tasklet is running (SMP only) */
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

struct va_format {
	const char *fmt;
	va_list *va;
};

typedef struct {
	void *cache_ptr;
	/* padding, need to be adjusted */
	//char  _pad[0x70 - sizeof(void *)];
	char _pad[128];
} _kmalloc_cache_row_t;

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

/* implement mutex too as shown in the kernel headers */

struct optimistic_spin_queue {
	/*
	 * Stores an encoded value of the CPU # of the tail node in the queue.
	 * If the queue is empty, then it's set to OSQ_UNLOCKED_VAL.
	 */
	atomic_t tail;
};

/*
 * Simple, straightforward mutexes with strict semantics:
 *
 * - only one task can hold the mutex at a time
 * - only the owner can unlock the mutex
 * - multiple unlocks are not permitted
 * - recursive locking is not permitted
 * - a mutex object must be initialized via the API
 * - a mutex object must not be initialized via memset or copying
 * - task may not exit with mutex held
 * - memory areas where held locks reside must not be freed
 * - held mutexes must not be reinitialized
 * - mutexes may not be used in hardware or software interrupt
 *   contexts such as tasklets and timers
 *
 * These semantics are fully enforced when DEBUG_MUTEXES is
 * enabled. Furthermore, besides enforcing the above rules, the mutex
 * debugging code also implements a number of additional features
 * that make lock debugging easier and faster:
 *
 * - uses symbolic names of mutexes, whenever they are printed in debug output
 * - point-of-acquire tracking, symbolic lookup of function names
 * - list of all locks held in the system, printout of them
 * - owner tracking
 * - detects self-recursing locks and prints out all relevant info
 * - detects multi-task circular deadlocks and prints out all affected
 *   locks and tasks (and only those tasks)
 */
struct mutex {
	atomic_long		owner;
	/* always consider a spinlock_t as an unsigned integer of 32 bits */
	u32			wait_lock;
	struct optimistic_spin_queue osq; /* Spinner MCS lock */
	struct list_head	wait_list;

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

/* use this trick for bypassing the 'struct device' not implemented limitation, should work for every kernel after 5.4 */
struct device {
	struct kobject		 kobj;
	struct device		*parent;
};

struct kobj_type {
	void (*release)(struct kobject *kobj);
	const struct sysfs_ops *sysfs_ops;
	struct attribute **default_attrs;
	const struct kobj_ns_type_operations *(*child_ns_type)(struct kobject *kobj);
	const void *(*namespace)(struct kobject *kobj);
	void (*get_ownership)(struct kobject *kobj, int *uid, int *gid);
};

struct cdev {
	struct kobject kobj;
	struct module *owner;
	const struct file_operations *ops;
	struct list_head list;
	u32 dev;
	unsigned int count;
};

struct usb_dev_tuple {
	__u16 usb_pid;
	__u16 usb_vid;
	bool is_used;
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

struct kernel_symbol {
#ifdef CONFIG_HAVE_ARCH_PREL32_RELOCATIONS
	int value_offset;
	int name_offset;
	int namespace_offset;
#else
	unsigned long value;
	const char *name;
	const char *namespace;
#endif
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
	unsigned char function_name[42];
	void *function_ptr;
	/* new update: add also the size of the symbol, useful for some operations at runtime */
	u16 function_size;
};

/* the 'device_driver' struct is defined inside the virtual struct generator at runtime, this is used just for 'usb_driver' */
struct device_driver {
	const char		*name;
	const struct bus_type	*bus;

	struct module		*owner;
	const char		*mod_name;	/* used for built-in modules */

	bool suppress_bind_attrs;	/* disables bind/unbind via sysfs */
	enum probe_type probe_type;

	const struct of_device_id	*of_match_table;
	const struct acpi_device_id	*acpi_match_table;

	int (*probe) (struct device *dev);
	void (*sync_state)(struct device *dev);
	int (*remove) (struct device *dev);
	void (*shutdown) (struct device *dev);
	int (*suspend) (struct device *dev, int state);
	int (*resume) (struct device *dev);
	const struct attribute_group **groups;
	const struct attribute_group **dev_groups;

	/* ... */
};

struct module_controller;
extern struct module_controller director;

#include <prototypes.h>

/* defines pthread functions for Windows below */
#ifdef _WIN64

#define __ALIGN_KERNEL(x, a)		__ALIGN_KERNEL_MASK(x, (__typeof__(x))(a) - 1)
#define __ALIGN_KERNEL_MASK(x, mask)	(((x) + (mask)) & ~(mask))

#ifdef __USE_FILE_OFFSET64
#define DWORD_HI(x) (x >> 32)
#define DWORD_LO(x) ((x) & 0xffffffff)
#else
#define DWORD_HI(x) (0)
#define DWORD_LO(x) (x)
#endif

/* taken from 'https://github.com/m-labs/uclibc-lm32/blob/master/utils/mmap-windows.c' */
static void *mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset){
	if (prot & ~(PROT_READ | PROT_WRITE | PROT_EXEC)){
		return MAP_FAILED;
	}
	if (fd == -1) {
		if (!(flags & MAP_ANON) || offset){
			return MAP_FAILED;
		}
	}else if (flags & MAP_ANON){
		return MAP_FAILED;
	}
	DWORD flProtect;
	if (prot & PROT_WRITE) {
		if (prot & PROT_EXEC){
			flProtect = PAGE_EXECUTE_READWRITE;
		}else{
			flProtect = PAGE_READWRITE;
		}
	}else if (prot & PROT_EXEC) {
		if (prot & PROT_READ){
			flProtect = PAGE_EXECUTE_READ;
		}else if (prot & PROT_EXEC){
			flProtect = PAGE_EXECUTE;
		}
	}else{
		flProtect = PAGE_READONLY;
	}

	off_t end = length + offset;
	HANDLE mmap_fd, h;
	if (fd == -1){
		mmap_fd = INVALID_HANDLE_VALUE;
	}else{
		mmap_fd = (HANDLE)_get_osfhandle(fd);
	}
	h = CreateFileMapping(mmap_fd, NULL, flProtect, DWORD_HI(end), DWORD_LO(end), NULL);
	if (h == NULL){
		return MAP_FAILED;
	}
	DWORD dwDesiredAccess;
	if (prot & PROT_WRITE){
		dwDesiredAccess = FILE_MAP_WRITE;
	}else{
		dwDesiredAccess = FILE_MAP_READ;
	}
	if (prot & PROT_EXEC){
		dwDesiredAccess |= FILE_MAP_EXECUTE;
	}
	if (flags & MAP_PRIVATE){
		dwDesiredAccess |= FILE_MAP_COPY;
	}
	void *ret = MapViewOfFile(h, dwDesiredAccess, DWORD_HI(offset), DWORD_LO(offset), length);
	if (ret == NULL) {
		CloseHandle(h);
		ret = MAP_FAILED;
	}
	return ret;
}

static int munmap(void *addr, size_t length){
	int ret = 0;
	if (addr == NULL || addr == MAP_FAILED) {
		ret = EINVAL;
		return ret;
	}

	if (VirtualFree(addr, 0, MEM_RELEASE) == NULL) {
		ret = EINVAL;
		return ret;
	}

	return ret;
}

static inline int getrandom(void *buf, int len, int unused) {
	int ret = 0;
	if (BCryptGenRandom(NULL, (PUCHAR)buf, (ULONG)len, BCRYPT_USE_SYSTEM_PREFERRED_RNG) != 0) {
		ret = -EINVAL;
		return ret;
	}
	return (int)len;
}

static inline void usleep(unsigned long usec) {
	DWORD ms = 0;
	ms       = (DWORD)((usec + 999) / 1000);
	Sleep(ms);
}

/* taken from 'https://github.com/digilus/getline/blob/master/getline.c' */
static int getline(char **lineptr, size_t *n, FILE *fp) {
	int ret = 0;
	int ch = 0;
	int i = 0;
	char free_on_err = 0;
	char *p = NULL;

	errno = 0;
	if (lineptr == NULL || n == NULL || fp == NULL) {
		errno = EINVAL;
		ret = -EINVAL;
		return ret;
	}

	if (*lineptr == NULL) {
		*n = 16; /* 'GETLINE_MINSIZE = 16 */
		*lineptr = (char *)malloc(sizeof(char) * (*n));
		if (*lineptr == NULL) {
			errno = ENOMEM;
			ret = -ENOMEM;
			return ret;
		}
		free_on_err = 1;
	}

	for (i = 0; ; i++) {
		ch = fgetc(fp);
		while (i >= (*n) - 2) {
			*n *= 2;
			p = realloc(*lineptr, sizeof(char) * (*n));
			if (p == NULL) {
				if (free_on_err){
					free(*lineptr);
				}
				errno = ENOMEM;
				ret = -ENOMEM;
				return ret;
			}
			*lineptr = p;
		}
		if (ch == EOF) {
			if (i == 0) {
				if (free_on_err){
					free(*lineptr);
				}
				return -1;
			}
			(*lineptr)[i] = '\0';
			*n = i;
			return i;
		}

		if (ch == '\n') {
			(*lineptr)[i] = '\n';
			(*lineptr)[i+1] = '\0';
			*n = i+1;
			return i + 1;
		}
		(*lineptr)[i] = (char)ch;
	}
}

/* rewrite also the 'pthread_xxx' functions but adapted for the Windows style */
struct pthread_start {
        void *(*fn)(void *);
        void *arg;
};

static inline void pthread_mutex_init(pthread_mutex_t *mutex, void *unused) {
        InitializeSRWLock(mutex);
}

static inline void pthread_mutex_lock(pthread_mutex_t *mutex) {
        AcquireSRWLockExclusive(mutex);
}

static inline void pthread_mutex_unlock(pthread_mutex_t *mutex) {
        ReleaseSRWLockExclusive(mutex);
}

static inline void pthread_cond_init(pthread_cond_t *cond, void *unused) {
        InitializeConditionVariable(cond);
}

static inline void pthread_cond_signal(pthread_cond_t *cond) {
        WakeConditionVariable(cond);
}

static inline int pthread_cond_wait(pthread_cond_t  *cond, pthread_mutex_t *mutex) {
        int  ret = 0;
        BOOL ok  = false;
        ok       = SleepConditionVariableSRW(cond, mutex, INFINITE, 0);
        if (ok) {
                ret = 0;
        } else {
                ret = -1;
        }
        return ret;
}

static inline int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, DWORD timeout_ms){
	int ret = 0;
	BOOL ok = false;

	ok = SleepConditionVariableSRW(cond, mutex, timeout_ms, 0);
	if (ok){
		return ret;
	}
	return (GetLastError() == ERROR_TIMEOUT) ? 1 : -1;
}

static int pthread_cond_destroy(pthread_cond_t *cond){
	int ret = 0;
	return ret;
}

static int pthread_mutex_destroy(pthread_mutex_t *mutex){
	int ret = 0;
	return ret;
}

static int pthread_equal(pthread_t t1, pthread_t t2){
	int ret = 0;
	DWORD id1 = 0;
	DWORD id2 = 0;

	if (t1 == NULL || t2 == NULL){
		return ret;
	}

	id1 = GetThreadId(t1);
	id2 = GetThreadId(t2);
	if (id1 == 0 || id2 == 0){
		return ret;
	}
	return id1 == id2;
}

static int pthread_condattr_init(pthread_condattr_t *attr){
	int ret = 0;
	if (attr == NULL) {
		ret = -EINVAL;
		return ret;
	}

	*attr = CLOCK_REALTIME;
	return ret;
}

static int pthread_condattr_setclock(pthread_condattr_t *attr, int clock_id){
	int ret = 0;
	if (attr == NULL || clock_id != CLOCK_MONOTONIC) {
		ret = -EINVAL;
		return ret;
	}

	*attr = clock_id;
	return ret;
}

static int pthread_condattr_destroy(pthread_condattr_t *attr){
	int ret = 0;
	if (attr == NULL) {
		ret = -EINVAL;
		return ret;
	}
	*attr = 0;
	return ret;
}

static int pthread_cond_broadcast(pthread_cond_t *cond){
	int ret = 0;
	if (cond == NULL) {
		ret = -EINVAL;
		return ret;
	}

	WakeAllConditionVariable(cond);
	return ret;
}

static int pthread_attr_init(pthread_attr_t *attr){
	int ret = 0;
	if (attr == NULL) {
		ret = -EINVAL;
		return ret;
	}

	*attr = CLOCK_REALTIME;
	return ret;
}

static int pthread_attr_destroy(pthread_attr_t *attr){
	int ret = 0;
	if (attr == NULL) {
		ret = EINVAL;
		return ret;
	}

	*attr = 0;
	return ret;
}

static pthread_t pthread_self(void){
	return GetCurrentThreadId();
}

static unsigned int pthread_trampoline(void *p) {
        unsigned int          ret        = 0;
        struct pthread_start *s          = NULL;
        void                 *return_val = NULL;
        s                                = (struct pthread_start *)p;
        return_val                       = s->fn(s->arg);
        HeapFree(GetProcessHeap(), 0, s);
        ret = (unsigned)(uintptr_t)return_val;
        return ret;
}

static BOOL CALLBACK pthread_once_trampoline(PINIT_ONCE once, PVOID param, PVOID *ctx){
	pthread_once_fn fn = (pthread_once_fn)param;
	fn();
	return TRUE;
}

static inline int pthread_create(pthread_t *out, void *attr, void *(*start_routine)(void *), void *arg) {
        int                   ret = 0;
        unsigned int          tid = 0;
        struct pthread_start *s   = NULL;
        HANDLE                h;

        s = (struct pthread_start *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*s));
        if (!s) {
                ret = -1;
                return ret;
        }
        s->fn  = start_routine;
        s->arg = arg;

        h = (HANDLE)_beginthreadex(NULL, 0, pthread_trampoline, s, 0, &tid);
        if (!h) {
                HeapFree(GetProcessHeap(), 0, s);
                ret = -1;
                return ret;
        }

        *out = h;
        return ret;
}

static inline int pthread_once(pthread_once_t *once_control, pthread_once_fn init_routine){
	BOOL ok = InitOnceExecuteOnce(once_control, pthread_once_trampoline, (PVOID)init_routine, NULL);
	return ok ? 0 : -1;
}

static inline int pthread_join(HANDLE th, DWORD *exit_code_out) {
        int   ret  = 0;
        DWORD code = 0;
        DWORD wr   = 0;
        wr         = WaitForSingleObject(th, INFINITE);
        if (wr != WAIT_OBJECT_0) {
                ret = -1;
                return ret;
        }

        if (exit_code_out) {
                code = 0;
                if (!GetExitCodeThread(th, &code)) {
                        ret = -1;
                        return ret;
                }
                *exit_code_out = code;
        }

        CloseHandle(th);
        return ret;
}

static void __pthread_cancel_apc(ULONG_PTR unused){
        _endthreadex((unsigned)0xDEAD);
}

static inline int pthread_cancel(pthread_t th){
        int ret = 0;
        DWORD wr = 0;

        CancelSynchronousIo(th);

        QueueUserAPC(__pthread_cancel_apc, th, 0);

        wr = WaitForSingleObject(th, 50);
        if (wr == WAIT_OBJECT_0){
                return ret;
        }

        if (wr == WAIT_FAILED){
                ret = -EAGAIN;
                return ret;
        }

        if (!TerminateThread(th, (DWORD)0xDEAD)){
                ret = -EAGAIN;
                return ret;
        }

        return ret;
}

static int clock_gettime_realtime(struct timespec *ts) {
	int ret = 0;
	FILETIME ft;
	ULARGE_INTEGER ui;
	GetSystemTimePreciseAsFileTime(&ft);

	ui.LowPart  = ft.dwLowDateTime;
	ui.HighPart = ft.dwHighDateTime;

	uint64_t t = ui.QuadPart - 116444736000000000ULL;
	ts->tv_sec  = (time_t)(t / 10000000ULL);
	ts->tv_nsec = (long)((t % 10000000ULL) * 100);
	return ret;
}

static int clock_gettime_monotonic(struct timespec *ts) {
	int ret = 0;
	LARGE_INTEGER freq;
	int init = 0;
	LARGE_INTEGER now;

	if (!init) {
		QueryPerformanceFrequency(&freq);
		init = 1;
	}

	QueryPerformanceCounter(&now);

	ts->tv_sec  = (time_t)(now.QuadPart / freq.QuadPart);
	ts->tv_nsec = (long)(((now.QuadPart % freq.QuadPart) * 1000000000LL) / freq.QuadPart);
	return ret;
}

static int timer_delete (void *__timerid){

}
#endif

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

/**
 * list_cut_before - cut a list into two, before given entry
 * @list: a new list to add all removed entries
 * @head: a list with entries
 * @entry: an entry within head, could be the head itself
 *
 * This helper moves the initial part of @head, up to but
 * excluding @entry, from @head to @list.  You should pass
 * in @entry an element you know is on @head.  @list should
 * be an empty list or a list you do not care about losing
 * its data.
 * If @entry == @head, all entries on @head are moved to
 * @list.
 */
static inline void list_cut_before(struct list_head *list, struct list_head *head, struct list_head *entry){
	if (head->next == entry) {
		INIT_LIST_HEAD(list);
		return;
	}
	list->next = head->next;
	list->next->prev = list;
	list->prev = entry->prev;
	list->prev->next = list;
	head->next = entry;
	entry->prev = head;
}

static inline void __list_splice(const struct list_head *list, struct list_head *prev, struct list_head *next){
	struct list_head *first = list->next;
	struct list_head *last = list->prev;

	first->prev = prev;
	prev->next = first;

	last->next = next;
	next->prev = last;
}

/**
 * list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
static inline void list_splice_init(struct list_head *list, struct list_head *head){
	if (!list_empty(list)) {
		__list_splice(list, head, head->next);
		INIT_LIST_HEAD(list);
	}
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
		#ifdef LIST_SECURE_CHECK
		ret &= __list_add_valid_or_report(new, prev, next);
		#endif
		return ret;
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
		#ifdef LIST_SECURE_CHECK
		ret &= __list_del_entry_valid_or_report(entry);
		#endif
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
 * list_del_rcu - deletes entry from list without re-initialization
 * @entry: the element to delete from the list.
 *
 * Note: list_empty() on entry does not return true after this,
 * the entry is in an undefined state. It is useful for RCU based
 * lockfree traversal.
 *
 * In particular, it means that we can not poison the forward
 * pointers that may still be used for walking the list.
 *
 * The caller must take whatever precautions are necessary
 * (such as holding appropriate locks) to avoid racing
 * with another list-mutation primitive, such as list_del_rcu()
 * or list_add_rcu(), running on this same list.
 * However, it is perfectly legal to run concurrently with
 * the _rcu list-traversal primitives, such as
 * list_for_each_entry_rcu().
 *
 * Note that the caller is not permitted to immediately free
 * the newly deleted entry.  Instead, either synchronize_rcu()
 * or call_rcu() must be used to defer freeing until an RCU
 * grace period has elapsed.
 */
static inline void list_del_rcu(struct list_head *entry){
	__list_del_entry(entry);
	entry->prev = LIST_POISON2;
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

/**
 * kref_init - initialize object.
 * @kref: object in question.
 */
static inline void kref_init(struct kref *kref){
	#ifdef __clang__
	__c11_atomic_store(&kref->refcount.refs, 1, __ATOMIC_RELEASE);
	#else
	__atomic_store_n(&kref->refcount.refs, 1, __ATOMIC_RELEASE);
	#endif
}

static inline void kref_get(struct kref *kref){
	refcount_inc(&kref->refcount);
}

/**
 * kref_put - Decrement refcount for object
 * @kref: Object
 * @release: Pointer to the function that will clean up the object when the
 *	     last reference to the object is released.
 *
 * Decrement the refcount, and if 0, call @release.  The caller may not
 * pass NULL or kfree() as the release function.
 *
 * Return: 1 if this call removed the object, otherwise return 0.  Beware,
 * if this function returns 0, another caller may have removed the object
 * by the time this function returns.  The return value is only certain
 * if you want to see if the object is definitely released.
 */
static inline int kref_put(struct kref *kref, void (*release)(struct kref *kref)){
	int ret = false;
	if (refcount_dec_and_test(&kref->refcount)) {
		release(kref);
		ret = true;
		return ret;
	}
	return ret;
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

/**
 * ERR_CAST - Explicitly cast an error-valued pointer to another pointer type
 * @ptr: The pointer to cast.
 *
 * Explicitly cast an error-valued pointer to another pointer type in such a
 * way as to make it clear that's what's going on.
 */
static inline void *ERR_CAST(const void *ptr){
	/* cast away the const */
	return (void *) ptr;
}

/**
 * IS_ERR - Detect an error pointer.
 * @ptr: The pointer to check.
 * Return: true if @ptr is an error pointer, false otherwise.
 */
static inline bool IS_ERR(__force const void *ptr){
	return IS_ERR_VALUE((unsigned long)ptr);
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

static inline void rb_link_node(struct rb_node *node, struct rb_node *rb_parent, struct rb_node **rb_link){
	node->__rb_parent_color = (unsigned long)rb_parent;
	node->rb_left = node->rb_right = NULL;

	*rb_link = node;
}

static inline void __rb_change_child(struct rb_node *old, struct rb_node *new, struct rb_node *rb_parent, struct rb_root *root){
	if (rb_parent) {
		if (rb_parent->rb_left == old){
			WRITE_ONCE(rb_parent->rb_left, new);
		}else{
			WRITE_ONCE(rb_parent->rb_right, new);
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
	struct rb_node *rb_parent = rb_parent(old);
	new->__rb_parent_color = old->__rb_parent_color;
	rb_set_parent_color(old, new, color);
	__rb_change_child(old, new, rb_parent, root);
}

static inline void __rb_insert(struct rb_node *node, struct rb_root *root, void (*augment_rotate)(struct rb_node *old, struct rb_node *new)){
	struct rb_node *rb_parent = rb_red_parent(node);
	struct rb_node *gparent = NULL;
	struct rb_node *tmp = NULL;

	while (true) {
		/*
		 * Loop invariant: node is red.
		 */
		if (unlikely(!rb_parent)) {
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
		if(rb_is_black(rb_parent)){
			break;
		}

		gparent = rb_red_parent(rb_parent);

		tmp = gparent->rb_right;
		if (rb_parent != tmp) {	/* parent == gparent->rb_left */
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
				rb_set_parent_color(rb_parent, gparent, RB_BLACK);
				node = gparent;
				rb_parent = rb_parent(node);
				rb_set_parent_color(node, rb_parent, RB_RED);
				continue;
			}

			tmp = rb_parent->rb_right;
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
				WRITE_ONCE(rb_parent->rb_right, tmp);
				WRITE_ONCE(node->rb_left, rb_parent);
				if (tmp){
					rb_set_parent_color(tmp, rb_parent, RB_BLACK);
				}
				rb_set_parent_color(rb_parent, node, RB_RED);
				augment_rotate(rb_parent, node);
				rb_parent = node;
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
			WRITE_ONCE(rb_parent->rb_right, gparent);
			if (tmp){
				rb_set_parent_color(tmp, gparent, RB_BLACK);
			}
			__rb_rotate_set_parents(gparent, rb_parent, root, RB_RED);
			augment_rotate(gparent, rb_parent);
			break;
		} else {
			tmp = gparent->rb_left;
			if (tmp && rb_is_red(tmp)) {
				/* Case 1 - color flips */
				rb_set_parent_color(tmp, gparent, RB_BLACK);
				rb_set_parent_color(rb_parent, gparent, RB_BLACK);
				node = gparent;
				rb_parent = rb_parent(node);
				rb_set_parent_color(node, rb_parent, RB_RED);
				continue;
			}

			tmp = rb_parent->rb_left;
			if (node == tmp) {
				/* Case 2 - right rotate at parent */
				tmp = node->rb_right;
				WRITE_ONCE(rb_parent->rb_left, tmp);
				WRITE_ONCE(node->rb_right, rb_parent);
				if (tmp){
					rb_set_parent_color(tmp, rb_parent, RB_BLACK);
				}
				rb_set_parent_color(rb_parent, node, RB_RED);
				augment_rotate(rb_parent, node);
				rb_parent = node;
				tmp = node->rb_left;
			}

			/* Case 3 - left rotate at gparent */
			WRITE_ONCE(gparent->rb_right, tmp); /* == parent->rb_left */
			WRITE_ONCE(rb_parent->rb_left, gparent);
			if (tmp){
				rb_set_parent_color(tmp, gparent, RB_BLACK);
			}
			__rb_rotate_set_parents(gparent, rb_parent, root, RB_RED);
			augment_rotate(gparent, rb_parent);
			break;
		}
	}
}

static inline const char *kobject_name(const struct kobject *kobj){
	return (const char *)kobj->name;
}

static void __kobject_del(struct kobject *kobj){
	kobj->state_in_sysfs = 0;
	list_del_init(&kobj->entry);
	kobj->parent = NULL;
}

/*
 * kobject_cleanup - free kobject resources.
 * @kobj: object to cleanup
 */
static void kobject_cleanup(struct kobject *kobj){
	struct kobject *rb_parent = kobj->parent;
	const struct kobj_type *t = kobj->ktype;
	const char *name = kobj->name;

	/* remove from sysfs if the caller did not do it */
	if (kobj->state_in_sysfs) {
		__kobject_del(kobj);
	} else {
		/* avoid dropping the parent reference unnecessarily */
		rb_parent = NULL;
	}

	if (t && t->release != NULL) {
		t->release(kobj);
	}

	/* free name if we allocated it */
	if (name) {
		kfree_const(name);
	}

	kobject_put(rb_parent);
}

static void kobject_release(struct kref *kref){
	struct kobject *kobj = container_of(kref, struct kobject, kref);
	kobject_cleanup(kobj);
}

static inline void mutex_init(struct mutex *lock, const char *name, void *key){
	mutex_init_generic(lock);
}

static void __mutex_init_generic(struct mutex *lock){
	atomic_store_explicit(&lock->owner, 0, memory_order_release);
	INIT_LIST_HEAD(&lock->wait_list);
}

static inline int spin_lock_init(pthread_mutex_t *lock){
	pthread_mutex_init(lock, NULL);
}

static inline int spin_lock(pthread_mutex_t *lock){
	pthread_mutex_lock(lock);
}

static inline int spin_unlock(pthread_mutex_t *lock){
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
static unsigned long strtoul_with_range(const char *nptr, char **endptr, int base, unsigned long max) {
	unsigned long l = strtoul(nptr, endptr, base);
	if(l > max) {
		errno = ERANGE;
		return max;
	}
	return l;
}

static int try_consume_completion(struct completion *x){
	int ret = 0;
	int v = 0;
	v = atomic_load_explicit(&x->done, memory_order_acquire);

	while (v > 0) {
		if (atomic_compare_exchange_weak_explicit(&x->done, &v, v - 1, memory_order_acq_rel, memory_order_acquire)) {
			ret = true;
			return ret;
		}
	}
	return ret;
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

#include <single_offset.h>
#include <struct_offset.h>
#include <mod_controller.h>
#include <controller_utils.h>
#include <hashtable.h>
#include <wq.h>
#include <jiffies.h>
#include <usb.h>
#include <bits.h>
#include <skb.h>
#include <net.h>
#include <hci.h>
#include <crypto.h>
#include <ieee80211.h>

#endif
