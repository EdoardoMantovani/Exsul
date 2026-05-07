#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <linux/const.h>
#include <linux/nl80211.h>
#include <linux/if_ether.h>
#include <libusb-1.0/libusb.h>
#include <libdwarf/libdwarf.h>

#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

#define s8  int8_t
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

#define IEEE80211_NUM_ACS    4
#define IEEE80211_MAX_QUEUES 16
#define MAX_FILE_IDX         32
#define IEEE80211_MAX_SSID_LEN 32
#define USB_MAX_ENDPOINT_NUM 30
#define MAX_KMEM_ARRAY_SIZE  36
#define MAX_USB_DEV_LIST     36
#define MAX_DEBUGFS_SIZE     128
#define MAX_USERSPACE_TO_DRIVER_IO_SIZE	256
#define MAX_DRIVER_TO_USERSPACE_IO_SIZE 256
#define IEEE80211_HT_MCS_MASK_LEN       10

#define atomic_bool bool

#define BITS_PER_BYTE           8
#define BITS_PER_TYPE(type)     (sizeof(type) * BITS_PER_BYTE)

#define DIV_ROUND_UP(n,d)       (((n) + (d) - 1) / (d))
#define BITS_TO_LONGS(nr)       __KERNEL_DIV_ROUND_UP(nr, BITS_PER_TYPE(long))

#define rcu_head		callback_head
#define __rcu
#define __packed		__attribute__((packed))
#define __aligned(x)		__attribute__((aligned(x)))

#define struct_group_tagged(TAG, NAME, MEMBERS...)                             \
        __struct_group(TAG, NAME, /* no attrs */, MEMBERS)
#define struct_group(NAME, MEMBERS...)                                         \
        __struct_group(/* no tag */, NAME, /* no attrs */, MEMBERS)

#ifndef __struct_group
#define __struct_group_tag(TAG)
#define __struct_group(TAG, NAME, ATTRS, MEMBERS...)                           \
        union {                                                                \
                struct {                                                       \
                        MEMBERS                                                \
                } ATTRS;                                                       \
                struct __struct_group_tag(TAG){MEMBERS} ATTRS NAME;            \
        } ATTRS
#endif

enum queue_stop_reason {
        IEEE80211_QUEUE_STOP_REASON_DRIVER,
        IEEE80211_QUEUE_STOP_REASON_PS,
        IEEE80211_QUEUE_STOP_REASON_CSA,
        IEEE80211_QUEUE_STOP_REASON_AGGREGATION,
        IEEE80211_QUEUE_STOP_REASON_SUSPEND,
        IEEE80211_QUEUE_STOP_REASON_SKB_ADD,
        IEEE80211_QUEUE_STOP_REASON_OFFCHANNEL,
        IEEE80211_QUEUE_STOP_REASON_FLUSH,
        IEEE80211_QUEUE_STOP_REASON_TDLS_TEARDOWN,
        IEEE80211_QUEUE_STOP_REASON_RESERVE_TID,
        IEEE80211_QUEUE_STOP_REASON_IFTYPE_CHANGE,

        IEEE80211_QUEUE_STOP_REASONS,
};

enum ieee80211_edmg_bw_config {
	IEEE80211_EDMG_BW_CONFIG_4	= 4,
	IEEE80211_EDMG_BW_CONFIG_5	= 5,
	IEEE80211_EDMG_BW_CONFIG_6	= 6,
	IEEE80211_EDMG_BW_CONFIG_7	= 7,
	IEEE80211_EDMG_BW_CONFIG_8	= 8,
	IEEE80211_EDMG_BW_CONFIG_9	= 9,
	IEEE80211_EDMG_BW_CONFIG_10	= 10,
	IEEE80211_EDMG_BW_CONFIG_11	= 11,
	IEEE80211_EDMG_BW_CONFIG_12	= 12,
	IEEE80211_EDMG_BW_CONFIG_13	= 13,
	IEEE80211_EDMG_BW_CONFIG_14	= 14,
	IEEE80211_EDMG_BW_CONFIG_15	= 15,
};

enum ieee80211_smps_mode {
	IEEE80211_SMPS_AUTOMATIC,
	IEEE80211_SMPS_OFF,
	IEEE80211_SMPS_STATIC,
	IEEE80211_SMPS_DYNAMIC,

