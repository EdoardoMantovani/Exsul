/* Made by Edoardo Mantovani
 * simple C "script" which permits to put the Wi-Fi card into the monitor mode and dump rx frames in realtime
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <linux/const.h>
#include <libusb-1.0/libusb.h>
#include <asm-generic/errno-base.h>

#define u8        uint8_t
#define u16       uint16_t
#define u32       uint32_t
#define u64       uint64_t

#define s8        int8_t
#define s16       int16_t
#define s32       int32_t
#define s64       int64_t

#define __u8      u8
#define __u16     u16
#define __u32     u32
#define __u64     u64

#define __s8      s8
#define __s16     s16
#define __s32     s32
#define __s64     s64

#define be8       __be8
#define be16      __be16
#define be32      __be32
#define be64      __be64

#define USB_TRANSFER_POOL_SIZE	128
#define USB_TRANSFER_PREALLOC   64
#define USB_ACTIVE_TRANSFER_MAX 2048
#define USB_EVENT_WAIT_USEC     10000

#define MAX_TCC_RUNTIME_CONTEXTS 4
#define USB_MAX_ENDPOINT_NUM     30

/* exsul ieee80211 macro */
#define EXSUL_FIF_BCN_PRBRESP_PROMISC           (1 << 4)
#define EXSUL_FIF_OTHER_BSS                     (1 << 6)
#define EXSUL_FIF_PROBE_REQ                     (1 << 8)
#define EXSUL_RX_DUMP_FILTER_FLAGS		(EXSUL_FIF_BCN_PRBRESP_PROMISC | EXSUL_FIF_OTHER_BSS)

/* copy just enough of 'module_controller' for arriving to the fields 'rx_dump_enabled' and rx_dump_sequence' */
struct module_controller {
        char module_name[32];
        /* current module major, minor and patch version */
        u8 kernel_major_version;
        u8 kernel_minor_version;
        u8 kernel_patch;
        /* mantain the count of the loaded modules in the current address space */
        int modules_loaded;
        /* mantain a track of the current memory used for everything, in bytes */
        int total_memory_used;
        int fd;
        /* pcap file descriptor, enabled with the 'ops rx on' command */
        int pcap_fd;
        u8 *trampoline_area;
        int trampoline_used;

        /* emulates jiffies */
        u64 jiffies;

        /* special flag which permits to search for the vid/pid of the 'DISK' version of the usb dongles */
        bool enable_usb_disk_search;

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
        libusb_context *usb_ctx;
        libusb_device_handle *usb_dev;
        /* pointers to the 'usb_device' structs allocated for the 'probe' callback */
        void *usb_device_ptr;

        /* backup args used for running the 'xxx_driver_init' function */
        struct usb_interface *usb_interface;
        struct usb_device_id *usb_device_id;

        /* data gathered when the usb device is attached to the host system */
        u8 usb_interface_number;
        u8 usb_endpoint_num;
        struct libusb_endpoint_descriptor usb_ep[USB_MAX_ENDPOINT_NUM];
        struct libusb_device_descriptor usb_dev_desc;
        pthread_mutex_t urb_anchor_mutex;

        pthread_mutex_t skb_queue_lock;
        bool connection_tx_trace;
        bool rx_dump_enabled;
        uint32_t rx_dump_sequence;
	/* ... */
};

static inline int ieee80211_update_rx_filter(struct ieee80211_local *local, unsigned int flags, bool enable){
        int ret = 0;
        unsigned int new_flags = 0;
        unsigned int changed_flags = 0;
        struct ieee80211_ops *ops = NULL;
        void (*configure_filter)(struct ieee80211_hw *, unsigned int, unsigned int *, u64) = NULL;

        ops = get_ieee80211_ops();
        if (local == NULL || ops == NULL){
                return ret;
        }
        configure_filter = ops->configure_filter;
        if (configure_filter == NULL){
                return ret;
        }
        new_flags = local->filter_flags;
        if (enable){
                new_flags |= flags;
        }else{
                new_flags &= ~flags;
        }
        changed_flags = local->filter_flags ^ new_flags;
        if (changed_flags == 0){
                return ret;
        }
        configure_filter(local->ieee80211_hw, changed_flags, &new_flags, 0);
        local->filter_flags = new_flags;
        return ret;
}

static inline int ieee80211_configure_rx_dump_filter(struct ieee80211_local *local, bool enable){
	int ret = 0;

	ret = ieee80211_update_rx_filter(local, EXSUL_RX_DUMP_FILTER_FLAGS, enable);
	return ret;
}

int main(int argc, char *argv[]){
        int ret = 0;
	bool enable = true;
	struct ieee80211_local *local = NULL;
	struct module_controller *director = NULL;

        elf_initialize_driver();
	director = get_director();
	if(director == NULL){
		printf("[!] failed to get the module director!\n");
		ret = -EINVAL;
		return ret;
	}
	local = get_ieee80211_local();
	if(local == NULL){
		printf("[!] \'ieee80211_local\' is NULL!\n");
		ret = -EINVAL;
		return ret;
	}
	ret = ieee80211_configure_rx_dump_filter(local, enable);
	#if 0
	atomic_store(&director->rx_dump_enabled, enable);
	if(enable == true){
		atomic_store(&director->rx_dump_sequence, 0);
	}
	#endif
	/* now start the monitor mode from the high level API */
	ret = wifi_start_monitor();
        return ret;
}
