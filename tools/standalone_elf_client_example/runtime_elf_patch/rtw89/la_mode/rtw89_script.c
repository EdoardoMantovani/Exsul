/* Made by Edoardo Mantovani, 2026 */
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
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <asm-generic/param.h>

#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

#define s8  int8_t
#define s16 int16_t
#define s32 int32_t
#define s64 int64_t

#define __u8  u8
#define __u16 u16
#define __u32 u32
#define __u64 u64

#define __s8  s8
#define __s16 s16
#define __s32 s32
#define __s64 s64

#define be8  __be8
#define be16 __be16
#define be32 __be32
#define be64 __be64

#define IEEE80211_NUM_ACS		4
#define IEEE80211_MAX_QUEUES		16
#define MAX_FILE_IDX			32
#define IEEE80211_MAX_SSID_LEN		32
#define USB_MAX_ENDPOINT_NUM		30
#define MAX_KMEM_ARRAY_SIZE		36
#define MAX_USB_DEV_LIST		36
#define MAX_DEBUGFS_SIZE		128
#define MAX_USERSPACE_TO_DRIVER_IO_SIZE	256
#define MAX_DRIVER_TO_USERSPACE_IO_SIZE 256
#define IEEE80211_HT_MCS_MASK_LEN       10

#define atomic_bool bool

#define BITS_PER_BYTE           	8
#define BITS_PER_TYPE(type)     	(sizeof(type) * BITS_PER_BYTE)

#define DIV_ROUND_UP(n,d)       	(((n) + (d) - 1) / (d))
#define BITS_TO_LONGS(nr)       	__KERNEL_DIV_ROUND_UP(nr, BITS_PER_TYPE(long))

#define rcu_head			callback_head
#define __rcu
#define __packed			__attribute__((packed))
#define __aligned(x)			__attribute__((aligned(x)))

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

/* prototypes for rtw89 */
u8  rtw89_usb_ops_read8(void *rtwdev, u32 addr);
u16 rtw89_usb_ops_read16(void *rtwdev, u32 addr);
u32 rtw89_usb_ops_read32(void *rtwdev, u32 addr);

void rtw89_usb_ops_write8(void *rtwdev, u32 addr, u8 val);
void rtw89_usb_ops_write16(void *rtwdev, u32 addr, u16 val);
void rtw89_usb_ops_write32(void *rtwdev, u32 addr, u32 val);

/* rtl8852cu defines, enums && structs */

#define BIT0				1
#define LA_CHK_PTRN_NUM			4
#define	MASKDWORD			0xffffffff
#define R_AX_LA_CFG			0x9660
#define R_AX_LA_ERRFLAG			0x966C
#define BITS_AX_LA_CFG  		0xFFF000FF
#define B_AX_LA_FEN			BIT(0)
#define BITS_AX_LA_CFG 			0xFFF000FF
#define B_AX_LA_FEN			BIT(0)
#define B_AX_LA_RESTART_EN 		BIT(4)
#define B_AX_LA_TO_EN			BIT(5)
#define B_AX_LA_TO_VAL_SH		6
#define B_AX_LA_TO_VAL_MSK		0x3
#define B_AX_LA_TRIG_TU_SEL_SH		20
#define B_AX_LA_TRIG_TU_SEL_MSK		0xf
#define B_AX_LA_TRIG_TIME_VAL_MSK	0x7f
#define B_AX_LA_TRIG_TIME_VAL_SH	24
#define B_AX_LA_BUF_SEL_SH		16
#define B_AX_LA_BUF_SEL_MSK		0xf
#define B_AX_LA_BUF_BNDY_MSK		0x3f
#define B_AX_LA_BUF_BNDY_SH		8
#define LA_SIZE_128K_BUF_BNDY_8852B	8
#define LA_SIZE_256K_BUF_BNDY		0x18
#define LA_SIZE_192K_BUF_BNDY		0x20
#define REG_DUMP_HISTORY_NUM		20
#define RTW_MAC_LA_BUF_SEL_64K		0
#define RTW_MAC_LA_BUF_SEL_128K		1
#define RTW_MAC_LA_BUF_SEL_192K		2
#define RTW_MAC_LA_BUF_SEL_256K		3
#define DLE_BLOCK_SIZE			0x2000
#define DLE_BUF_BNDY_8852B		0x20000
#define LA_BUF_END_8852C_128K		0x50000
#define DLE_BUF_BNDY_8852A		0x70000

#define __AC(X,Y)			(X##Y)
#define _AC(X,Y)			__AC(X,Y)
#define _AT(T,X)			((T)(X))
#define _UL(x)				(_AC(x, UL))
#define _ULL(x)				(_AC(x, ULL))
#define UL(x)				(_UL(x))
#define ULL(x)				(_ULL(x))
#define BIT(nr)				(UL(1) << (nr))
#define SET_WORD(_v, _f)		(((_v) & (_f##_MSK)) << (_f##_SH))
#define SET_CLR_WORD(_w, _v, _f)	(((_w) & ~((_f##_MSK) << (_f##_SH))) | (((_v) & (_f##_MSK)) << (_f##_SH)))

/* set the support of this script for the rtl8852cu hardware, probably the support will be enabled for rtl8851bu and rtl8852bu too, need to search more for rtl8912 and 8922au */
#define MAC_AX_8852C_SUPPORT

/* smart trick: we do not know if the chip supports dbcc, so make bb->bb_phy_idx equal to 'HW_PHY_0' via a macro */
#define bb_phy_idx			HW_PHY_0

/*PHYx + Sx*/
enum phl_phy_idx {
	HW_PHY_0,
	HW_PHY_1,
	HW_PHY_MAX
};

enum channel_width {
	CHANNEL_WIDTH_20        	= 0,
	CHANNEL_WIDTH_40        	= 1,
	CHANNEL_WIDTH_80        	= 2,
	CHANNEL_WIDTH_160       	= 3,
	CHANNEL_WIDTH_320       	= 4,
	CHANNEL_WIDTH_80_80     	= 5,
	CHANNEL_WIDTH_5         	= 6,
	CHANNEL_WIDTH_10        	= 7,
	CHANNEL_WIDTH_MAX       	= 8,
};

enum bb_timer_cfg_t {
	BB_INIT_TIMER           	= 0,
	BB_CANCEL_TIMER         	= 1,
	BB_RELEASE_TIMER        	= 2,
	BB_SET_TIMER            	= 3
};

enum halbb_event_idx_t {
	/* timer */
	BB_EVENT_TIMER_DIG              = 0,
	BB_EVENT_TIMER_CFO              = 1,
	BB_EVENT_TIMER_ANTDIV           = 2,
	BB_EVENT_TIMER_TDMA_CR          = 3,
	BB_EVENT_TIMER_LA               = 4,
	BB_EVENT_TIMER_DTR              = 5
};

enum halbb_timer_state_t {
	/* timer */
	BB_TIMER_IDLE                   = 0,
	BB_TIMER_RUN                    = 1,
	BB_TIMER_RELEASE                = 0xff,
};

enum la_mac_src_t {
	LA_TRIG_SRC_T_MAC	= 0,
	LA_TRIG_SRC_P_MAC	= 1
};