	/* keep last */
	IEEE80211_SMPS_NUM_MODES,
};

enum ieee80211_hw_flags {
	IEEE80211_HW_HAS_RATE_CONTROL,
	IEEE80211_HW_RX_INCLUDES_FCS,
	IEEE80211_HW_HOST_BROADCAST_PS_BUFFERING,
	IEEE80211_HW_SIGNAL_UNSPEC,
	IEEE80211_HW_SIGNAL_DBM,
	IEEE80211_HW_NEED_DTIM_BEFORE_ASSOC,
	IEEE80211_HW_SPECTRUM_MGMT,
	IEEE80211_HW_AMPDU_AGGREGATION,
	IEEE80211_HW_SUPPORTS_PS,
	IEEE80211_HW_PS_NULLFUNC_STACK,
	IEEE80211_HW_SUPPORTS_DYNAMIC_PS,
	IEEE80211_HW_MFP_CAPABLE,
	IEEE80211_HW_WANT_MONITOR_VIF,
	IEEE80211_HW_NO_VIRTUAL_MONITOR,
	IEEE80211_HW_NO_AUTO_VIF,
	IEEE80211_HW_SW_CRYPTO_CONTROL,
	IEEE80211_HW_SUPPORT_FAST_XMIT,
	IEEE80211_HW_REPORTS_TX_ACK_STATUS,
	IEEE80211_HW_CONNECTION_MONITOR,
	IEEE80211_HW_QUEUE_CONTROL,
	IEEE80211_HW_SUPPORTS_PER_STA_GTK,
	IEEE80211_HW_AP_LINK_PS,
	IEEE80211_HW_TX_AMPDU_SETUP_IN_HW,
	IEEE80211_HW_SUPPORTS_RC_TABLE,
	IEEE80211_HW_P2P_DEV_ADDR_FOR_INTF,
	IEEE80211_HW_TIMING_BEACON_ONLY,
	IEEE80211_HW_SUPPORTS_HT_CCK_RATES,
	IEEE80211_HW_CHANCTX_STA_CSA,
	IEEE80211_HW_SUPPORTS_CLONED_SKBS,
	IEEE80211_HW_SINGLE_SCAN_ON_ALL_BANDS,
	IEEE80211_HW_TDLS_WIDER_BW,
	IEEE80211_HW_SUPPORTS_AMSDU_IN_AMPDU,
	IEEE80211_HW_BEACON_TX_STATUS,
	IEEE80211_HW_NEEDS_UNIQUE_STA_ADDR,
	IEEE80211_HW_SUPPORTS_REORDERING_BUFFER,
	IEEE80211_HW_USES_RSS,
	IEEE80211_HW_TX_AMSDU,
	IEEE80211_HW_TX_FRAG_LIST,
	IEEE80211_HW_REPORTS_LOW_ACK,
	IEEE80211_HW_SUPPORTS_TX_FRAG,
	IEEE80211_HW_SUPPORTS_TDLS_BUFFER_STA,
	IEEE80211_HW_DOESNT_SUPPORT_QOS_NDP,
	IEEE80211_HW_BUFF_MMPDU_TXQ,
	IEEE80211_HW_SUPPORTS_VHT_EXT_NSS_BW,
	IEEE80211_HW_STA_MMPDU_TXQ,
	IEEE80211_HW_TX_STATUS_NO_AMPDU_LEN,
	IEEE80211_HW_SUPPORTS_MULTI_BSSID,
	IEEE80211_HW_SUPPORTS_ONLY_HE_MULTI_BSSID,
	IEEE80211_HW_AMPDU_KEYBORDER_SUPPORT,
	IEEE80211_HW_SUPPORTS_TX_ENCAP_OFFLOAD,
	IEEE80211_HW_SUPPORTS_RX_DECAP_OFFLOAD,
	IEEE80211_HW_SUPPORTS_CONC_MON_RX_DECAP,
	IEEE80211_HW_DETECTS_COLOR_COLLISION,
	IEEE80211_HW_MLO_MCAST_MULTI_LINK_TX,
	IEEE80211_HW_DISALLOW_PUNCTURING,
	IEEE80211_HW_HANDLES_QUIET_CSA,
	IEEE80211_HW_STRICT,

