/* Made by Edoardo Mantovani, 2026 */
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
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

#define IEEE80211_NUM_ACS    		4
#define IEEE80211_MAX_QUEUES 		16
#define MAX_FILE_IDX         		32
#define IEEE80211_MAX_SSID_LEN 		32
#define USB_MAX_ENDPOINT_NUM 		30
#define MAX_KMEM_ARRAY_SIZE  		36
#define MAX_USB_DEV_LIST     		36
#define MAX_DEBUGFS_SIZE     		128
#define MAX_USERSPACE_TO_DRIVER_IO_SIZE	256
#define MAX_DRIVER_TO_USERSPACE_IO_SIZE 256
#define IEEE80211_HT_MCS_MASK_LEN       10

#define atomic_bool			bool

#define BITS_PER_BYTE			8
#define BITS_PER_TYPE(type)     	(sizeof(type) * BITS_PER_BYTE)

#define DIV_ROUND_UP(n,d)      	 	(((n) + (d) - 1) / (d))
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

#define container_of(ptr, type, member)                                        \
        ({                                                                     \
                const typeof(((type *)0)->member) *__mptr = ((void *)(ptr));   \
                (type *)((char *)__mptr - offsetof(type, member));             \
        })

#define timer_container_of(var, callback_timer, timer_fieldname)		\
container_of(callback_timer, typeof(*var), timer_fieldname)

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

struct timer_list {
	struct hlist_node       entry;
	unsigned long           expires;
	void                    (*function)(struct timer_list *);
	u32                     flags;
};

#define RTW89_CORE_RX_TYPE_CHAN_INFO	2
#define RTW89_USB_RX_ALIGNMENT		8
#define RTW89_RXDESC_SHORT_LEN		16
#define RTW89_RXDESC_LONG_LEN		32
#define RTW89_CHINFO_HDR_LEN		8
#define RTW89_CHINFO_PHY_HDR_LEN		8
#define RTW89_CSI_DATA_MAX		65536

/* RTL8852C MAC channel-info registers. */
#define R_AX_BBRPT_COEX_CFG		0x9604
#define B_AX_BBRPT_COEX_EN		0x00000001
#define R_AX_CH_INFO			0x9620
#define B_AX_CH_INFO_EN			0x00000001
#define B_AX_CH_INFO_ON			0x00000008
#define B_AX_CH_INFO_STOP_REQ		0x00000010
#define B_AX_CH_INFO_STOP		0x00000020
#define R_AX_BBRPT_CHINFO_ERR_IMR_ISR	0x962c
#define B_AX_BBRPT_CHINFO_ERR_MASK	0x00ff0000
#define R_AX_CH_INFO_QRY		0xce44
#define B_AX_CH_INFO_REQUESTING		0x00100000
#define B_AX_CH_INFO_MGNT_FRM		0x00080000
#define B_AX_CH_INFO_CTRL_FRM		0x00040000
#define B_AX_CH_INFO_DATA_FRM		0x00020000

/* RTL8852C AP2 BB channel-info registers from halbb_cr_info_8852c.h. */
#define R_8852C_CH_INFO_CFG		0x025c
#define B_8852C_CH_INFO_ENABLE		0x00000003
#define B_8852C_CH_INFO_FORMAT		0x000000fc
#define B_8852C_CH_INFO_BUFFER		0x0000ff00
#define R_8852C_CH_INFO_BITMAP		0x0260
#define R_8852C_CH_INFO_TYPE		0x2000
#define B_8852C_CH_INFO_TYPE		0x00004000
#define R_8852C_CH_INFO_SEG_LEN		0x2008
#define B_8852C_CH_INFO_SEG_LEN		0x06000000

struct csi_rx_meta {
	u32 timestamp;
	u16 data_rate;
	u8 bandwidth;
	u8 matrix_report;
};