enum la_run_mode_t {
	LA_RUN_HERITAGE		= 0,
	LA_RUN_FAST		= 1,
	LA_RUN_MANUAL		= 2,
	LA_RUN_RTL_TEST		= 3,
	LA_RUN_GET_MORE		= 4,
};

enum la_state_trig_t {
	LA_AND_DISABLE		= 0,
	LA_CCK_CCA		= 1,
	LA_OFDM_CCA		= 2,
	LA_OFDM_VBON		= 3,
	LA_RX_TD_STATE		= 4,
	LA_RX_STATE_FEQ		= 5,
	LA_NO_USE		= 6,
	LA_MUX_STATE		= 7,
	LA_PHYTXON		= 8,
	LA_BFMX_NDP_STANDBY	= 9,
	LA_BFMX_CSI_STANDBY	= 10,
	LA_CCA_SPOOF		= 11,
	LA_RXPKT_OK_MX		= 12,
	LA_RXPKT_FAIL_MX	= 13,
	LA_BRK			= 14,
	LA_BRK_SEL		= 15,
	LA_HE_TB_STANDBY	= 16,
};

enum la_hdr_sel_t {
	LA_HDR_ORI		= 0,
	LA_HDR_CCA		= 1,
	LA_HDR_CCA_OFDM		= 2,
	LA_HDR_CCA_CCK		= 3,
	LA_HDR_AGC_RDY		= 4,
	LA_HDR_AGC_RDY_HT_OR_AMPDU_MISS	= 5, /*AX: AGC_RDY_HT, BE: ampdu_miss*/
	LA_HDR_RXHT		= 6,
	LA_HDR_RXVHT		= 7,
	LA_HDR_RXHE_FULLBAND	= 8,
	LA_HDR_RXHE_OFDMA	= 9,
	LA_HDR_OFDM_VBON	= 10,
	LA_HDR_RXPKT_OK_SYNC	= 11,
	LA_HDR_RDRDY		= 12,
	LA_HDR_CRC_OK		= 13,
	LA_HDR_CRC_ERR		= 14,
	LA_HDR_RXEHT		= 15,
	LA_HDR_GRANT_BT_RX	= 16,
	LA_HDR_GRANT_BT_TX	= 17,
	LA_HDR_GRANT_WL		= 18,
	LA_HDR_PHYTXON		= 19,
	LA_HDR_IS_POP		= 20,
	LA_HDR_IS_SU		= 21,
	LA_HDR_IS_MU_MINO	= 22,
	LA_HDR_IS_BF		= 23,
	LA_HDR_IS_STBC		= 24,
	LA_HDR_IS_LDPC		= 25,
	LA_HDR_IS_AWGN		= 26,
	LA_HDR_IS_NDP		= 27,
	LA_HDR_PHY_BB_IDX_TYPE_A = 28,
	LA_HDR_PHY_BB_IDX_TYPE_B = 29,
	LA_HDR_PHY_BB_IDX_TYPE_C = 30,
	LA_HDR_PHY_BB_IDX_TYPE_D = 31
};

enum la_bb_trig_edge {
	LA_P_EDGE		= 0,
	LA_N_EDGE		= 1,
};

enum la_mac_polling_state {
	LA_HW_IDLE		= 0,
	LA_HW_START		= 1,
	LA_HW_FINISH_STOP	= 2,
	LA_HW_FINISH_TIMEOUT	= 3,
	LA_HW_RE_START		= 4
};

enum la_mode_state_t {
	LA_STATE_IDLE		= 0,
	LA_STATE_MAIN		= 1,
	LA_STATE_GET_DLE_BUF	= 2,
	LA_STATE_WAIT_RESTART	= 3
};

enum la_buff_mode_t {
	LA_BUFF_64K	= 0,
	LA_BUFF_128K	= 1,
	LA_BUFF_192K	= 2,
	LA_BUFF_256K	= 3,
	LA_BUFF_320K	= 4,
	LA_BUFF_DEFAULT,
	LA_BUF_DISABLE	= 0xff
};

enum la_bb_smp_clk {
	LA_SMP_CLK_80	= 0,
	LA_SMP_CLK_40	= 1,
	LA_SMP_CLK_20	= 2,
	LA_SMP_CLK_10	= 3,
	LA_SMP_CLK_5	= 4,
	LA_SMP_CLK_2_5	= 5,
	LA_SMP_CLK_1_25	= 6,
	LA_SMP_CLK_160	= 7,
	LA_SMP_CLK_320	= 8,
	LA_SMP_DEFAULT,
	LA_SMP_MAX
};

enum la_dma_data_type_t {
	DMA00_NRML_1s_14b	= 0,
	DMA01_NRML_2s_12b	= 1, /*Dbgport 16-bit: dbg[N, N-15]*/
	DMA02_NRML_2s_13b	= 2, /*Dbgport 12-bit: dbg[N, N-11]*/
	DMA03_NRML_2s_14b	= 3, /*Dbgport 08-bit: dbg[N, N-07]*/
	DMA04_NRML_3s_08b	= 4,
	DMA05_NRML_3s_09b	= 5,
	DMA06_NRML_3s_10b	= 6,
	DMA07_NRML_4s_07b	= 7,
	DMA08_NRML_4s_08b	= 8,
	DMA09_DUAL_4s_12b	= 9,
	DMA10_DUAL_4s_13b	= 10,
	DMA11_DUAL_4s_14b	= 11,
	DMA12_MPHS_1s_2p_12b	= 12,
	DMA13_MPHS_1s_3p_10b	= 13,
	DMA14_MPHS_1s_4p_08b	= 14,
	DMA15_MPHS_2s_2p_08b	= 15,
	DMA16_DBG_BB_MNTR	= 16,
	DMA17_DUAL_WB_1s_14b	= 17,
	DMA18_DUAL_WB_2s_14b	= 18,
	DMA19_DUAL_WB_3s_14b	= 19,
	DMA20_DUAL_WB_4s_14b 	= 20,
	DMA30_DBG_LA_SEL	= 30
};

enum la_trig_sign_t {
	LA_UNSIGNED		= 0,
	LA_SIGNED		= 1,
	LA_NORM			= 2,
};

enum la_input_src_sel_t {
	LA_SRC_DCCL_OUT	= 0,
	LA_SRC_IQK	= 1,
	LA_SRC_DFIR	= 2,
	LA_SRC_ADC	= 3,
	LA_SRC_TX_DAC	= 4,
	LA_SRC_WB_ADC	= 5,
	LA_SRC_SAR_ADC	= 6
};

/*@--------------------------[Structure]-------------------------------------*/

struct mac_ax_la_cfg {
	u32 la_func_en:1;
	u32 la_restart_en:1;
	u32 la_timeout_en:1;
	/* 2'h0: 1s, 2'h1: 2s, 2'h2: 4s, 2'h3: 8s */
	u32 la_timeout_val:2;
	/* Error flag mask bit for LA data loss due to pktbuffer busy */
	u32 la_data_loss_imr:1;
	/* TU (time unit) = 2^ B_AX_LA_TRIG_TU_SEL */
	u32 la_tgr_tu_sel:4;
	/* 6'h0: No delay, 6'h1: 1 TU, 6'h2: 2TU, ??*/
	u32 la_tgr_time_val:7;
	u32 rsvd:15;
};