	/* keep last, obviously */
	NUM_IEEE80211_HW_FLAGS
};

typedef void (*work_func_t)(struct work_struct *work);

typedef u32 (*rht_hashfn_t)(void *data, u32 len, u32 seed);
typedef u32 (*rht_obj_hashfn_t)(void *data, u32 len, u32 seed);
typedef int (*rht_obj_cmpfn_t)(struct rhashtable_compare_arg *arg, void *obj);
typedef void (*wiphy_work_func_t)(struct wiphy *, struct wiphy_work *);

typedef struct {
        int counter;
} atomic_t;

struct global_conf {
	u8 show_struct_offset;
	u8 strict_missing_syms;
};

struct fast_struct_size {
        u16 ieee80211_hw;
        u16 ieee80211_ops;
	u16 ieee80211_vif;
	u16 ieee80211_bss_conf;
        u16 module;
        u16 net_device;
        u16 rate_info;
        u16 sk_buff;
        u16 sk_buff_head;
        u16 skb_shared_info;
        u16 sock;
        u16 urb;
        u16 usb_device;
        u16 usb_driver;
        u16 usb_interface;
        u16 device;
        u16 hci_dev;
};

/* 'struct file' transation layer */
struct file_layer {
        /* play with the file pointer, associate a fd with an file pointer used by the kernel module */
        void *original_file_ptr;
        int actual_fd;
        int actual_offset;
};

struct usb_dev_tuple {
        __u16 usb_pid;
        __u16 usb_vid;
        bool is_used;
};

struct runtime_single_fields {
        u8 module_mem_size_offset;
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

struct tasklet_struct {
        struct tasklet_struct *next;
        unsigned long state;
        unsigned int count;
        bool use_callback;
        union {
                void (*func)(unsigned long data);
                void (*callback)(struct tasklet_struct *t);
        };
        unsigned long data;
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

struct work_struct {
	long data;
        struct list_head entry;
        work_func_t func;
};

struct optimistic_spin_queue {
        atomic_t tail;
};

struct mutex {
        long             owner;
        /* always consider a spinlock_t as an unsigned integer of 32 bits */
        u32                     wait_lock;
        struct optimistic_spin_queue osq; /* Spinner MCS lock */
        struct list_head        wait_list;

};

struct rhashtable_params {
        u16                     nelem_hint;
        u16                     key_len;
        u16                     key_offset;
        u16                     head_offset;
        unsigned int            max_size;
        u16                     min_size;
        bool                    automatic_shrinking;
        rht_hashfn_t            hashfn;
        rht_obj_hashfn_t        obj_hashfn;
        rht_obj_cmpfn_t         obj_cmpfn;
};

struct rhashtable {
        struct bucket_table __rcu       *tbl;
        unsigned int                    key_len;
        unsigned int                    max_elems;
        struct rhashtable_params        p;
        bool                            rhlist;
        struct work_struct              run_work;
        struct mutex                    mutex;
        u32                             lock;
        int	                        nelems;
	/* usually not enabled, not really useful but mantained for consistency with the Linux kernel */
	#ifdef CONFIG_MEM_ALLOC_PROFILING
        struct alloc_tag                *alloc_tag;
	#endif
};

/**
 * struct rhltable - Hash table with duplicate objects in a list
 * @ht: Underlying rhtable
 */
struct rhltable {
        struct rhashtable ht;
};

struct module_controller {
        char module_name[16];
	/* current module major, minor and patch version */
	u8 kernel_major_version;
	u8 kernel_minor_version;
	u8 kernel_patch;
	/* mantain the count of the loaded modules in the current address space */
	int modules_loaded;
	/* mantain a track of the current memory used for everything, in bytes */
	int total_memory_used;
	int fd;
	u8 *trampoline_area;
        int trampoline_used;

	/* emulates jiffies */
	u64 jiffies;

        int libusb_ret;

	/* socket file handler used for the standalone ELF ipc */
	int socket_fd;

	/* socket flags */
	int socket_flags;

	/* ipc client */
	int client_flags;