struct csi_reassembly {
	bool active;
	u64 report_id;
	u8 expected_seq;
	u8 total_segments;
	u8 n_rx;
	u8 n_sts;
	u8 noise;
	u8 evm[2];
	u8 rssi[2];
	u8 rssi_avg;
	u8 rxsc;
	u32 expected_len;
	u32 data_len;
	struct csi_rx_meta meta;
	u8 data[RTW89_CSI_DATA_MAX];
};

static pthread_mutex_t csi_lock = PTHREAD_MUTEX_INITIALIZER;
static struct csi_reassembly csi_frame;
static struct csi_reassembly csi_pending_frame;
static struct csi_reassembly csi_print_frame_buf;
static bool csi_pending_ready;
static volatile u64 csi_report_count;
static volatile u64 csi_segment_count;
static volatile u64 csi_chan_info_record_count;
static volatile u64 csi_bad_rx_record_count;
static volatile u64 csi_output_drop_count;
static volatile u64 csi_rx_type_count[16];
static volatile u64 csi_rx_macid_count[256];
static volatile u64 csi_rx_frame_type_count[4];
static volatile u64 csi_rx_invalid_macid_count;
static u32 csi_bb_bitmap = 0x303;
static u32 csi_bb_type = 1;
static bool csi_legacy_mode;
static bool csi_saw_channel_info_on;
static bool csi_saw_requesting;

int elf_initialize_driver(void);
void *get_ieee80211_priv_data(void);
int usb_assign_rx_observer(int (*callback)(void *, int));
u32 rtw89_usb_ops_read32(void *rtwdev, u32 addr);
void rtw89_usb_ops_write8(void *rtwdev, u32 addr, u8 val);
void rtw89_usb_ops_write32(void *rtwdev, u32 addr, u32 val);
void rtw89_phy_set_phy_regs(void *rtwdev, u32 addr, u32 mask, u32 val);
u32 rtw89_phy_read32_idx(void *rtwdev, u32 addr, u32 mask, int phy_idx);

static u32 csi_get_le32(const u8 *p)
{
	return (u32)p[0] | (u32)p[1] << 8 | (u32)p[2] << 16 | (u32)p[3] << 24;
}

static u32 csi_align(u32 value, u32 alignment)
{
	return (value + alignment - 1) & ~(alignment - 1);
}

static unsigned long csi_env_ulong(const char *name, unsigned long fallback)
{
	char *end = NULL;
	const char *value = getenv(name);
	unsigned long parsed;

	if (!value || !value[0])
		return fallback;
	errno = 0;
	parsed = strtoul(value, &end, 0);
	if (errno || end == value || *end != '\0') {
		printf("[!] ignoring invalid %s=%s\n", name, value);
		return fallback;
	}
	return parsed;
}

static void csi_hex_dump(const u8 *data, u32 len)
{
	u32 i;

	for (i = 0; i < len; i++) {
		if ((i & 15) == 0)
			printf("%04x: ", i);
		printf("%02X%c", data[i], (i & 15) == 15 || i + 1 == len ? '\n' : ' ');
	}
}

static void csi_print_frame(const struct csi_reassembly *frame)
{
	u32 bytes_per_complex = 4; /* 16-bit I plus 16-bit Q */
	u32 matrix = frame->n_rx * frame->n_sts;
	u32 tones = matrix ? frame->data_len / (matrix * bytes_per_complex) : 0;

	printf("\n=== RTW89 CSI report %llu ===\n",
	       (unsigned long long)frame->report_id);
	printf("valid=1 segments=%u data_len=%u tones=%u bits_per_component=16\n",
	       frame->total_segments, frame->data_len, tones);
	printf("nr=%u nc=%u noise=%u evm={%u,%u} rssi={%u,%u} rssi_avg=%u rxsc=%u\n",
	       frame->n_rx, frame->n_sts, frame->noise,
	       frame->evm[0], frame->evm[1], frame->rssi[0] >> 1,
	       frame->rssi[1] >> 1, frame->rssi_avg >> 1, frame->rxsc);
	printf("timestamp=%u data_rate=0x%03x bandwidth=%u matrix_report=%u\n",
	       frame->meta.timestamp, frame->meta.data_rate,
	       frame->meta.bandwidth, frame->meta.matrix_report);
	printf("CSI RAW:\n");
	csi_hex_dump(frame->data, frame->data_len);
	fflush(stdout);
}