struct mac_ax_la_buf_param {
	u32 start_addr;
	u32 end_addr;
	u8 la_buf_sel; /*0: 64KB; 1: 128KB; 2: 192KB; 3: 256KB; 4: 320KB*/
};

struct la_ptrn_chk_info {
	u32	smp_point;
	u32	la_ptrn_chk_mask; /*if mask=0: disable pattern chk, for MSB 32bit only*/
	u32	la_ptrn_chk_val;
};

struct la_print_info {
	bool	is_la_print;
	u8	print_mode; /*0: hex, 1:unsign, 2:sign*/
	u8	print_lsb;
	u8	print_msb;
	u8	print_buff_opt; /*print to 0:Debug Log, 1: CNSL Buff*/
};

struct la_dma_info {
	u8 dma_dbgport_base_n;
	u8 dma_a_path_sel;
	u8 dma_b_path_sel;
	u8 dma_c_path_sel;
	u8 dma_d_path_sel;
	enum la_input_src_sel_t dma_a_src_sel;
	enum la_input_src_sel_t dma_b_src_sel;
	enum la_input_src_sel_t dma_c_src_sel;
	enum la_input_src_sel_t dma_d_src_sel;
	enum la_hdr_sel_t dma_hdr_sel_63;
	enum la_hdr_sel_t dma_hdr_sel_62;
	enum la_hdr_sel_t dma_hdr_sel_61;
	enum la_hdr_sel_t dma_hdr_sel_60;
	bool dma_a_ck160_dly_en;
	bool dma_b_ck160_dly_en;
	bool dma_c_ck160_dly_en;
	bool dma_d_ck160_dly_en;
	enum phl_phy_idx dma_dbcc_phy_sel;
	enum la_dma_data_type_t dma_data_type;
	u8 dma_dbgport_ext_base_n;
	enum la_hdr_sel_t dma_hdr_sel_59;
	enum la_hdr_sel_t dma_hdr_sel_58;
	enum la_hdr_sel_t dma_hdr_sel_57;
	enum la_hdr_sel_t dma_hdr_sel_56;
};

struct la_string_info {
	u32			*octet;
	u32			length;
	u32			buffer_size;	/*Byte*/
	u32			start_pos;
	u32			end_pos;	/*buf addr*/
	u32			smp_number_max; /*number of LA sample*/
};

struct la_re_trig_info {
	bool			re_trig_en;
	u16			re_trig_wait_cnt;
	/*Re-trig*/
	bool			la_re_trig_edge;
	u8			la_re_and0_sel;
	u8			la_re_and0_val;
	bool			la_re_and0_inv;
	u16			la_re_and0_mask;
};

struct la_adv_trig_info { /*AND0~AND7*/
	bool			adv_trig_en; /*SW ctrl value*/
	/*AND1*/
	u32			la_and1_mask; /*sel all 0 = disable*/
	u32			la_and1_val;
	bool			la_and1_inv;
	/*AND2*/
	bool			la_and2_en;
	bool			la_and2_inv;
	u32			la_and2_val;
	u32			la_and2_mask;
	enum la_trig_sign_t	la_and2_sign; /*0: unsigned, 1:signed, 2:norm*/
	/*AND3*/
	bool			la_and3_en;
	bool			la_and3_inv;
	u32			la_and3_val;
	u32			la_and3_mask;
	enum la_trig_sign_t	la_and3_sign; /*0: unsigned, 1:signed, 2:norm*/
	/*AND4*/
	bool			la_and4_en;
	u16			la_and4_rate; /*rate_idx*/
	bool			la_and4_inv;
	/*AND5*/
	enum la_state_trig_t	la_and5_sel;
	bool			la_and5_inv;
	u8			la_and5_val;
	/*AND6*/
	enum la_state_trig_t	la_and6_sel;
	bool			la_and6_inv;
	u8			la_and6_val;
	/*AND7*/
	enum la_state_trig_t	la_and7_sel;
	bool			la_and7_inv;
	u8			la_and7_val;
	bool			la_and0_inv;
	u16			la_and5_mask;
	u16			la_and6_mask;
	u16			la_and7_mask;
	/*AND8*/
	u8			la_and8_base_n;
	bool			la_and8_inv;
	u32			la_and8_mask;
	u32			la_and8_val;
};

struct la_trig_mac_info {
	bool	la_mac_trig_en;  /*sw tag*/
	bool	la_mac_and0_en;
	u8	la_mac_and0_sel; /*0:cca, 1:1st_crc_OK, 2:1st_crc_err, 3:OK, 4:err*/
	u8	la_mac_and0_tmac_pmac_sel; /*0: true mac, 1: pmac*/
	bool	la_mac_and1_en;
	u8	la_mac_and1_addr; /*LSB 8 Bits of target MAC ADDRESS*/
	bool	la_mac_and2_en;
	u8	la_mac_and2_frame_sel; /*6-bit mac hdr*/
	u8	la_mac_uid;
	u8	la_mac_crc_pmac_pkt_sel; /*1: CCK 2: Legacy 3: HT 4: VHT 5: HE 6: EHT 7: A-MPDU default: all type crc*/
};

struct la_mac_cfg_info {
	enum la_buff_mode_t mac_la_buf_sel;
	bool mac_alloc_success;
	u32 la_trigger_time; /*mu sec*/
	u8 mac_la_en;
	u8 mac_la_restart_en;
	u8 mac_la_timeout_en;
	u8 mac_la_data_loss_imr;/*Error flag mask bit for LA data loss due to pktbuffer busy */
	u8 mac_la_timeout_val;	/*0:1s, 1:2s, 2:4s, 3:8s*/
};