	/* global configuration struct, the field can be set at runtime via the 'set' command */
	struct global_conf global_conf;
        struct module *mod;
        struct load_info *info;
        struct internal_cb_table *internal_cb_table;
	struct runtime_struct *runtime_generated_structs;

	/* lock used for the hashtable add/removal of an item */
	pthread_mutex_t rhashtable_lock;

        /* libusb data, use only 1 device handler for an entire application */
        libusb_device_handle *usb_dev;
	/* pointers to the 'usb_device' structs allocated for the 'probe' callback */
	void *usb_device_ptr;

	/* backup args used for running the 'xxx_driver_init' function */
	struct usb_interface *usb_interface;
	struct usb_device_id *usb_device_id;

	struct file_layer file_table[MAX_FILE_IDX];

        /* data gathered when the usb device is attached to the host system */
        u8                                usb_interface_number;
        u8                                usb_endpoint_num;
        struct libusb_endpoint_descriptor usb_ep[USB_MAX_ENDPOINT_NUM];
        struct libusb_device_descriptor   usb_dev_desc;
	pthread_mutex_t			  urb_anchor_mutex;

	pthread_mutex_t			  skb_queue_lock;

	/* threads associated with libusb's async urb transfer, permits to implement the 'usb_send_urb' kernel function */
	pthread_t			  libusb_event_thread;
	atomic_bool			  libusb_thread_stop;

	/* locks for usb i/o operations */
	pthread_mutex_t usb_io_lock;
	pthread_cond_t  usb_io_idle;
	int usb_io_depth;

	/* init driver function */
	int (*driver_init_function)(void);
	/* backup copy of the 'usb_device_id' table */
	void *usb_table_ptr;
	int usb_table_size;

	u8 **envp;
	/* base pointer of the region header mapped in memory, used for creating the userspace independent executable */
	u8 segments_in_memory;
	void *segment_header_in_memory;

	/* tcc scripting data */
	#ifdef TCC_SCRIPTING
	TCCState *tcc_ctx;
	unsigned char *tcc_memarea;
	pthread_mutex_t tcc_mutex;
	#endif

	/* special flags used for indicating that the offsets have already been given by a configuration file, this permits to skip the dwarf parsing part during the lkm loading */
	u8 offset_init_by_conf;
        /* provide the support for loading multiple lkm, permitting thus to reuse the functions from every lkm loaded */
        u8 loaded_lkm_num;
	/* track every memory region mapped in the actual memory, it doesn't matter much the original module they belongs to */
	struct allocation_list *module_allocation_list;

        struct rfkill *rfkill;
	struct ieee80211_local *ieee80211_local;
        struct cfg80211_registered_device *rdev;

	/* store here the backup for every 'ops' struct, it will be called later via the 'ops' command */
	struct ieee80211_ops *ieee80211_ops;

	/* array of pointers which emulates the functions 'devm_kxxx', when the driver will quit, every element of the array will be freed */
	void *kmem_array[MAX_KMEM_ARRAY_SIZE];

	struct usb_dev_tuple usb_dev_list[MAX_USB_DEV_LIST];

	u8 debugfs_buffer[MAX_DEBUGFS_SIZE];
	/* create 2 buffers: one for emulating the userspace-driver data input and the other for emulating the driver-userspace communication */
	u8 userspace_to_driver_buffer[MAX_USERSPACE_TO_DRIVER_IO_SIZE];
	u8 driver_to_userspace_buffer[MAX_DRIVER_TO_USERSPACE_IO_SIZE];

	/* dwarf data, used for handling the libdwarf context */
	Dwarf_Debug dbg;
	Dwarf_Error err;

	struct runtime_single_fields runtime_single_fields;

	struct list_head wakeup_sources;

	/* global workqueues */
	struct workqueue_struct system_wq;
	struct workqueue_struct system_freezable_wq;
	struct workqueue_struct system_dfl_wq;

	/* structures used for seq/procfs/sysfs virtual files */
	struct seq_list seq_list;
	struct seq_file *current_seq_file;

	/* firmware builtin data */
	#ifdef USE_BUILTIN_FIRMWARE
	struct firmware_builtin_space *fw_builtin;
	#endif
	struct firmware *firmware_ptr;