static void csi_copy_frame(struct csi_reassembly *dst,
			   const struct csi_reassembly *src)
{
	size_t header_len = offsetof(struct csi_reassembly, data);

	memcpy(dst, src, header_len);
	memcpy(dst->data, src->data, src->data_len);
}

static void csi_queue_complete_frame(void)
{
	csi_frame.report_id = ++csi_report_count;
	if (csi_pending_ready) {
		csi_output_drop_count++;
		return;
	}
	csi_copy_frame(&csi_pending_frame, &csi_frame);
	csi_pending_ready = true;
}

static bool csi_drain_output(void)
{
	bool ready = false;

	pthread_mutex_lock(&csi_lock);
	if (csi_pending_ready) {
		csi_copy_frame(&csi_print_frame_buf, &csi_pending_frame);
		csi_pending_ready = false;
		ready = true;
	}
	pthread_mutex_unlock(&csi_lock);

	if (ready)
		csi_print_frame(&csi_print_frame_buf);
	return ready;
}

static void csi_reset_reassembly(void)
{
	memset(&csi_frame, 0, sizeof(csi_frame));
}

static void csi_process_channel_info(const u8 *payload, u32 len,
				     const struct csi_rx_meta *meta)
{
	u32 total_len;
	u32 available;
	u32 remaining;
	u32 copy_len;
	u32 data_offset;
	u8 total_segments;
	u8 seq;
	u8 flags;
	bool last;

	if (len < RTW89_CHINFO_HDR_LEN)
		return;

	total_len = (u32)payload[0] | (u32)payload[1] << 8 |
		    (u32)(payload[2] & 1) << 16;
	total_segments = payload[2] >> 1;
	flags = payload[4];
	seq = payload[7];
	last = flags & 1;

	if (!(flags & 2) || !total_segments || total_len < 16 ||
	    total_len - 16 > RTW89_CSI_DATA_MAX)
		return;

	data_offset = RTW89_CHINFO_HDR_LEN;
	if (seq == 0) {
		if (len < RTW89_CHINFO_HDR_LEN + RTW89_CHINFO_PHY_HDR_LEN)
			return;
		csi_reset_reassembly();
		csi_frame.active = true;
		csi_frame.expected_seq = 0;
		csi_frame.total_segments = total_segments;
		csi_frame.expected_len = total_len - 16;
		csi_frame.noise = payload[3];
		csi_frame.n_rx = (flags >> 2) & 7;
		csi_frame.n_sts = (flags >> 5) & 7;
		csi_frame.evm[0] = payload[6];
		csi_frame.rssi[0] = payload[8];
		csi_frame.rssi[1] = payload[9];
		csi_frame.rssi_avg = payload[12];
		csi_frame.rxsc = payload[13] & 0xf;
		csi_frame.evm[1] = (payload[14] & 0xf) << 4 | payload[13] >> 4;
		csi_frame.meta = *meta;
		data_offset += RTW89_CHINFO_PHY_HDR_LEN;
	}

	if (!csi_frame.active || seq != csi_frame.expected_seq ||
	    total_segments != csi_frame.total_segments) {
		csi_reset_reassembly();
		return;
	}

	available = len - data_offset;
	remaining = csi_frame.expected_len - csi_frame.data_len;
	copy_len = available < remaining ? available : remaining;
	if (copy_len) {
		memcpy(csi_frame.data + csi_frame.data_len,
		       payload + data_offset, copy_len);
		csi_frame.data_len += copy_len;
	}
	csi_frame.expected_seq++;
	csi_segment_count++;

	if (last || csi_frame.expected_seq == csi_frame.total_segments ||
	    csi_frame.data_len == csi_frame.expected_len) {
		if (last && csi_frame.data_len == csi_frame.expected_len) {
			csi_queue_complete_frame();
		} else {
			printf("[!] dropped incomplete CSI report: segments=%u/%u bytes=%u/%u last=%u\n",
			       csi_frame.expected_seq, csi_frame.total_segments,
			       csi_frame.data_len, csi_frame.expected_len, last);
		}
		csi_reset_reassembly();
	}
}