struct bb_la_cr_info {
	u32 la_clk_en;
	u32 la_clk_en_m;
	u32 la_en;
	u32 la_en_m;
	u32 dma_dbgport_base_n;
	u32 dma_dbgport_base_n_m;
	u32 dma_a_path_sel;
	u32 dma_a_path_sel_m;
	u32 dma_b_path_sel;
	u32 dma_b_path_sel_m;
	u32 dma_c_path_sel;
	u32 dma_c_path_sel_m;
	u32 dma_d_path_sel;
	u32 dma_d_path_sel_m;
	u32 dma_a_src_sel;
	u32 dma_a_src_sel_m;
	u32 dma_b_src_sel;
	u32 dma_b_src_sel_m;
	u32 dma_c_src_sel;
	u32 dma_c_src_sel_m;
	u32 dma_d_src_sel;
	u32 dma_d_src_sel_m;
	u32 la_smp_rt_sel;
	u32 la_smp_rt_sel_m;
	u32 rdrdy_3_phase_en;
	u32 rdrdy_3_phase_en_m;
	u32 la_trigger_edge;
	u32 la_trigger_edge_m;
	u32 dma_hdr_sel_63;
	u32 dma_hdr_sel_63_m;
	u32 dma_hdr_sel_62;
	u32 dma_hdr_sel_62_m;
	u32 dma_hdr_sel_61;
	u32 dma_hdr_sel_61_m;
	u32 dma_hdr_sel_60;
	u32 dma_hdr_sel_60_m;
	u32 dma_a_ck160_dly_en;
	u32 dma_a_ck160_dly_en_m;
	u32 dma_b_ck160_dly_en;
	u32 dma_b_ck160_dly_en_m;
	u32 dma_dbgport_phy_sel;
	u32 dma_dbgport_phy_sel_m;
	u32 dma_la_phy_sel;
	u32 dma_la_phy_sel_m;
	u32 dma_data_type;
	u32 dma_data_type_m;
	u32 r_dma_rdrdy;
	u32 r_dma_rdrdy_m;
	u32 la_and0_bit_sel;
	u32 la_and0_bit_sel_m;
	u32 la_trigger_cnt;
	u32 la_trigger_cnt_m;
	u32 and0_trig_disable;
	u32 and0_trig_disable_m;
	u32 la_and1_inv;
	u32 la_and1_inv_m;
	u32 la_and2_en;
	u32 la_and2_en_m;
	u32 la_and2_inv;
	u32 la_and2_inv_m;
	u32 la_and3_en;
	u32 la_and3_en_m;
	u32 la_and3_inv;
	u32 la_and3_inv_m;
	u32 la_and4_en;
	u32 la_and4_en_m;
	u32 la_and4_rate;
	u32 la_and4_rate_m;
	u32 la_and4_inv;
	u32 la_and4_inv_m;
	u32 la_and1_mask;
	u32 la_and1_mask_m;
	u32 la_and1_val;
	u32 la_and1_val_m;
	u32 la_and2_mask;
	u32 la_and2_mask_m;
	u32 la_and2_val;
	u32 la_and2_val_m;
	u32 la_and3_mask;
	u32 la_and3_mask_m;
	u32 la_and3_val;
	u32 la_and3_val_m;
	u32 la_and5_sel;
	u32 la_and5_sel_m;
	u32 la_and5_val;
	u32 la_and5_val_m;
	u32 la_and5_inv;
	u32 la_and5_inv_m;
	u32 la_and6_sel;
	u32 la_and6_sel_m;
	u32 la_and6_val;
	u32 la_and6_val_m;
	u32 la_and6_inv;
	u32 la_and6_inv_m;
	u32 la_and7_sel;
	u32 la_and7_sel_m;
	u32 la_and7_val;
	u32 la_and7_val_m;
	u32 la_and7_inv;
	u32 la_and7_inv_m;
	u32 la_brk_sel;
	u32 la_brk_sel_m;
	u32 la_mac_and1_en;
	u32 la_mac_and1_en_m;
	u32 la_mac_and2_en;
	u32 la_mac_and2_en_m;
	u32 la_target_frame_type_en;
	u32 la_target_frame_type_en_m;
	u32 la_mac_addr_en;
	u32 la_mac_addr_en_m;
	u32 la_mac_addr;
	u32 la_mac_addr_m;
	u32 la_mac_multi_user_uid;
	u32 la_mac_multi_user_uid_m;
	u32 la_mac_and2_frame_sel;
	u32 la_mac_and2_frame_sel_m;
	u32 la_mac_and0_sel;
	u32 la_mac_and0_sel_m;
	u32 la_mac_and0_en;
	u32 la_mac_and0_en_m;
	u32 la_mac_and0_mac_sel;
	u32 la_mac_and0_mac_sel_m;
	u32 la_mac_and0_crc_src_sel;
	u32 la_mac_and0_crc_src_sel_m;
	u32 la_and2_sign;
	u32 la_and2_sign_m;
	u32 la_and3_sign;
	u32 la_and3_sign_m;
	u32 la_re_trig_edge;
	u32 la_re_trig_edge_m;
	u32 la_re_and1_sel;
	u32 la_re_and1_sel_m;
	u32 la_re_and1_val;
	u32 la_re_and1_val_m;
	u32 la_re_and1_inv;
	u32 la_re_and1_inv_m;
	u32 la_adc_320up;
	u32 la_adc_320up_m;
	u32 la_scope_mode_en;
	u32 la_scope_mode_en_m;
	u32 la_scope_mode_auto_fix_la;
	u32 la_scope_mode_auto_fix_la_m;
	u32 la_dbg_port_ip_ext;
	u32 la_dbg_port_ip_ext_m;
	u32 la_dbg_port_ext;
	u32 la_dbg_port_ext_m;
	u32 dma_dbgport_ext_base_n;
	u32 dma_dbgport_ext_base_n_m;
	u32 dma_hdr_sel_59;
	u32 dma_hdr_sel_59_m;
	u32 dma_hdr_sel_58;
	u32 dma_hdr_sel_58_m;
	u32 dma_hdr_sel_57;
	u32 dma_hdr_sel_57_m;
	u32 dma_hdr_sel_56;
	u32 dma_hdr_sel_56_m;
	u32 la_and0_inv;
	u32 la_and0_inv_m;
	u32 la_and5_mask;
	u32 la_and5_mask_m;
	u32 la_and6_mask;
	u32 la_and6_mask_m;
	u32 la_and7_mask;
	u32 la_and7_mask_m;
	u32 la_and8_base_n;
	u32 la_and8_base_n_m;
	u32 la_and8_inv;
	u32 la_and8_inv_m;
	u32 la_and8_mask;
	u32 la_and8_mask_m;
	u32 la_and8_val;
	u32 la_and8_val_m;
	u32 la_re_and1_mask;
	u32 la_re_and1_mask_m;
	u32 la_mac_crc_pmac_pkt_sel;
	u32 la_mac_crc_pmac_pkt_sel_m;
};

struct halbb_timer_info {
	u32     cb_time; /* callback time (ms) */
	enum halbb_event_idx_t  event_idx;
	enum halbb_timer_state_t timer_state;
};

struct bb_la_mode_info {
	struct bb_la_cr_info	bb_la_cr_i;
	struct la_string_info	la_string_i;
	enum la_mode_state_t	la_mode_state;
	u32			la_dbg_port; /* [31:16]:DBG_IP, [15:0]:DBG_PORT */
	u32			la_count; /* curr value */
	u32			la_count_max;
	u32			smp_number;
	u32			txff_page;
	bool			not_stop_trig; /* set impossible trigger condition */
	u8			la_basic_mode_sel; /* 0:bb_mode, mac mode */
	/* [General setting] */
	u8			la_polling_cnt;
	u8			la_trigger_cnt;
	enum la_bb_trig_edge	la_trigger_edge;
	enum la_bb_smp_clk	la_smp_rate; /* CR Setting */
	u16			la_smp_rate_log; /* 20/40/80/160/320M, for debug log only */
	bool			la_1115_320up_clk_en;
	/* [AND-0 sel] */
	bool			la_and0_disable;
	u32			la_and0_bit_sel; /* And0 trigger bit sel */
	struct la_dma_info	la_dma_i;
	struct la_re_trig_info	la_re_trig_i;
	struct la_adv_trig_info	adv_trig_i;
	struct la_trig_mac_info	la_trig_mac_i;
	struct la_mac_cfg_info	la_mac_cfg_i; /* MAC CR Control */
	struct la_print_info	la_print_i;
	bool			la_ptrn_chk_en;
	struct halbb_timer_info la_timer_i;
	struct la_ptrn_chk_info la_ptrn_chk_i[LA_CHK_PTRN_NUM];
	enum la_run_mode_t 	la_run_mode;
	u32			la_dbg_port_ext; /* [31:16]:DBG_IP, [15:0]:DBG_PORT */
};