	/* list of the allocated memory, used for freeing everything at the end */
	struct allocation_list *allocation_list;

	/* store some important struct sizes, done once foreach module loaded */
	struct fast_struct_size struct_sizes;

	/* timing synchronization primitives used in 'timer_xxx' functions */
	pthread_mutex_t   	timer_lock;
	pthread_cond_t    	timer_cond;
	struct hlist_head 	timer_head;
	pthread_once_t  	timer_once;
	pthread_t 		timer_thread;
	struct timespec 	timer_base;
	/* pointer to the timer whose callback is currently executing, NULL otherwise */
	struct timer_list	*timer_executing;

	/* hci specific data */
	struct list_head hci_dev_list;
	struct list_head hci_cb_list;
};

struct ieee80211_edmg {
	u8 channels;
	enum ieee80211_edmg_bw_config bw_config;
};

struct cfg80211_chan_def {
	struct ieee80211_channel *chan;
	enum nl80211_chan_width width;
	u32 center_freq1;
	u32 center_freq2;
	struct ieee80211_edmg edmg;
	u16 freq1_offset;
	u16 punctured;
	bool s1g_primary_2mhz;
};

struct ieee80211_chan_req {
        struct cfg80211_chan_def oper;
        struct cfg80211_chan_def ap;
};

struct ieee80211_conf {
	u32 flags;
	int power_level;
	int dynamic_ps_timeout;

	u16 listen_interval;
	u8 ps_dtim_period;

	u8 long_frame_max_tx_count;
	u8 short_frame_max_tx_count;

	struct cfg80211_chan_def chandef;
	bool radar_enabled;
	enum ieee80211_smps_mode smps_mode;
};

/* do not reproduce the real 'ieee80211_hw' struct, it will be used for other purposes. */
struct ieee80211_emulated_hw {
	struct ieee80211_conf conf;
	struct wiphy *wiphy;
	const char *rate_control_algorithm;
	void *priv;
	unsigned long flags[BITS_TO_LONGS(NUM_IEEE80211_HW_FLAGS)];
	unsigned int extra_tx_headroom;
	unsigned int extra_beacon_tailroom;
	int sta_data_size;
	int chanctx_data_size;
	int txq_data_size;
	u16 queues;
	u16 max_listen_interval;
	s8 max_signal;
	u8 max_rates;
	u8 max_report_rates;
	u8 max_rate_tries;
	u16 max_rx_aggregation_subframes;
	u16 max_tx_aggregation_subframes;
	u8 max_tx_fragments;
	u8 offchannel_tx_hw_queue;
	u8 radiotap_mcs_details;
	u16 radiotap_vht_details;
	struct {
		int units_pos;
		s16 accuracy;
	} radiotap_timestamp;
	u64 netdev_features;
	u8 uapsd_queues;
	u8 uapsd_max_sp_len;
	u8 max_nan_de_entries;
	u8 tx_sk_pacing_shift;
	u8 weight_multiplier;
	u32 max_mtu;
	const s8 *tx_power_levels;
	u8 max_txpwr_levels_idx;
};

struct ieee80211_mcs_info {
        u8 rx_mask[IEEE80211_HT_MCS_MASK_LEN];
        __le16 rx_highest;
        u8 tx_params;
        u8 reserved[3];
} __packed;

struct ieee80211_sta_ht_cap {
        u16 cap; /* use IEEE80211_HT_CAP_ */
        bool ht_supported;
        u8 ampdu_factor;
        u8 ampdu_density;
        struct ieee80211_mcs_info mcs;
};

struct ieee80211_vht_mcs_info {
        __le16 rx_mcs_map;
        __le16 rx_highest;
        __le16 tx_mcs_map;
        __le16 tx_highest;
} __packed;

struct ieee80211_sta_vht_cap {
        bool vht_supported;
        u32 cap; /* use IEEE80211_VHT_CAP_ */
        struct ieee80211_vht_mcs_info vht_mcs;
};

struct ieee80211_sta_s1g_cap {
        bool s1g;
        u8 cap[10]; /* use S1G_CAPAB_ */
        u8 nss_mcs[5];
};

struct ieee80211_supported_band {
        struct ieee80211_channel *channels;
        struct ieee80211_rate *bitrates;
        enum nl80211_band band;
        int n_channels;
        int n_bitrates;
        struct ieee80211_sta_ht_cap ht_cap;
        struct ieee80211_sta_vht_cap vht_cap;
        struct ieee80211_sta_s1g_cap s1g_cap;
        struct ieee80211_edmg edmg_cap;
        u16 n_iftype_data;
        const struct ieee80211_sband_iftype_data *iftype_data;
};

struct sk_buff_head {
        /* These two members must be first. */
        struct_group_tagged(sk_buff_list, list,
                struct sk_buff  *next;
                struct sk_buff  *prev;
        );