static int csi_usb_rx_observer(void *buffer, int buffer_len)
{
	const u8 *data = buffer;
	u32 remaining;
	u32 cursor = 0;

	if (!data || buffer_len < RTW89_RXDESC_SHORT_LEN)
		return 0;
	remaining = (u32)buffer_len;

	while (remaining >= RTW89_RXDESC_SHORT_LEN) {
		struct csi_rx_meta meta;
		u32 dword0 = csi_get_le32(data + cursor);
		u32 dword1 = csi_get_le32(data + cursor + 4);
		u32 dword2 = csi_get_le32(data + cursor + 8);
		u32 dword3 = csi_get_le32(data + cursor + 12);
		u32 pkt_size = dword0 & 0x3fff;
		u32 shift_len = ((dword0 >> 14) & 3) * 2;
		u32 drv_info_len = ((dword0 >> 28) & 7) * 8;
		u32 pkt_type = (dword0 >> 24) & 0xf;
		u32 rxd_len = dword0 & 0x80000000U ?
			      RTW89_RXDESC_LONG_LEN : RTW89_RXDESC_SHORT_LEN;
		u32 pkt_offset = rxd_len + shift_len + drv_info_len;
		u32 consumed;

		if (!pkt_size || pkt_offset > remaining ||
		    pkt_size > remaining - pkt_offset) {
			csi_bad_rx_record_count++;
			break;
		}

		meta.timestamp = dword2;
		meta.data_rate = (dword1 >> 16) & 0x1ff;
		meta.bandwidth = (dword1 >> 29) & 7;
		meta.matrix_report = (dword3 >> 14) & 3;
		csi_rx_type_count[pkt_type]++;
		if (pkt_type == 0 && rxd_len == RTW89_RXDESC_LONG_LEN) {
			u32 dword4 = csi_get_le32(data + cursor + 16);
			u32 dword5 = csi_get_le32(data + cursor + 20);

			csi_rx_frame_type_count[dword4 & 3]++;
			if (dword5 & (1U << 28))
				csi_rx_macid_count[(dword5 >> 16) & 0xff]++;
			else
				csi_rx_invalid_macid_count++;
		}

		if (pkt_type == RTW89_CORE_RX_TYPE_CHAN_INFO) {
			csi_chan_info_record_count++;
			pthread_mutex_lock(&csi_lock);
			csi_process_channel_info(data + cursor + pkt_offset,
					 pkt_size, &meta);
			pthread_mutex_unlock(&csi_lock);
		}

		consumed = csi_align(pkt_offset + pkt_size, RTW89_USB_RX_ALIGNMENT);
		if (!consumed || consumed > remaining)
			break;
		cursor += consumed;
		remaining -= consumed;
	}
	return 0;
}

static void csi_configure_bb(void *rtwdev)
{
	/*
	 * Mirror the RTL8852C vendor driver's AP2 channel-info setup:
	 * channel estimation, 16-bit I/Q, 1/16 tone grouping, 2x2 MIMO,
	 * 12-qword BB segments, and MU buffer blocks 1 through 10.
	 */
	rtw89_phy_set_phy_regs(rtwdev, R_8852C_CH_INFO_CFG,
			       B_8852C_CH_INFO_FORMAT, 0x3e);
	rtw89_phy_set_phy_regs(rtwdev, R_8852C_CH_INFO_BITMAP,
			       0xffffffff, csi_bb_bitmap);
	rtw89_phy_set_phy_regs(rtwdev, R_8852C_CH_INFO_TYPE,
			       B_8852C_CH_INFO_TYPE, csi_bb_type);
	rtw89_phy_set_phy_regs(rtwdev, R_8852C_CH_INFO_SEG_LEN,
			       B_8852C_CH_INFO_SEG_LEN, 0);
	rtw89_phy_set_phy_regs(rtwdev, R_8852C_CH_INFO_CFG,
			       B_8852C_CH_INFO_BUFFER, 0xa1);
	rtw89_phy_set_phy_regs(rtwdev, R_8852C_CH_INFO_CFG,
			       B_8852C_CH_INFO_ENABLE, 1);
}