u32 halbb_cal_bit_shift(u32 bit_mask){
	int ret = 0;
	u32 i = 0;

	if (bit_mask == 0){
		return ret;
	}
	for (i = 0; i <= 31; i++) {
		if ((bit_mask >> i) & BIT0){
			break;
		}
	}
        return i;
}

void halbb_la_bb_set_smp_rate(struct bb_la_mode_info *la_mode_info, u8 fix_mode_en, enum la_bb_smp_clk la_smp_rate_in){
	bool ck160_dly_en = false;
	struct la_dma_info *dma = &la_mode_info->la_dma_i;
	enum channel_width bw = CHANNEL_WIDTH_80;
	enum la_bb_smp_clk smp_rate_tmp = la_smp_rate_in;

	if (fix_mode_en == false) {
		if (la_smp_rate_in != LA_SMP_DEFAULT) {
			return;
		}

		if (bw >= CHANNEL_WIDTH_160){
			smp_rate_tmp = LA_SMP_CLK_160;
		}else if (bw >= CHANNEL_WIDTH_20){
			smp_rate_tmp = LA_SMP_CLK_20 - bw;
		}else{
			smp_rate_tmp = LA_SMP_CLK_20;
		}
        }

	ck160_dly_en = (smp_rate_tmp == LA_SMP_CLK_160) ? 1 : 0;
	dma->dma_a_ck160_dly_en = ck160_dly_en;
	dma->dma_b_ck160_dly_en = ck160_dly_en;
	dma->dma_c_ck160_dly_en = ck160_dly_en;
	dma->dma_d_ck160_dly_en = ck160_dly_en;

	la_mode_info->la_smp_rate = smp_rate_tmp;
	la_mode_info->la_smp_rate_log = (smp_rate_tmp == LA_SMP_CLK_160) ? 160 : (80 >> smp_rate_tmp);
	printf("[*] smp_rate_tmp=%d, la_smp_rate_log=%d M\n", smp_rate_tmp, la_mode_info->la_smp_rate_log);
}

void halbb_la_bb_set_cmn_reset(struct bb_la_mode_info *la_mode_info){
	u32 trig_time_cca = 0;
	enum channel_width bw = CHANNEL_WIDTH_80;

	/* Trig Time */
	if (bw <= CHANNEL_WIDTH_160) {
		trig_time_cca = ((la_mode_info->la_string_i.smp_number_max >> (bw + 1)) / 10) - (2 << (u32)(CHANNEL_WIDTH_160 - bw)) - (u32)(CHANNEL_WIDTH_160 - bw);
		printf("[*] bw=%d, default trig_time_cca =%d\n", 20 << bw, trig_time_cca);

		la_mode_info->la_mac_cfg_i.la_trigger_time = trig_time_cca - 10;
	}else{
		la_mode_info->la_mac_cfg_i.la_trigger_time = 195;
	}

	la_mode_info->la_trigger_cnt = 0;
	la_mode_info->la_trigger_edge = LA_P_EDGE;
	halbb_la_bb_set_smp_rate(la_mode_info, false, LA_SMP_DEFAULT);
	la_mode_info->la_polling_cnt = 20;
	la_mode_info->la_and0_disable = true;
	la_mode_info->la_1115_320up_clk_en = false;
}

void halbb_la_bb_set_re_trig_reset(struct bb_la_mode_info *la_mode_info){
	struct la_re_trig_info *re_trig = &la_mode_info->la_re_trig_i;

	memset(re_trig, 0, sizeof(struct la_re_trig_info));
}

void halbb_la_bb_set_dma_type_reset(struct bb_la_mode_info *la_mode_info){
	struct la_dma_info      *dma = &la_mode_info->la_dma_i;

	dma->dma_dbgport_base_n = 31;
	dma->dma_a_path_sel = 0;
	dma->dma_b_path_sel = 1;
	dma->dma_c_path_sel = 2;
	dma->dma_d_path_sel = 3;
	dma->dma_a_src_sel = LA_SRC_ADC;
	dma->dma_b_src_sel = LA_SRC_ADC;
	dma->dma_c_src_sel = LA_SRC_ADC;
	dma->dma_d_src_sel = LA_SRC_ADC;
	dma->dma_dbcc_phy_sel = 0;
	dma->dma_hdr_sel_63 = LA_HDR_ORI;
	dma->dma_hdr_sel_62 = LA_HDR_ORI;
	dma->dma_hdr_sel_61 = LA_HDR_ORI;
	dma->dma_hdr_sel_60 = LA_HDR_ORI;
	dma->dma_data_type = DMA01_NRML_2s_12b;
}

void halbb_la_bb_set_adv_reset(struct bb_la_mode_info *la_mode_info){
	struct la_adv_trig_info *adv = &la_mode_info->adv_trig_i;

	memset(adv, 0, sizeof(struct la_adv_trig_info));
}

void halbb_la_mac_set_adv_reset(struct bb_la_mode_info *la_mode_info){
	struct la_trig_mac_info *trig_mac = &la_mode_info->la_trig_mac_i;

	memset(trig_mac, 0, sizeof(struct la_trig_mac_info));
}

void halbb_la_set_mac_trig_time(u32 trig_time, u8 *unit, u8 *unit_num){
	u8 time_unit_num = 0;
	u32 ref_time = 128;

        /*mac_la_tgr_tu_sel:   0 ~ 2^[0~15] */
        /*mac_la_tgr_time_val: 0 ~ 2^7      */

	if (trig_time > 0x400000){
		trig_time = 0x400000;
	}
	for (u8 i = 0; i < 16; i++) {
		if (trig_time <= (ref_time << i)) {
			*unit = i;
			break;
		}
	}

	*unit_num = (u8)(trig_time >> *unit);
	printf("[*] 2. [Set Trig-Time] Time=%d * unit=2^%d us\n", unit_num[0], unit[0]);
}

u32 mac_lamode_cfg(void *rtwdev, struct mac_ax_la_cfg *cfg){
	int ret = 0;
	u32 val32 = 0;

	val32 = rtw89_usb_ops_read32(rtwdev, R_AX_LA_CFG);
	val32 &= ~BITS_AX_LA_CFG;
	val32 |= ((cfg->la_func_en ? B_AX_LA_FEN : 0) |
		  (cfg->la_restart_en ? B_AX_LA_RESTART_EN : 0) |
		  (cfg->la_timeout_en ? B_AX_LA_TO_EN : 0) |
		  SET_WORD(cfg->la_timeout_val, B_AX_LA_TO_VAL) |
		  SET_WORD(cfg->la_tgr_tu_sel, B_AX_LA_TRIG_TU_SEL) |
		  SET_WORD(cfg->la_tgr_time_val, B_AX_LA_TRIG_TIME_VAL));
	rtw89_usb_ops_write32(rtwdev, R_AX_LA_CFG, val32);

	if (cfg->la_data_loss_imr){
		rtw89_usb_ops_write8(rtwdev, R_AX_LA_ERRFLAG, BIT0);
	}
	return ret;
}

