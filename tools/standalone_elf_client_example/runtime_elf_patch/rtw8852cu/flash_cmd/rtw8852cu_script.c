/* Made by Edoardo Mantovani, 2026 */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <libdwarf/libdwarf.h>
#include <linux/nl80211.h>
#include <linux/const.h>
#include <linux/if_ether.h>
#include <libusb-1.0/libusb.h>
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <asm-generic/param.h>

#if __BYTE_ORDER == __LITTLE_ENDIAN
#include <linux/byteorder/little_endian.h>
#else
#include <linux/byteorder/big_endian.h>
#endif

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

#ifndef le32_to_cpu
#define le32_to_cpu			__le32_to_cpu
#endif

#ifndef cpu_to_le32
#define cpu_to_le32			__cpu_to_le32
#endif

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

#define atomic_bool			bool

#define EXSUL_DIRECTOR_STRUCT_SIZES_OFFSET	13976
#define EXSUL_FAST_STRUCT_SIZE_NET_DEVICE_OFFSET 24
#define EXSUL_NET_DEVICE_FALLBACK_SIZE		(3 * 1024)
#define EXSUL_ALIGN_UP(value, align)		(((value) + ((align) - 1)) & ~((align) - 1))

#define RTW_ADAPTER_DVOBJ_OFFSET		64
#define RTW_ADAPTER_PNETDEV_OFFSET		21360
#define RTW_DVOBJ_PHL_OFFSET			200
#define RTW_PHL_HAL_OFFSET			34360
#define RTW_HAL_MAC_OFFSET			440

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

/* macros implemented by rtw8852cu */
#define dvobj_get_primary_adapter(dvobj)	((dvobj)->padapters[IFACE_ID0])
#define wiphy_to_adapter(wiphy)			(dvobj_get_primary_adapter(wiphy_to_dvobj(wiphy)))
#define wiphy_to_dvobj(wiphy)			(((struct rtw_wiphy_data *)wiphy_priv(wiphy))->dvobj)

/* enums used by rtw8852cu */
enum _IFACE_ID {
	IFACE_ID0, /*PRIMARY_ADAPTER*/
	IFACE_ID1,
	IFACE_ID2,
	IFACE_ID3,
	IFACE_ID4,
	IFACE_ID5,
	IFACE_ID6,
	IFACE_ID7,
	IFACE_ID_MAX,
};

#define MAX_READ_BUFFER_LENGTH 48

/* struct defined in rtw8852cu */
struct mac_ax_adapter {
        struct mac_ax_ops *ops;
        void *drv_adapter; /* hal_com adapter */
        void *phl_adapter; /* phl_com adapter */
        struct mac_ax_pltfm_cb *pltfm_cb;
};

struct rtw_netdev_priv_indicator {
	void *priv;
	u32 sizeof_priv;
};

/* prototypes for the standard kernel API && standalone ELF init functions */
void *get_director(void);
void *get_net_device(void);
void elf_initialize_driver(void);
void *get_ieee80211_priv_data(void);
int fast_get_symbol(unsigned char *sym_name, void **sym_addr);
u32 mac_flash_read(void *adapter, u32 addr, u32 length, u8 *buffer, u32 timeout);

/* simple support function for dumping the 'flash_read' command output */
void DumpHex(const void* data, size_t size) {
	char ascii[17];
	size_t i, j;
	ascii[16] = '\0';

	for (i = 0; i < size; ++i) {
		printf("%02X ", ((unsigned char*)data)[i]);
		if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
			ascii[i % 16] = ((unsigned char*)data)[i];
		} else {
			ascii[i % 16] = '.';
		}
		if ((i+1) % 8 == 0 || i+1 == size) {
			printf(" ");
			if ((i+1) % 16 == 0) {
				printf("|  %s \n", ascii);
			} else if (i+1 == size) {
				ascii[(i+1) % 16] = '\0';
				if ((i+1) % 16 <= 8) {
					printf(" ");
				}
				for (j = (i+1) % 16; j < 16; ++j) {
					printf("   ");
				}
				printf("|  %s \n", ascii);
			}
		}
	}
}

