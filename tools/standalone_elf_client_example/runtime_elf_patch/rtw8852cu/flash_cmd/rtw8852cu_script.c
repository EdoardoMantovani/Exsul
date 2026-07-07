/* Made by Edoardo Mantovani, 2026 */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
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

/* struct defined in rtw8852cu */
struct mac_ax_adapter {
        struct mac_ax_ops *ops;
        void *drv_adapter; /* hal_com adapter */
        void *phl_adapter; /* phl_com adapter */
        struct mac_ax_pltfm_cb *pltfm_cb;
};

/* prototypes for the standard kernel API && standalone ELF init functions */
void *get_net_device(void);
void elf_initialize_driver(void);
void *get_ieee80211_priv_data(void);
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

int main(void){
	int ret = 0;
	u16 read_buffer_len = 0;
	u32 flash_read_dump_addr = 0;
	void *dvobj = NULL;
	struct mac_ax_adapter *mac_ax_adapter = NULL;
	u8 read_buffer[MAX_READ_BUFFER_LENGTH] = 0;

	elf_initialize_driver();
	/* get rtwdev from the private data of ieee80211_local */
	rtwdev = get_ieee80211_priv_data();
	if(rtwdev == NULL){
		printf("[!] cannot find the ieee80211 driver private data!\n");
		ret = -EINVAL;
		return ret;
	}
	/* after some research, I discovered that the equivalent of 'struct mac_ax_adapter' is a global variable
	*  in our case it is: 'mac_8852c_adapter'
	*/
	mac_ax_adapter = mac_8852c_adapter;
	if(mac_ax_adapter == NULL){
		ret = -EINVAL;
		return ret;
	}
	if(mac_ax_adapter->ops == NULL || mac_ax_adapter->drv_adapter == NULL || mac_ax_adapter->phl_adapter == NULL){
		ret = -EINVAL;
		return ret;
	}
	flash_read_dump_addr = 0x2000000;
	/* invoke the 'mac_flash_read' function */
	ret = mac_flash_read(mac_ax_adapter, flash_read_dump_addr, MAX_READ_BUFFER_LENGTH, &read_buffer, 200);
	if(ret < 0){
		printf("[!] failed to send the flash_read command!\n");
		goto end;
	}
	printf("[*] dumping flash_read command at %p\n", flash_read_dump_addr);
	DumpHex(read_buffer, MAX_READ_BUFFER_LENGTH);
	end:
}