        __u32           qlen;
};

struct callback_head {
        struct callback_head *next;
        void (*func)(struct callback_head *head);
} __attribute__((aligned(sizeof(void *))));

struct cfg80211_cqm_config {
        struct rcu_head rcu_head;
        u32 rssi_hyst;
        s32 last_rssi_event_value;
        enum nl80211_cqm_rssi_threshold_event last_rssi_event_type;
        bool use_range_api;
        int n_rssi_thresholds;
        s32 rssi_thresholds[] __counted_by(n_rssi_thresholds);
};

struct cfg80211_ssid {
        u8 ssid[IEEE80211_MAX_SSID_LEN];
        u8 ssid_len;
};

struct cfg80211_scan_info {
        u64 scan_start_tsf;
        u8 tsf_bssid[ETH_ALEN] __aligned(2);
        bool aborted;
};

struct wiphy_work {
        struct list_head entry;
        wiphy_work_func_t func;
};

struct ieee80211_local {
	struct ieee80211_hw *ieee80211_hw;
	struct wiphy *wiphy;
	struct cfg80211_registered_device *rdev;
	struct rate_control_ref *rate_ctrl;
	struct ieee80211_emulated_hw hw;
	struct ieee80211_supported_band bands[NUM_NL80211_BANDS];

	/* for now use 3 interfaces: AP, STA and MONITOR */
	struct ieee80211_vif *ieee80211_vif;
	struct ieee80211_vif *ieee80211_ap_vif;
	struct ieee80211_vif *ieee80211_mon_vif;

	struct ieee80211_bss_conf *bss_conf;
	/* hashtables for memorizing the wlan stations in the network */
	unsigned long num_sta;

	struct list_head sta_list;
	struct rhltable sta_hash;
	struct rhltable link_sta_hash;
	struct list_head wiphy_work_list;
	struct tasklet_struct tasklet;

	/* include here the crda timeout variable instead of implementing it as a global one */
	u32 reg_crda_timeouts;

	u32 wep_iv;
	u8 dynamic_ps_forced_timeout;
	u32 tx_status_drop;
	u16 airtime_flags;
	u32 aql_txq_limit_low[IEEE80211_NUM_ACS];
	u32 aql_txq_limit_high[IEEE80211_NUM_ACS];
	u32 aql_threshold;
	unsigned int aql_total_pending_airtime;
	unsigned int aql_ac_pending_airtime[IEEE80211_NUM_ACS];
	unsigned int agg_queue_stop[IEEE80211_MAX_QUEUES];
	pthread_mutex_t iflist_mtx;
	pthread_mutex_t bss_lock;
	pthread_mutex_t wiphy_work_queue_mutex;

	/* protects active_txqs and txqi->schedule_order */
	pthread_mutex_t regulatory_req_mutex;
	pthread_mutex_t active_txq_lock[IEEE80211_NUM_ACS];
	struct list_head active_txqs[IEEE80211_NUM_ACS];
	u16 schedule_round[IEEE80211_NUM_ACS];
	struct ieee80211_ops *ieee80211_ops;
	struct sk_buff_head pending[IEEE80211_MAX_QUEUES];

	struct tasklet_struct tx_pending_tasklet;
	struct tasklet_struct wake_txqs_tasklet;

	int user_power_level; /* in dBm */
	int ap_power_level; /* in dBm */