static u16 get_exsul_netdev_size(void)
{
	u8 *director = (u8 *)get_director();
	u16 netdev_size = 0;

	if (director != NULL) {
		netdev_size = *(u16 *)(director +
			EXSUL_DIRECTOR_STRUCT_SIZES_OFFSET +
			EXSUL_FAST_STRUCT_SIZE_NET_DEVICE_OFFSET);
	}

	if (netdev_size < 512 || netdev_size > 8192)
		netdev_size = EXSUL_NET_DEVICE_FALLBACK_SIZE;

	return netdev_size;
}

static void *read_ptr_at(void *base, u32 offset)
{
	if (base == NULL)
		return NULL;

	return *(void **)((u8 *)base + offset);
}

static void *get_live_mac_ax_adapter(void){
	u16 netdev_size = get_exsul_netdev_size();
	void *netdev = get_net_device();
	struct rtw_netdev_priv_indicator *npi = NULL;
	void *adapter = NULL;
	void *dvobj = NULL;
	void *phl = NULL;
	void *hal = NULL;
	void *mac = NULL;

	if (netdev == NULL) {
		printf("[!] net_device is NULL\n");
		return NULL;
	}

	npi = (struct rtw_netdev_priv_indicator *)((u8 *)netdev +
		EXSUL_ALIGN_UP(netdev_size, 32));
	if (npi == NULL || npi->priv == NULL) {
		printf("[!] rtw netdev private indicator is NULL\n");
		return NULL;
	}
	if (npi->sizeof_priv <= RTW_ADAPTER_PNETDEV_OFFSET || npi->sizeof_priv > (1024 * 1024)) {
		printf("[!] unexpected Realtek adapter private size: %u\n", npi->sizeof_priv);
		return NULL;
	}

	adapter = npi->priv;
	if (read_ptr_at(adapter, RTW_ADAPTER_PNETDEV_OFFSET) != netdev) {
		printf("[!] recovered adapter does not point back to net_device\n");
		return NULL;
	}
	dvobj = read_ptr_at(adapter, RTW_ADAPTER_DVOBJ_OFFSET);
	phl = read_ptr_at(dvobj, RTW_DVOBJ_PHL_OFFSET);
	hal = read_ptr_at(phl, RTW_PHL_HAL_OFFSET);
	mac = read_ptr_at(hal, RTW_HAL_MAC_OFFSET);

	if (mac == NULL) {
		printf("[!] failed to recover live mac_ax_adapter\n");
		printf("[!] netdev=%p netdev_size=%u adapter=%p dvobj=%p phl=%p hal=%p\n",
		       netdev, netdev_size, adapter, dvobj, phl, hal);
	}

	return mac;
}

int main(void){
	int ret = 0;
	u32 flash_read_dump_addr = 0;
	void *mac_ax_adapter = NULL;
	u8 read_buffer[MAX_READ_BUFFER_LENGTH] = {};

	mac_ax_adapter = get_live_mac_ax_adapter();
	if(mac_ax_adapter == NULL){
		ret = -EINVAL;
		return ret;
	}
	flash_read_dump_addr = 0x2000000;
	/* invoke the 'mac_flash_read' function */
	ret = mac_flash_read(mac_ax_adapter, flash_read_dump_addr, MAX_READ_BUFFER_LENGTH, read_buffer, 200);
	if(ret != 0){
		printf("[!] failed to send the flash_read command: 0x%08x\n", ret);
		goto end;
	}
	printf("[*] dumping flash_read command at 0x%08x\n", flash_read_dump_addr);
	DumpHex(read_buffer, MAX_READ_BUFFER_LENGTH);
	end:
	return ret;
}