static void csi_dump_configuration(void *rtwdev)
{
	u32 bb_cfg = rtw89_phy_read32_idx(rtwdev, R_8852C_CH_INFO_CFG,
					 0xffffffff, 0);
	u32 bb_bitmap = rtw89_phy_read32_idx(rtwdev, R_8852C_CH_INFO_BITMAP,
					    0xffffffff, 0);
	u32 bb_type = rtw89_phy_read32_idx(rtwdev, R_8852C_CH_INFO_TYPE,
					  B_8852C_CH_INFO_TYPE, 0);
	u32 bb_seg = rtw89_phy_read32_idx(rtwdev, R_8852C_CH_INFO_SEG_LEN,
					 B_8852C_CH_INFO_SEG_LEN, 0);
	u32 route = rtw89_usb_ops_read32(rtwdev, R_AX_CH_INFO);
	u32 query = rtw89_usb_ops_read32(rtwdev, R_AX_CH_INFO_QRY);
	u32 error = rtw89_usb_ops_read32(rtwdev, R_AX_BBRPT_CHINFO_ERR_IMR_ISR);

	printf("[*] RTL8852CU CSI register readback: BB_CFG=0x%08x "
	       "BB_BITMAP=0x%08x type=%u seg=%u\n",
	       bb_cfg, bb_bitmap, bb_type, bb_seg);
	printf("[*] RTL8852CU CSI MAC readback: ROUTE=0x%08x QUERY=0x%08x "
	       "ERROR=0x%08x%s\n", route, query, error,
	       query & B_AX_CH_INFO_REQUESTING ? " (request active)" : "");
	if ((bb_cfg & B_8852C_CH_INFO_ENABLE) != 1 ||
	    (bb_cfg & B_8852C_CH_INFO_FORMAT) != 0xf8 ||
	    (bb_cfg & B_8852C_CH_INFO_BUFFER) != 0xa100 ||
	    bb_bitmap != csi_bb_bitmap || bb_type != csi_bb_type ||
	    !(route & B_AX_CH_INFO_EN) ||
	    !(query & B_AX_CH_INFO_EN))
		printf("[!] RTL8852CU channel-info register readback differs from the requested setup\n");
	if (error & B_AX_BBRPT_CHINFO_ERR_MASK)
		printf("[!] RTL8852CU channel-info hardware error flags: 0x%02x\n",
		       (error & B_AX_BBRPT_CHINFO_ERR_MASK) >> 16);
}

static void csi_sample_mac_state(void *rtwdev)
{
	u32 route = rtw89_usb_ops_read32(rtwdev, R_AX_CH_INFO);
	u32 query = rtw89_usb_ops_read32(rtwdev, R_AX_CH_INFO_QRY);

	if (route & B_AX_CH_INFO_ON)
		csi_saw_channel_info_on = true;
	if (query & B_AX_CH_INFO_REQUESTING)
		csi_saw_requesting = true;
}

static void csi_print_observed_macids(void)
{
	unsigned int printed = 0;
	unsigned int macid;

	printf("[*] Wi-Fi RX MACIDs observed:");
	for (macid = 0; macid < 256; macid++) {
		if (!csi_rx_macid_count[macid])
			continue;
		printf(" %u=%llu", macid,
		       (unsigned long long)csi_rx_macid_count[macid]);
		printed++;
	}
	if (!printed)
		printf(" none (no long Wi-Fi RX descriptor)");
	printf("\n");
	printf("[*] Wi-Fi RX eligibility: management=%llu control=%llu data=%llu "
	       "extension=%llu addr-cam-invalid=%llu\n",
	       (unsigned long long)csi_rx_frame_type_count[0],
	       (unsigned long long)csi_rx_frame_type_count[1],
	       (unsigned long long)csi_rx_frame_type_count[2],
	       (unsigned long long)csi_rx_frame_type_count[3],
	       (unsigned long long)csi_rx_invalid_macid_count);
}