int rtw_hal_mac_lamode_cfg(void *rtwdev, u8 func_en, u8 restart_en, u8 timeout_en, u8 timeout_val, u8 data_loss_imr, u8 la_tgr_tu_sel, u8 tgr_time_val){
	int ret = 0;
	struct mac_ax_la_cfg cfg = {0};

	cfg.la_func_en = func_en;
	cfg.la_restart_en = restart_en;
	cfg.la_timeout_en = timeout_en;
	cfg.la_timeout_val = timeout_val;
	cfg.la_data_loss_imr = data_loss_imr;
	cfg.la_tgr_tu_sel = la_tgr_tu_sel;
	cfg.la_tgr_time_val = tgr_time_val;

	mac_lamode_cfg(rtwdev, &cfg);

	return ret;
}

void halbb_la_mac_cfg_cmn(void *rtwdev, struct bb_la_mode_info *la_mode_info){
	u8 t_unit = 0;
	u8 t_unit_num = 0;
	struct la_mac_cfg_info  *cfg = &la_mode_info->la_mac_cfg_i;
	cfg->mac_la_en = (cfg->mac_la_buf_sel == LA_BUF_DISABLE) ? 0 : 1;

	halbb_la_set_mac_trig_time(cfg->la_trigger_time, &t_unit, &t_unit_num);
	rtw_hal_mac_lamode_cfg(rtwdev, cfg->mac_la_en,
		cfg->mac_la_restart_en,
		cfg->mac_la_timeout_en,
		cfg->mac_la_timeout_val,
		cfg->mac_la_data_loss_imr,
		t_unit,
		t_unit_num
	);
}

void halbb_la_mac_init(void *rtwdev, struct bb_la_mode_info *la_mode_info){
	struct la_mac_cfg_info *cfg = &la_mode_info->la_mac_cfg_i;

	halbb_la_mac_cfg_cmn(rtwdev, la_mode_info);
}

/* note that in this function it is necessary to select one specific chip by enabling macros like 'MAC_AX_8852B_SUPPORT' or 'MAC_AX_8852C_SUPPORT'.. */
u32 mac_lamode_buf_cfg(void *rtwdev, struct mac_ax_la_buf_param *param){
	int ret = 0;
	u32 val32 = 0;

	val32 = rtw89_usb_ops_read32(rtwdev, R_AX_LA_CFG);
	val32 = SET_CLR_WORD(val32, param->la_buf_sel, B_AX_LA_BUF_SEL);

	if (param->la_buf_sel == RTW_MAC_LA_BUF_SEL_256K) { /* la buf 256K */
		val32 = SET_CLR_WORD(val32, LA_SIZE_256K_BUF_BNDY, B_AX_LA_BUF_BNDY);
		param->start_addr = LA_SIZE_256K_BUF_BNDY * DLE_BLOCK_SIZE;
		param->end_addr = DLE_BUF_BNDY_8852A;
	} else if (param->la_buf_sel == RTW_MAC_LA_BUF_SEL_192K) { /* la buf 192K */
		val32 = SET_CLR_WORD(val32, LA_SIZE_192K_BUF_BNDY, B_AX_LA_BUF_BNDY);
		param->start_addr = LA_SIZE_192K_BUF_BNDY * DLE_BLOCK_SIZE;
		param->end_addr = DLE_BUF_BNDY_8852A;
	} else if (param->la_buf_sel == RTW_MAC_LA_BUF_SEL_128K) { /* la buf 128K */
	#ifdef MAC_AX_8852B_SUPPORT /*temp setting, will move to per IC files later*/
			val32 = SET_CLR_WORD(val32, LA_SIZE_128K_BUF_BNDY_8852B, B_AX_LA_BUF_BNDY);
			param->start_addr = LA_SIZE_128K_BUF_BNDY_8852B * DLE_BLOCK_SIZE;
			param->end_addr = DLE_BUF_BNDY_8852B;
	#endif
	#ifdef MAC_AX_8852C_SUPPORT
			val32 = SET_CLR_WORD(val32, LA_SIZE_256K_BUF_BNDY, B_AX_LA_BUF_BNDY);
			param->start_addr = LA_SIZE_256K_BUF_BNDY * DLE_BLOCK_SIZE;
			param->end_addr = LA_BUF_END_8852C_128K;
	#endif
	#ifdef MAC_AX_8851B_SUPPORT /*temp setting, will move to per IC files later*/
			val32 = SET_CLR_WORD(val32, LA_SIZE_128K_BUF_BNDY_8851B, B_AX_LA_BUF_BNDY);
			param->start_addr = LA_SIZE_128K_BUF_BNDY_8851B * DLE_BLOCK_SIZE;
			param->end_addr = DLE_BUF_BNDY_8851B;
	#endif
	}else if (param->la_buf_sel == RTW_MAC_LA_BUF_SEL_64K) { /* la buf 64K */
		val32 |= SET_WORD(LA_SIZE_128K_BUF_BNDY_8852B, B_AX_LA_BUF_BNDY);
		param->start_addr = LA_SIZE_128K_BUF_BNDY_8852B * DLE_BLOCK_SIZE;
		param->end_addr = DLE_BUF_BNDY_8852B;
	}

	rtw89_usb_ops_write32(rtwdev, R_AX_LA_CFG, val32);
	return ret;
}

int rtw_hal_mac_lamode_cfg_buf(void *rtwdev, u8 buf_sel, u32 *addr_start, u32 *addr_end){
	int ret = 0;
	struct mac_ax_la_buf_param param = {0};

	param.la_buf_sel = buf_sel;

	mac_lamode_buf_cfg(rtwdev, &param);

	*addr_start = param.start_addr;
	*addr_end = param.end_addr;

	return ret;
}