	int open_count;
	int monitors;
	int virt_monitors;
	int tx_mntrs;
	/* number of interfaces with corresponding FIF_ flags */
	int fif_fcsfail;
	int fif_plcpfail;
	int fif_control;
	int fif_other_bss;
	int fif_pspoll;
	int fif_probe_req;
	bool probe_req_reg;
	bool rx_mcast_action_reg;
	unsigned int filter_flags; /* FIF_* */
	struct cfg80211_chan_def dflt_chandef;
	bool emulate_chanctx;
	bool tim_in_locked_section; /* see ieee80211_beacon_get() */

	struct ieee80211_chan_req monitor_chanreq;
	struct cfg80211_cqm_config cqm_config;
	/*
	 * suspended is true if we finished all the suspend _and_ we have
	 * not yet come up from resume. This is to be used by mac80211
	 * to ensure driver sanity during suspend and mac80211's own
	 * sanity. It can eventually be used for WoW as well.
	 */
	bool suspended;

	/* suspending is true during the whole suspend process */
	bool suspending;

	/*
	 * Resuming is true while suspended, but when we're reprogramming the
	 * hardware -- at that time it's allowed to use ieee80211_queue_work()
	 * again even though some other parts of the stack are still suspended
	 * and we still drop received frames to avoid waking the stack.
	 */
	bool resuming;

	/*
	 * quiescing is true during the suspend process _only_ to
	 * ease timer cancelling etc.
	 */
	bool quiescing;

	/* device is started */
	bool started;

	/* device is during a HW reconfig */
	bool in_reconfig;

	/* reconfiguration failed ... suppress some warnings etc. */
	bool reconfig_failure;

	/* wowlan is enabled -- don't reconfig on resume */
	bool wowlan;

	/* number of RX chains the hardware has */
	u8 rx_chains;

	/* bitmap of which sbands were copied */
	u8 sband_allocated;

	/* extended capabilities provided by mac80211 */
	u8 ext_capa[8];

	int tx_headroom; /* required headroom for hardware/radiotap */
	struct sk_buff_head skb_queue;
	struct sk_buff_head skb_queue_unreliable;

	/* see iface.c */
	struct list_head interfaces;
	struct list_head mon_list; /* only that are IFF_UP */
	struct list_head reg_requests_list;

	/* Scanning and BSS list */
	unsigned long scanning;
	struct cfg80211_ssid scan_ssid;
	struct cfg80211_scan_request *int_scan_req;
	struct cfg80211_scan_request __rcu *scan_req;
	struct ieee80211_scan_request *hw_scan_req;
	struct cfg80211_chan_def scan_chandef;
	enum nl80211_band hw_scan_band;
	int scan_channel_idx;
	int scan_ies_len;
	int hw_scan_ies_bufsize;
	struct cfg80211_scan_info scan_info;

	unsigned long queue_stop_reasons[IEEE80211_MAX_QUEUES];
	int q_stop_reasons[IEEE80211_MAX_QUEUES][IEEE80211_QUEUE_STOP_REASONS];

	/* Temporary remain-on-channel for off-channel operations */
	struct ieee80211_channel *tmp_channel;

	/* channel contexts */
	struct list_head chanctx_list;
	struct ieee80211_sub_if_data *sdata;

	/* workqueues */
	struct workqueue_struct *workqueue;
	struct work_struct restart_work;
	struct wiphy_work hw_roc_start;
	struct wiphy_work hw_roc_done;
	struct wiphy_work cqm_rssi_work;
	struct wiphy_work monitor_work;
	struct wiphy_work beacon_connection_loss_work;
	struct wiphy_work csa_connection_drop_work;
};

/* function prototypes */
void elf_initialize_driver(void);
void *get_director(void);
void *ieee80211_priv(void *hw);
void *get_ieee80211_priv_data(void);
/* vendor driver related function */
long rtw89_debug_priv_fw_log_manual_set(void *rtwdev, void *data, const char *buf, size_t count);

int main(void){
	int ret = 0;
	void *rtwdev = NULL;

	elf_initialize_driver();
	/* get rtwdev from the private data of ieee80211_local */
	rtwdev = get_ieee80211_priv_data();
	if(rtwdev == NULL){
		printf("[!] cannot find the ieee80211 driver private data!\n");
		return -1;
	}
	/* "Y" is used for starting the fw logs */
	ret = rtw89_debug_priv_fw_log_manual_set(rtwdev, NULL, "Y", sizeof("Y"));
	return ret;
}