static void csi_disable_bb(void *rtwdev)
{
	rtw89_phy_set_phy_regs(rtwdev, R_8852C_CH_INFO_CFG,
			       B_8852C_CH_INFO_ENABLE, 0);
	rtw89_phy_set_phy_regs(rtwdev, R_8852C_CH_INFO_CFG,
			       B_8852C_CH_INFO_BUFFER, 0);
}

static void csi_enable_rich_mode(void *rtwdev, u8 macid)
{
	u32 route;
	u32 query;

	/* Host port/queue 0, 512-byte segments, 4-unit interval, timeout 28. */
	route = (1U << 18) | (2U << 16) | (4U << 12) |
		(7U << 9) | B_AX_CH_INFO_EN;
	/* MIMO channel estimates require data; legacy mode can use ACKs/beacons. */
	query = B_AX_CH_INFO_EN | (u32)macid << 8 | B_AX_CH_INFO_DATA_FRM;
	if (csi_legacy_mode)
		query |= B_AX_CH_INFO_CTRL_FRM | B_AX_CH_INFO_MGNT_FRM;

	rtw89_usb_ops_write8(rtwdev, R_AX_BBRPT_COEX_CFG, B_AX_BBRPT_COEX_EN);
	rtw89_usb_ops_write32(rtwdev, R_AX_CH_INFO, route);
	rtw89_usb_ops_write32(rtwdev, R_AX_CH_INFO_QRY, query);
	printf("[*] enabled RTL8852CU rich channel-info mode for MACID %u\n", macid);
}

static void csi_disable_rich_mode(void *rtwdev)
{
	u32 value;
	unsigned int retry;

	rtw89_usb_ops_write32(rtwdev, R_AX_CH_INFO_QRY, 0);
	value = rtw89_usb_ops_read32(rtwdev, R_AX_CH_INFO);
	if (value & B_AX_CH_INFO_EN) {
		rtw89_usb_ops_write32(rtwdev, R_AX_CH_INFO,
				       value | B_AX_CH_INFO_STOP_REQ);
		for (retry = 0; retry < 100; retry++) {
			if (rtw89_usb_ops_read32(rtwdev, R_AX_CH_INFO) & B_AX_CH_INFO_STOP)
				break;
			usleep(100);
		}
	}
	rtw89_usb_ops_write32(rtwdev, R_AX_CH_INFO, 0);
}