bool halbb_la_mac_cfg_buf(void *rtwdev, struct bb_la_mode_info *la_mode_info, enum la_buff_mode_t mode){
	u32 addr_start = 0;
	u32 addr_end = 0;
	u32 buf_size_tmp = 0;
	struct la_mac_cfg_info	*cfg = &la_mode_info->la_mac_cfg_i;
	struct la_string_info *buf = &la_mode_info->la_string_i;
	u8 buf_sel = 0;
	u32 status = 0;

	/* for now not enable this code, seems that it needs the support for rtl8912/8922 */
	#if 0
	if (bb->bb_80211spec == BB_BE_IC && mode == LA_BUFF_DEFAULT) {
		status = rtw_hal_mac_lamode_query_buf (bb->hal_com, &buf_sel);
		printf("[%s] buf_sel : %d\n", __FUNCTION__, buf_sel);
		if (status != 0) { // 0 : MACSUCCESS
			printf("[!] LA query buf fail! status : %d\n", status);
			return false;
		}

	}
	#endif

	if (mode == LA_BUFF_256K || mode == LA_BUFF_DEFAULT){
		if (mode == LA_BUFF_DEFAULT){
			la_mode_info->la_mac_cfg_i.mac_la_buf_sel = LA_BUFF_256K;
		}
		buf->buffer_size = 0x40000; /*2^18=(2^8)*(2^10)=256K Byte*/
	}else if (mode == LA_BUFF_128K){
		buf->buffer_size = 0x20000; /*2^17=(2^7)*(2^10)=128K Byte*/
	}else{
		buf->buffer_size = 0;
	}

	if (buf->buffer_size == 0) {
		return false;
	}
	rtw_hal_mac_lamode_cfg_buf(rtwdev, cfg->mac_la_buf_sel, &addr_start, &addr_end);

	buf->start_pos = addr_start;
	buf->end_pos = addr_end;
	buf_size_tmp = buf->end_pos - buf->start_pos;

	if (buf_size_tmp != buf->buffer_size) {
		printf("[*] buf_size_tmp=0x%x, buffer_size=0x%x\n", buf_size_tmp, buf->buffer_size);
		return false;
	}
	buf->smp_number_max = buf->buffer_size >> 3;

	printf("[*] addr=[0x%x ~ 0x%x], buf_size=(%d K), smp_num_max=(%d)\n", buf->start_pos, buf->end_pos, (buf->buffer_size >> 10),  buf->smp_number_max);
	return true;
}

void halbb_la_cr_dump(struct bb_la_mode_info *la_mode_info){
	bool brk_compare_match = false;
	u8 print_idx = 0;
	u8 round_up_time = 0;
	u8 reg_history_idx = 0;
	u32 len = 0;
	u32 val_tmp = 0;
	u32 reg_val = 0;
	u32 val_mask_tmp = 0;
	u32 reg_history[REG_DUMP_HISTORY_NUM] = {0};
	u32 *str_table = NULL;
	struct bb_la_cr_info *cr = NULL;

	cr = &la_mode_info->bb_la_cr_i;
	len = (sizeof(struct bb_la_cr_info) >> 2);
	str_table = (u32 *)cr;

	for (int i = 0; i < len; i++) {
		brk_compare_match = false;
		val_tmp = str_table[i];

		if (val_tmp > 10000 || val_tmp == 0) {
			continue;
		}

		if (!((val_tmp & 0xf) == 0 || (val_tmp & 0xf) == 4 || (val_tmp & 0xf) == 8 || (val_tmp & 0xf) == 0xc)) {
			continue;
		}
		val_mask_tmp = val_tmp >> halbb_cal_bit_shift(val_tmp);

		if (val_mask_tmp == 0x1 || val_mask_tmp == 0x3 || val_mask_tmp == 0x7 || val_mask_tmp == 0xf || val_mask_tmp == 0x1f || val_mask_tmp == 0x3f || val_mask_tmp == 0x7f || val_mask_tmp == 0xff) {
			continue;
		}

		for (int j = 0; j < REG_DUMP_HISTORY_NUM; j++) {
			if (val_tmp == reg_history[j]) {
				brk_compare_match = true;
				break;
			}
 		}

		if (brk_compare_match){
			continue;
		}
		reg_history[reg_history_idx] = val_tmp;
		reg_history_idx++;

		if (reg_history_idx >= REG_DUMP_HISTORY_NUM) {
			round_up_time++;
			for (int j = 0; j < REG_DUMP_HISTORY_NUM; j++) {
				reg_val = halbb_get_reg_cmn(reg_history[j], MASKDWORD, bb_phy_idx);
				print_idx++;
				printf("[%03d] Reg 0x%04x = 0x%08x\n", print_idx, reg_history[j], reg_val);
			}
			reg_history_idx = 0;
		}
	}

	if (reg_history_idx != 0) {
		for (int j = 0; j < reg_history_idx; j++) {
			reg_val = halbb_get_reg_cmn(reg_history[j], MASKDWORD, bb_phy_idx);
			print_idx++;
			printf("[%03d] Reg 0x%04x = 0x%08x\n", print_idx, reg_history[j], reg_val);
		}
	}

	printf("[*] round_up_time = %d, print_idx=%d\n", round_up_time, print_idx);
}

void halbb_la_stop(struct bb_la_mode_info *la_mode_info){
	la_mode_info->la_mode_state = LA_STATE_IDLE;
}

void halbb_la_main(void *rtwdev, struct bb_la_mode_info *la_mode_info){
	struct la_dma_info	*dma = &la_mode_info->la_dma_i;
	struct la_mac_cfg_info *cfg = &la_mode_info->la_mac_cfg_i;
	u8 mac_rpt_state = LA_HW_IDLE;
	u32 mac_trig_fail;
	u8 tmp_u1b = 0;
	u8 i = 0, la_hw_finish_cnt = 0;
	u16 finish_ofst = 0;
	bool round_up = 0;
	bool loss_data;

	if (la_mode_info->la_mode_state != LA_STATE_MAIN) {
		halbb_la_stop(la_mode_info);
	}

	printf("[*] 1. [BB Setting] dbg_port = ((0x%x)), Trig_Edge = ((%d)), smp_rate = ((%d)), Trig_Bit_Sel = ((0x%x)), Dma_type = ((%d))\n",
		 la_mode_info->la_dbg_port, la_mode_info->la_trigger_edge,
		 la_mode_info->la_smp_rate, la_mode_info->la_and0_bit_sel, dma->dma_data_type);

	halbb_la_bb_set_general(la_mode_info);
	halbb_la_bb_set_dma_type(la_mode_info);
	halbb_la_bb_set_dbg_port(la_mode_info->not_stop_trig);
	halbb_la_bb_set_trig(la_mode_info->la_and0_disable, la_mode_info->adv_trig_i.adv_trig_en, la_mode_info->not_stop_trig);
	halbb_la_bb_set_re_trig(la_mode_info->la_re_trig_i.re_trig_en);
	halbb_la_mac_set_trig(la_mode_info->la_trig_mac_i.la_mac_trig_en);

	halbb_la_mac_cfg_cmn(rtwdev, la_mode_info);

	if (la_mode_info->not_stop_trig) {
		halbb_delay_ms(100);
		halbb_la_bb_set_dbg_port(false);
	}

	mac_trig_fail = rtw_hal_mac_lamode_trig(1);

	if (!mac_trig_fail) {
		do { /*Polling time always use 100ms, when it exceed 2s, break loop*/
			rtw_hal_mac_get_lamode_st((u8 *)&mac_rpt_state,
						  &finish_ofst, &round_up,
						  &loss_data);

			printf("[%d] rpt=((0x%x)), finish_ofst =((%d)), round_up =((%d)) \n", i, mac_rpt_state, finish_ofst, round_up);

			if (mac_rpt_state == LA_HW_RE_START) {
				printf("[Restart]\n");
				break;
			} else if (mac_rpt_state == LA_HW_FINISH_STOP) {
				printf("[LA Query OK] cnt:%d\n", ++la_hw_finish_cnt);
				if (la_hw_finish_cnt >= 2){
					break;
				}
			} else if (mac_rpt_state == LA_HW_START) {
				halbb_delay_ms(100);
				i++;
				continue;
			} else if (mac_rpt_state == LA_HW_FINISH_TIMEOUT) {
				printf("[LA HW timeout]\n");
				break;
			} else { /*LA_HW_IDLE)*/
				break;
			}
		} while (i < la_mode_info->la_polling_cnt);

		if (mac_rpt_state == LA_HW_FINISH_STOP) {
			la_mode_info->la_mode_state = LA_STATE_GET_DLE_BUF;
			halbb_la_rpt_buf_get(finish_ofst, round_up);
		} else if (mac_rpt_state == LA_HW_RE_START) {
			la_mode_info->la_mode_state = LA_STATE_WAIT_RESTART;
			la_mode_info->la_count = 0;
			return;
		} else {
			printf("[Polling Fail]\n");
		}
	}

	halbb_la_stop(la_mode_info);

	printf("[LA mode] la_count = ((%d))\n", la_mode_info->la_count);
	if (la_mode_info->la_count <= 1) {
		printf("LA Dump finished ---------->\n\n\n");
		halbb_release_bb_dbg_port(la_mode_info);
	} else {
		la_mode_info->la_count--;
		printf("LA Dump more ---------->\n\n\n");

		la_mode_info->la_timer_i.cb_time = 500;
		halbb_cfg_timers(BB_SET_TIMER, &la_mode_info->la_timer_i);
	}
}