int main(void)
{
	unsigned long duration_ms;
	unsigned long max_reports;
	unsigned long macid;
	unsigned long elapsed_ms = 0;
	bool observer_registered = false;
	void *rtwdev = NULL;
	int ret;

	duration_ms = csi_env_ulong("RTW89_CSI_DURATION_MS", 300);
	max_reports = csi_env_ulong("RTW89_CSI_MAX_REPORTS", 100);
	macid = csi_env_ulong("RTW89_CSI_MACID", 0);
	csi_legacy_mode = csi_env_ulong("RTW89_CSI_LEGACY", 0) != 0;
	if (csi_legacy_mode) {
		csi_bb_bitmap = 0x1;
		csi_bb_type = 0;
	}
	if (macid > 255) {
		printf("[!] RTW89_CSI_MACID must fit in 8 bits\n");
		return -EINVAL;
	}

	ret = elf_initialize_driver();
	if (ret) {
		printf("[!] failed to initialize driver callbacks: %d\n", ret);
		return ret;
	}
	rtwdev = get_ieee80211_priv_data();
	if (!rtwdev) {
		printf("[!] rtw89 private data is unavailable\n");
		return -ENODEV;
	}

	ret = usb_assign_rx_observer(csi_usb_rx_observer);
	if (ret) {
		printf("[!] failed to install USB-IN CSI observer: %d\n", ret);
		return ret;
	}
	observer_registered = true;

	csi_configure_bb(rtwdev);
	csi_enable_rich_mode(rtwdev, (u8)macid);
	printf("[*] RTL8852CU CSI profile: %s\n",
	       csi_legacy_mode ? "legacy 1x1 (management/control/data)" :
				 "MIMO 2x2 (data frames)");
	csi_dump_configuration(rtwdev);
	printf("[*] printing channel-info reports for %lu ms%s\n",
	       duration_ms, duration_ms ? "" : " (unbounded)");
	printf("[*] leave scan or other RX traffic active; set RTW89_CSI_MACID for an associated station\n");
	fflush(stdout);

	while ((!duration_ms || elapsed_ms < duration_ms) &&
	       (!max_reports || csi_report_count < max_reports)) {
		csi_drain_output();
		csi_sample_mac_state(rtwdev);
		usleep(10000);
		elapsed_ms += 10;
	}
	while (csi_drain_output())
		;

	if (duration_ms && !csi_report_count && !csi_legacy_mode &&
	    !csi_rx_frame_type_count[2]) {
		printf("[*] no MIMO data frame was observed; retrying with legacy 1x1 CSI\n");
		csi_disable_rich_mode(rtwdev);
		csi_disable_bb(rtwdev);
		csi_legacy_mode = true;
		csi_bb_bitmap = 0x1;
		csi_bb_type = 0;
		if (!csi_rx_macid_count[macid] && csi_rx_invalid_macid_count) {
			macid = 0xff;
			printf("[*] using MACID 255 for address-CAM-invalid management frames\n");
		}
		csi_configure_bb(rtwdev);
		csi_enable_rich_mode(rtwdev, (u8)macid);
		csi_dump_configuration(rtwdev);
		elapsed_ms = 0;
		while (elapsed_ms < duration_ms &&
		       (!max_reports || csi_report_count < max_reports)) {
			csi_drain_output();
			csi_sample_mac_state(rtwdev);
			usleep(10000);
			elapsed_ms += 10;
		}
		while (csi_drain_output())
			;
	}

	printf("[*] CSI summary: reports=%llu segments=%llu channel-info-records=%llu bad-rx-records=%llu output_drops=%llu\n",
	       (unsigned long long)csi_report_count,
	       (unsigned long long)csi_segment_count,
	       (unsigned long long)csi_chan_info_record_count,
	       (unsigned long long)csi_bad_rx_record_count,
	       (unsigned long long)csi_output_drop_count);
	printf("[*] RX descriptor types: wifi=%llu ppdu=%llu channel-info=%llu "
	       "tx-report=%llu dfs=%llu c2h=%llu\n",
	       (unsigned long long)csi_rx_type_count[0],
	       (unsigned long long)csi_rx_type_count[1],
	       (unsigned long long)csi_rx_type_count[2],
	       (unsigned long long)csi_rx_type_count[6],
	       (unsigned long long)csi_rx_type_count[8],
	       (unsigned long long)csi_rx_type_count[10]);
	csi_print_observed_macids();
	printf("[*] channel-info engine activity: on=%u requesting=%u\n",
	       csi_saw_channel_info_on, csi_saw_requesting);
	if (!csi_chan_info_record_count) {
		printf("[!] no RTL8852CU RX type-2 channel-info record was observed\n");
		csi_dump_configuration(rtwdev);
		if (!csi_rx_type_count[0] && !csi_rx_type_count[1])
			printf("[!] no Wi-Fi or PPDU RX traffic reached the USB observer during capture\n");
		else
			printf("[!] RX traffic was active; verify that the selected MACID receives matching frames\n");
	}
	csi_disable_rich_mode(rtwdev);
	csi_disable_bb(rtwdev);

out:
	if (observer_registered)
		usb_assign_rx_observer(NULL);
	return ret;
}