void halbb_la_run(void *rtwdev, struct bb_la_mode_info *la_mode_info){
	struct la_string_info *buf = &la_mode_info->la_string_i;
	bool is_set_success = true;

	printf("[*] LA_State=(%d), mode=%d, phy_idx=%d\n", la_mode_info->la_mode_state, la_mode_info->la_run_mode, bb_phy_idx);

	if (!la_mode_info->la_mac_cfg_i.mac_alloc_success) {
		la_mode_info->la_mac_cfg_i.mac_alloc_success = halbb_la_mac_cfg_buf(rtwdev, la_mode_info, la_mode_info->la_mac_cfg_i.mac_la_buf_sel);
		if (!la_mode_info->la_mac_cfg_i.mac_alloc_success) {
			printf("[!] MAC BUF CR set fail)\n");
			return;
		}
	}

	if (la_mode_info->la_mode_state != LA_STATE_IDLE) {
		halbb_la_stop(la_mode_info);
		return;
	}

	if (buf->length == 0)
		is_set_success = halbb_la_drv_buf_allocate(la_mode_info);

	if (!is_set_success) {
		printf("[!] LA_BUf_alloc fail)\n");
		return;
	}

	memset(buf->octet, 0, buf->buffer_size);

	la_mode_info->la_re_trig_i.re_trig_wait_cnt = 0;
	la_mode_info->la_mode_state = LA_STATE_MAIN;
	halbb_la_main(rtwdev, la_mode_info);
}


/* function prototypes */
void elf_initialize_driver(void);
void *get_ieee80211_priv_data(void);

int main(void){
	int ret = 0;
	void *rtwdev = NULL;
	struct la_adv_trig_info *adv = NULL;
	struct la_dma_info      *dma = NULL;
	struct bb_la_mode_info la_mode_info = {};

	memset(&la_mode_info, 0, sizeof(struct bb_la_mode_info));
	elf_initialize_driver();
	/* get rtwdev from the private data of ieee80211_local */
	rtwdev = get_ieee80211_priv_data();
	if(rtwdev == NULL){
		printf("[!] cannot find the ieee80211 driver private data!\n");
		ret = -EINVAL;
	}

	dma = &la_mode_info.la_dma_i;
	adv = &la_mode_info.adv_trig_i;

	/* rewrite lamode 'reset' command */
	la_mode_info.la_mode_state = LA_STATE_IDLE;
	la_mode_info.la_print_i.is_la_print = false;
	la_mode_info.not_stop_trig = false;
	la_mode_info.la_and0_bit_sel = 0;

        halbb_la_bb_set_cmn_reset(&la_mode_info);
        halbb_la_bb_set_dma_type_reset(&la_mode_info);
        halbb_la_bb_set_adv_reset(&la_mode_info);
        halbb_la_mac_set_adv_reset(&la_mode_info);
        halbb_la_bb_set_re_trig_reset(&la_mode_info);

        halbb_la_mac_init(rtwdev, &la_mode_info);
	/* end of lamode 'reset' command */
	/* start of lamode 'buf 1' command */
	la_mode_info.la_mac_cfg_i.mac_la_buf_sel = LA_BUFF_128K;
	la_mode_info.la_mac_cfg_i.mac_alloc_success = halbb_la_mac_cfg_buf(rtwdev, &la_mode_info, la_mode_info.la_mac_cfg_i.mac_la_buf_sel);
	/* end of lamode 'buf 1' command */
	/* start of lamode 'fast cca' command */
	la_mode_info.la_mode_state = LA_STATE_IDLE;
	la_mode_info.la_print_i.is_la_print = false;
	la_mode_info.not_stop_trig = false;
	la_mode_info.la_and0_bit_sel = 0;

	halbb_la_bb_set_cmn_reset(&la_mode_info);
	halbb_la_bb_set_dma_type_reset(&la_mode_info);
	halbb_la_bb_set_adv_reset(&la_mode_info);
	halbb_la_mac_set_adv_reset(&la_mode_info);
	halbb_la_bb_set_re_trig_reset(&la_mode_info);

	halbb_la_mac_init(rtwdev, &la_mode_info);

	la_mode_info.la_mac_cfg_i.la_trigger_time = 390;
	dma->dma_data_type = DMA01_NRML_2s_12b;
	dma->dma_hdr_sel_63 = LA_HDR_CCA;
	dma->dma_hdr_sel_62 = LA_HDR_AGC_RDY;
	dma->dma_hdr_sel_61 = LA_HDR_RXHE_OFDMA;
	dma->dma_hdr_sel_60 = LA_HDR_ORI;

	/* DBG_PORT */
	la_mode_info.la_dbg_port = 0x10203; /*dbg_rx_inner_state_2*/
	/* AND_0 */
	la_mode_info.la_and0_disable = true;
	/* AND_1~AND_7 */
	adv->adv_trig_en = true;
	/* AND_5 */
	adv->la_and5_sel = LA_OFDM_CCA;
	adv->la_and5_inv = 0;
	adv->la_and5_val = 0; /* NHT_6M_DATA */
	printf("[*] Fast Trigger\n");
	printf("[*] la_trigger_time=%d\n", la_mode_info.la_mac_cfg_i.la_trigger_time);
	halbb_la_cr_dump(&la_mode_info);
	la_mode_info.la_run_mode = LA_RUN_FAST;
	la_mode_info.la_count = la_mode_info.la_count_max;
	halbb_la_run(rtwdev, &la_mode_info);
	/* end of lamode 'fast cca' command */

	return ret;
}

