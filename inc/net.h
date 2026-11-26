/* contains generic functions from the networking subsystem */

#ifndef MAX_ADDR_LEN
#define MAX_ADDR_LEN	  		32
#endif

#ifndef INIT_NETDEV_GROUP
#define INIT_NETDEV_GROUP 		0
#endif

#define NET_RX_SUCCESS			0	/* keep 'em coming, baby */
#define NET_RX_DROP			1	/* packet dropped */

#define DEFAULT_TX_QUEUE_LEN		1000
#define STAB_SIZE_LOG_MAX		30

#define NETDEV_HW_ADDR_T_LAN		1
#define NETDEV_HW_ADDR_T_SAN		2
#define NETDEV_HW_ADDR_T_UNICAST	3
#define NETDEV_HW_ADDR_T_MULTICAST	4

/* for setting kernel sock attribute on TCP connection setup */
#define GSO_MAX_SEGS		65535u
#define GSO_LEGACY_MAX_SIZE	65536u

/* TCP minimal MSS is 8 (TCP_MIN_GSO_SIZE),
 * and shinfo->gso_segs is a 16bit field.
 */
#define GSO_MAX_SIZE		(8 * GSO_MAX_SEGS)
#define TSO_LEGACY_MAX_SIZE	65536

/* copy'n'paste compression ;) */
#define __NETIF_F_BIT(bit)	((netdev_features_t)1 << (bit))
#define __NETIF_F(name)		__NETIF_F_BIT(NETIF_F_##name##_BIT)

#define NETIF_F_FCOE_CRC	__NETIF_F(FCOE_CRC)
#define NETIF_F_FRAGLIST	__NETIF_F(FRAGLIST)
#define NETIF_F_FSO		__NETIF_F(FSO)
#define NETIF_F_GRO		__NETIF_F(GRO)
#define NETIF_F_GRO_HW		__NETIF_F(GRO_HW)
#define NETIF_F_GSO		__NETIF_F(GSO)
#define NETIF_F_GSO_ROBUST	__NETIF_F(GSO_ROBUST)
#define NETIF_F_HIGHDMA		__NETIF_F(HIGHDMA)
#define NETIF_F_HW_CSUM		__NETIF_F(HW_CSUM)
#define NETIF_F_HW_VLAN_CTAG_FILTER __NETIF_F(HW_VLAN_CTAG_FILTER)
#define NETIF_F_HW_VLAN_CTAG_RX	__NETIF_F(HW_VLAN_CTAG_RX)
#define NETIF_F_HW_VLAN_CTAG_TX	__NETIF_F(HW_VLAN_CTAG_TX)
#define NETIF_F_IP_CSUM		__NETIF_F(IP_CSUM)
#define NETIF_F_IPV6_CSUM	__NETIF_F(IPV6_CSUM)
#define NETIF_F_LOOPBACK	__NETIF_F(LOOPBACK)
#define NETIF_F_LRO		__NETIF_F(LRO)
#define NETIF_F_NOCACHE_COPY	__NETIF_F(NOCACHE_COPY)
#define NETIF_F_NTUPLE		__NETIF_F(NTUPLE)
#define NETIF_F_RXCSUM		__NETIF_F(RXCSUM)
#define NETIF_F_RXHASH		__NETIF_F(RXHASH)
#define NETIF_F_SCTP_CRC	__NETIF_F(SCTP_CRC)
#define NETIF_F_SG		__NETIF_F(SG)
#define NETIF_F_TSO6		__NETIF_F(TSO6)
#define NETIF_F_TSO_ECN		__NETIF_F(TSO_ECN)
#define NETIF_F_GSO_ACCECN	__NETIF_F(GSO_ACCECN)
#define NETIF_F_TSO		__NETIF_F(TSO)
#define NETIF_F_VLAN_CHALLENGED	__NETIF_F(VLAN_CHALLENGED)
#define NETIF_F_RXFCS		__NETIF_F(RXFCS)
#define NETIF_F_RXALL		__NETIF_F(RXALL)
#define NETIF_F_GSO_GRE		__NETIF_F(GSO_GRE)
#define NETIF_F_GSO_GRE_CSUM	__NETIF_F(GSO_GRE_CSUM)
#define NETIF_F_GSO_IPXIP4	__NETIF_F(GSO_IPXIP4)
#define NETIF_F_GSO_IPXIP6	__NETIF_F(GSO_IPXIP6)
#define NETIF_F_GSO_UDP_TUNNEL	__NETIF_F(GSO_UDP_TUNNEL)
#define NETIF_F_GSO_UDP_TUNNEL_CSUM __NETIF_F(GSO_UDP_TUNNEL_CSUM)
#define NETIF_F_TSO_MANGLEID	__NETIF_F(TSO_MANGLEID)
#define NETIF_F_GSO_PARTIAL	 __NETIF_F(GSO_PARTIAL)
#define NETIF_F_GSO_TUNNEL_REMCSUM __NETIF_F(GSO_TUNNEL_REMCSUM)
#define NETIF_F_GSO_SCTP	__NETIF_F(GSO_SCTP)
#define NETIF_F_GSO_ESP		__NETIF_F(GSO_ESP)
#define NETIF_F_GSO_UDP		__NETIF_F(GSO_UDP)
#define NETIF_F_HW_VLAN_STAG_FILTER __NETIF_F(HW_VLAN_STAG_FILTER)
#define NETIF_F_HW_VLAN_STAG_RX	__NETIF_F(HW_VLAN_STAG_RX)
#define NETIF_F_HW_VLAN_STAG_TX	__NETIF_F(HW_VLAN_STAG_TX)
#define NETIF_F_HW_L2FW_DOFFLOAD	__NETIF_F(HW_L2FW_DOFFLOAD)
#define NETIF_F_HW_TC		__NETIF_F(HW_TC)
#define NETIF_F_HW_ESP		__NETIF_F(HW_ESP)
#define NETIF_F_HW_ESP_TX_CSUM	__NETIF_F(HW_ESP_TX_CSUM)
#define	NETIF_F_RX_UDP_TUNNEL_PORT  __NETIF_F(RX_UDP_TUNNEL_PORT)
#define NETIF_F_HW_TLS_RECORD	__NETIF_F(HW_TLS_RECORD)
#define NETIF_F_GSO_UDP_L4	__NETIF_F(GSO_UDP_L4)
#define NETIF_F_HW_TLS_TX	__NETIF_F(HW_TLS_TX)
#define NETIF_F_HW_TLS_RX	__NETIF_F(HW_TLS_RX)
#define NETIF_F_GRO_FRAGLIST	__NETIF_F(GRO_FRAGLIST)
#define NETIF_F_GSO_FRAGLIST	__NETIF_F(GSO_FRAGLIST)
#define NETIF_F_HW_MACSEC	__NETIF_F(HW_MACSEC)
#define NETIF_F_GRO_UDP_FWD	__NETIF_F(GRO_UDP_FWD)
#define NETIF_F_HW_HSR_TAG_INS	__NETIF_F(HW_HSR_TAG_INS)
#define NETIF_F_HW_HSR_TAG_RM	__NETIF_F(HW_HSR_TAG_RM)
#define NETIF_F_HW_HSR_FWD	__NETIF_F(HW_HSR_FWD)
#define NETIF_F_HW_HSR_DUP	__NETIF_F(HW_HSR_DUP)


#define VLAN_HLEN	4		/* The additional bytes required by VLAN
					 * (in addition to the Ethernet header)
					 */
#define VLAN_ETH_HLEN	18		/* Total octets in header.	 */
#define VLAN_ETH_ZLEN	64		/* Min. octets in frame sans FCS */

/*
 * According to 802.3ac, the packet can be 4 bytes longer. --Klika Jan
 */
#define VLAN_ETH_DATA_LEN	1500	/* Max. octets in payload	 */
#define VLAN_ETH_FRAME_LEN	1518	/* Max. octets in frame sans FCS */

#define VLAN_MAX_DEPTH		8 /* Max. number of nested VLAN tags parsed */

/* List of IP checksum features. Note that NETIF_F_HW_CSUM should not be
 * set in features when NETIF_F_IP_CSUM or NETIF_F_IPV6_CSUM are set--
 * this would be contradictory
 */
#define NETIF_F_CSUM_MASK	(NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM | \
				 NETIF_F_HW_CSUM)

#define NETIF_F_ALL_TSO 	(NETIF_F_TSO | NETIF_F_TSO6 | \
				 NETIF_F_TSO_ECN | NETIF_F_TSO_MANGLEID)

/* List of features with software fallbacks. */
#define NETIF_F_GSO_SOFTWARE	(NETIF_F_ALL_TSO | \
				 NETIF_F_GSO_ACCECN | NETIF_F_GSO_SCTP | \
				 NETIF_F_GSO_UDP_L4 | NETIF_F_GSO_FRAGLIST)
#ifndef NETDEV_UP
#define NETDEV_UP               1
#define NETDEV_DOWN             2
#define NETDEV_REBOOT           3
#define NETDEV_CHANGE           4
#define NETDEV_REGISTER         5
#define NETDEV_UNREGISTER       6
#define NETDEV_GOING_DOWN       9
#define NETDEV_CHANGENAME       11
#define NETDEV_PRE_UP           14
#define NETDEV_POST_INIT        17
#endif

#define MAX_NETDEVICE_NOTIFIERS 16

enum netdev_queue_state_t {
	__QUEUE_STATE_DRV_XOFF,
	__QUEUE_STATE_STACK_XOFF,
	__QUEUE_STATE_FROZEN,
};

enum {
	NETIF_F_SG_BIT,			/* Scatter/gather IO. */
	NETIF_F_IP_CSUM_BIT,		/* Can checksum TCP/UDP over IPv4. */
	__UNUSED_NETIF_F_1,
	NETIF_F_HW_CSUM_BIT,		/* Can checksum all the packets. */
	NETIF_F_IPV6_CSUM_BIT,		/* Can checksum TCP/UDP over IPV6 */
	NETIF_F_HIGHDMA_BIT,		/* Can DMA to high memory. */
	NETIF_F_FRAGLIST_BIT,		/* Scatter/gather IO. */
	NETIF_F_HW_VLAN_CTAG_TX_BIT,	/* Transmit VLAN CTAG HW acceleration */
	NETIF_F_HW_VLAN_CTAG_RX_BIT,	/* Receive VLAN CTAG HW acceleration */
	NETIF_F_HW_VLAN_CTAG_FILTER_BIT,/* Receive filtering on VLAN CTAGs */
	NETIF_F_VLAN_CHALLENGED_BIT,	/* Device cannot handle VLAN packets */
	NETIF_F_GSO_BIT,		/* Enable software GSO. */
	__UNUSED_NETIF_F_12,
	__UNUSED_NETIF_F_13,
	NETIF_F_GRO_BIT,		/* Generic receive offload */
	NETIF_F_LRO_BIT,		/* large receive offload */

	NETIF_F_GSO_SHIFT,		/* keep the order of SKB_GSO_* bits */
	NETIF_F_TSO_BIT			/* ... TCPv4 segmentation */
		= NETIF_F_GSO_SHIFT,
	NETIF_F_GSO_ROBUST_BIT,		/* ... ->SKB_GSO_DODGY */
	NETIF_F_TSO_ECN_BIT,		/* ... TCP ECN support */
	NETIF_F_TSO_MANGLEID_BIT,	/* ... IPV4 ID mangling allowed */
	NETIF_F_TSO6_BIT,		/* ... TCPv6 segmentation */
	NETIF_F_FSO_BIT,		/* ... FCoE segmentation */
	NETIF_F_GSO_GRE_BIT,		/* ... GRE with TSO */
	NETIF_F_GSO_GRE_CSUM_BIT,	/* ... GRE with csum with TSO */
	NETIF_F_GSO_IPXIP4_BIT,		/* ... IP4 or IP6 over IP4 with TSO */
	NETIF_F_GSO_IPXIP6_BIT,		/* ... IP4 or IP6 over IP6 with TSO */
	NETIF_F_GSO_UDP_TUNNEL_BIT,	/* ... UDP TUNNEL with TSO */
	NETIF_F_GSO_UDP_TUNNEL_CSUM_BIT,/* ... UDP TUNNEL with TSO & CSUM */
	NETIF_F_GSO_PARTIAL_BIT,	/* ... Only segment inner-most L4
					 *     in hardware and all other
					 *     headers in software.
					 */
	NETIF_F_GSO_TUNNEL_REMCSUM_BIT, /* ... TUNNEL with TSO & REMCSUM */
	NETIF_F_GSO_SCTP_BIT,		/* ... SCTP fragmentation */
	NETIF_F_GSO_ESP_BIT,		/* ... ESP with TSO */
	NETIF_F_GSO_UDP_BIT,		/* ... UFO, deprecated except tuntap */
	NETIF_F_GSO_UDP_L4_BIT,		/* ... UDP payload GSO (not UFO) */
	NETIF_F_GSO_FRAGLIST_BIT,		/* ... Fraglist GSO */
	NETIF_F_GSO_ACCECN_BIT,		/* TCP AccECN w/ TSO (no clear CWR) */
	NETIF_F_GSO_LAST =		/* last bit, see GSO_MASK */
		NETIF_F_GSO_ACCECN_BIT,
	NETIF_F_FCOE_CRC_BIT,		/* FCoE CRC32 */
	NETIF_F_SCTP_CRC_BIT,		/* SCTP checksum offload */
	NETIF_F_NTUPLE_BIT,		/* N-tuple filters supported */
	NETIF_F_RXHASH_BIT,		/* Receive hashing offload */
	NETIF_F_RXCSUM_BIT,		/* Receive checksumming offload */
	NETIF_F_NOCACHE_COPY_BIT,	/* Use no-cache copyfromuser */
	NETIF_F_LOOPBACK_BIT,		/* Enable loopback */
	NETIF_F_RXFCS_BIT,		/* Append FCS to skb pkt data */
	NETIF_F_RXALL_BIT,		/* Receive errored frames too */
	NETIF_F_HW_VLAN_STAG_TX_BIT,	/* Transmit VLAN STAG HW acceleration */
	NETIF_F_HW_VLAN_STAG_RX_BIT,	/* Receive VLAN STAG HW acceleration */
	NETIF_F_HW_VLAN_STAG_FILTER_BIT,/* Receive filtering on VLAN STAGs */
	NETIF_F_HW_L2FW_DOFFLOAD_BIT,	/* Allow L2 Forwarding in Hardware */

	NETIF_F_HW_TC_BIT,		/* Offload TC infrastructure */
	NETIF_F_HW_ESP_BIT,		/* Hardware ESP transformation offload */
	NETIF_F_HW_ESP_TX_CSUM_BIT,	/* ESP with TX checksum offload */
	NETIF_F_RX_UDP_TUNNEL_PORT_BIT, /* Offload of RX port for UDP tunnels */
	NETIF_F_HW_TLS_TX_BIT,		/* Hardware TLS TX offload */
	NETIF_F_HW_TLS_RX_BIT,		/* Hardware TLS RX offload */

	NETIF_F_GRO_HW_BIT,		/* Hardware Generic receive offload */
	NETIF_F_HW_TLS_RECORD_BIT,	/* Offload TLS record */
	NETIF_F_GRO_FRAGLIST_BIT,	/* Fraglist GRO */

	NETIF_F_HW_MACSEC_BIT,		/* Offload MACsec operations */
	NETIF_F_GRO_UDP_FWD_BIT,	/* Allow UDP GRO for forwarding */

	NETIF_F_HW_HSR_TAG_INS_BIT,	/* Offload HSR tag insertion */
	NETIF_F_HW_HSR_TAG_RM_BIT,	/* Offload HSR tag removal */
	NETIF_F_HW_HSR_FWD_BIT,		/* Offload HSR forwarding */
	NETIF_F_HW_HSR_DUP_BIT,		/* Offload HSR duplication */

	/*
	 * Add your fresh new feature above and remember to update
	 * netdev_features_strings[] in net/ethtool/common.c and maybe
	 * some feature mask #defines below. Please also describe it
	 * in Documentation/networking/netdev-features.rst.
	 */

	/* */NETDEV_FEATURE_COUNT
};

/* These flag bits are private to the generic network queueing
 * layer; they may not be explicitly referenced by any other
 * code.
 */
enum netdev_state_t {
	__LINK_STATE_START,
	__LINK_STATE_PRESENT,
	__LINK_STATE_NOCARRIER,
	__LINK_STATE_LINKWATCH_PENDING,
	__LINK_STATE_DORMANT,
	__LINK_STATE_TESTING,
};

enum netdev_reg_state {
	NETREG_UNINITIALIZED = 0,
	NETREG_REGISTERED,	/* completed register_netdevice */
	NETREG_UNREGISTERING,	/* called unregister_netdevice */
	NETREG_UNREGISTERED,	/* completed unregister todo */
	NETREG_RELEASED,	/* called free_netdev */
	NETREG_DUMMY,		/* dummy device for NAPI poll */
};

struct vlan_type_depth {
	__be16 type;
	u16 depth;
};

struct netdev_notifier_info {
	struct net_device *dev;
	void *extack;
};

struct netdev_addr_slot {
	struct net_device *dev;
	u8 addr[MAX_ADDR_LEN];
};

static struct netdev_addr_slot netdev_addr_slots[16];
static struct notifier_block *netdevice_notifiers[MAX_NETDEVICE_NOTIFIERS];

/*
 * enum netdev_priv_flags - &struct net_device priv_flags
 *
 * These are the &struct net_device, they are only set internally
 * by drivers and used in the kernel. These flags are invisible to
 * userspace; this means that the order of these flags can change
 * during any kernel release.
 *
 * You should add bitfield booleans after either net_device::priv_flags
 * (hotpath) or ::threaded (slowpath) instead of extending these flags.
 *
 * @IFF_802_1Q_VLAN: 802.1Q VLAN device
 * @IFF_EBRIDGE: Ethernet bridging device
 * @IFF_BONDING: bonding master or slave
 * @IFF_ISATAP: ISATAP interface (RFC4214)
 * @IFF_WAN_HDLC: WAN HDLC device
 * @IFF_XMIT_DST_RELEASE: dev_hard_start_xmit() is allowed to
 *	release skb->dst
 * @IFF_DONT_BRIDGE: disallow bridging this ether dev
 * @IFF_DISABLE_NETPOLL: disable netpoll at run-time
 * @IFF_MACVLAN_PORT: device used as macvlan port
 * @IFF_BRIDGE_PORT: device used as bridge port
 * @IFF_OVS_DATAPATH: device used as Open vSwitch datapath port
 * @IFF_TX_SKB_SHARING: The interface supports sharing skbs on transmit
 * @IFF_UNICAST_FLT: Supports unicast filtering
 * @IFF_TEAM_PORT: device used as team port
 * @IFF_SUPP_NOFCS: device supports sending custom FCS
 * @IFF_LIVE_ADDR_CHANGE: device supports hardware address
 *	change when it's running
 * @IFF_MACVLAN: Macvlan device
 * @IFF_XMIT_DST_RELEASE_PERM: IFF_XMIT_DST_RELEASE not taking into account
 *	underlying stacked devices
 * @IFF_L3MDEV_MASTER: device is an L3 master device
 * @IFF_NO_QUEUE: device can run without qdisc attached
 * @IFF_OPENVSWITCH: device is a Open vSwitch master
 * @IFF_L3MDEV_SLAVE: device is enslaved to an L3 master device
 * @IFF_TEAM: device is a team device
 * @IFF_RXFH_CONFIGURED: device has had Rx Flow indirection table configured
 * @IFF_PHONY_HEADROOM: the headroom value is controlled by an external
 *	entity (i.e. the master device for bridged veth)
 * @IFF_MACSEC: device is a MACsec device
 * @IFF_NO_RX_HANDLER: device doesn't support the rx_handler hook
 * @IFF_FAILOVER: device is a failover master device
 * @IFF_FAILOVER_SLAVE: device is lower dev of a failover master device
 * @IFF_L3MDEV_RX_HANDLER: only invoke the rx handler of L3 master device
 * @IFF_NO_ADDRCONF: prevent ipv6 addrconf
 * @IFF_TX_SKB_NO_LINEAR: device/driver is capable of xmitting frames with
 *	skb_headlen(skb) == 0 (data starts from frag0)
 */
enum netdev_priv_flags {
	IFF_802_1Q_VLAN			= 1 << 0,
	IFF_EBRIDGE			= 1 << 1,
	IFF_BONDING			= 1 << 2,
	IFF_ISATAP			= 1 << 3,
	IFF_WAN_HDLC			= 1 << 4,
	IFF_XMIT_DST_RELEASE		= 1 << 5,
	IFF_DONT_BRIDGE			= 1 << 6,
	IFF_DISABLE_NETPOLL		= 1 << 7,
	IFF_MACVLAN_PORT		= 1 << 8,
	IFF_BRIDGE_PORT			= 1 << 9,
	IFF_OVS_DATAPATH		= 1 << 10,
	IFF_TX_SKB_SHARING		= 1 << 11,
	IFF_UNICAST_FLT			= 1 << 12,
	IFF_TEAM_PORT			= 1 << 13,
	IFF_SUPP_NOFCS			= 1 << 14,
	IFF_LIVE_ADDR_CHANGE		= 1 << 15,
	IFF_MACVLAN			= 1 << 16,
	IFF_XMIT_DST_RELEASE_PERM	= 1 << 17,
	IFF_L3MDEV_MASTER		= 1 << 18,
	IFF_NO_QUEUE			= 1 << 19,
	IFF_OPENVSWITCH			= 1 << 20,
	IFF_L3MDEV_SLAVE		= 1 << 21,
	IFF_TEAM			= 1 << 22,
	IFF_RXFH_CONFIGURED		= 1 << 23,
	IFF_PHONY_HEADROOM		= 1 << 24,
	IFF_MACSEC			= 1 << 25,
	IFF_NO_RX_HANDLER		= 1 << 26,
	IFF_FAILOVER			= 1 << 27,
	IFF_FAILOVER_SLAVE		= 1 << 28,
	IFF_L3MDEV_RX_HANDLER		= 1 << 29,
	IFF_NO_ADDRCONF			= BIT_ULL(30),
	IFF_TX_SKB_NO_LINEAR		= BIT_ULL(31),
};

typedef u64 netdev_features_t;

struct netdev_hw_addr_list {
	struct list_head list;
	int count;

	/* Auxiliary tree for faster lookup on addition and deletion */
	struct rb_root tree;
};

struct netdev_hw_addr {
	struct list_head list;
	struct rb_node node;
	unsigned char addr[MAX_ADDR_LEN];
	unsigned char type;
	bool global_use;
	int sync_cnt;
	int refcount;
	int synced;
	struct rcu_head	rcu_head;
};

static inline struct tcphdr *tcp_hdr(const struct sk_buff *skb){
	return (struct tcphdr *)(skb->head + skb->transport_header);
}

static inline unsigned int __tcp_hdrlen(const struct tcphdr *th){
	int ret = 0;

	ret = th->doff * 4;
	return (unsigned int)ret;
}

static inline unsigned int tcp_hdrlen(const struct sk_buff *skb){
	int ret = 0;

	ret = __tcp_hdrlen(tcp_hdr(skb));
	return (unsigned int)ret;
}

/*
 * eth_random_addr - Generate software assigned random Ethernet address
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Generate a random Ethernet address (MAC) that is not multicast
 * and has the local assigned bit set.
 */
static inline void eth_random_addr(u8 *addr){
	get_random_bytes(addr, ETH_ALEN);
	addr[0] &= 0xfe;	/* clear multicast bit */
	addr[0] |= 0x02;	/* set local assignment bit (IEEE802) */
}

/*
 * eth_broadcast_addr - Assign broadcast address
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Assign the broadcast address to the given address array.
 */
static inline void eth_broadcast_addr(u8 *addr){
	memset(addr, 0xff, ETH_ALEN);
}

/*
 * eth_zero_addr - Assign zero address
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Assign the zero address to the given address array.
 */
static inline void eth_zero_addr(u8 *addr){
	memset(addr, 0x00, ETH_ALEN);
}

static inline void ether_addr_copy(u8 *dst, const u8 *src){
	*(u32 *)dst = *(const u32 *)src;
	*(u16 *)(dst + 4) = *(const u16 *)(src + 4);
}

static inline void netif_tx_stop_queue(struct netdev_queue *dev_queue){
	/* Must be an atomic op see netif_txq_try_stop() */
	set_bit(__QUEUE_STATE_DRV_XOFF, &dev_queue->state);
}

/*
 *	netif_running - test if up
 *	@dev: network device
 *
 *	Test if the device has been brought up.
 */
static inline bool netif_running(const struct net_device *netdev){
	return test_bit(__LINK_STATE_START, &netdev->state);
}

static inline struct netdev_queue *netdev_get_tx_queue(const struct net_device *netdev, unsigned int index){
	return &netdev->_tx[index];
}

static struct netdev_hw_addr *__hw_addr_create(const unsigned char *addr, int addr_len, unsigned char addr_type, bool global, bool sync){
        int alloc_size = 0;
        struct netdev_hw_addr *ha = NULL;

        alloc_size = sizeof(*ha);
        if (alloc_size < L1_CACHE_BYTES){
                alloc_size = L1_CACHE_BYTES;
        }
        ha = malloc(alloc_size);
	if (ha == NULL){
                return NULL;
        }

        memcpy(ha->addr, addr, addr_len);
        ha->type = addr_type;
        ha->refcount = 1;
        ha->global_use = global;
        ha->synced = sync ? 1 : 0;
        ha->sync_cnt = 0;
        return ha;
}

static int __hw_addr_add_ex(struct netdev_hw_addr_list *list, unsigned char *addr, int addr_len, unsigned char addr_type, bool global, bool sync, int sync_count, bool exclusive){
	int ret = 0;
	int diff = 0;
	struct rb_node **ins_point = &list->tree.rb_node, *parent = NULL;
	struct netdev_hw_addr *ha = NULL;

	if (addr_len > MAX_ADDR_LEN){
		ret = -EINVAL;
		return ret;
	}
	while (*ins_point) {
		ha = rb_entry(*ins_point, struct netdev_hw_addr, node);
		diff = memcmp(addr, ha->addr, addr_len);
		if (diff == 0){
			diff = memcmp(&addr_type, &ha->type, sizeof(addr_type));
		}

		parent = *ins_point;
		if (diff < 0){
			ins_point = &parent->rb_left;
		}else if (diff > 0){
			ins_point = &parent->rb_right;
		}else{
			if (exclusive){
				ret = -EEXIST;
				return ret;
			}
			if (global){
				/* check if addr is already used as global */
				if (ha->global_use){
					return ret;
				}else{
					ha->global_use = true;
				}
			}
			if (sync){
				if (ha->synced && sync_count){
					ret = -EEXIST;
					return ret;
				}else{
					ha->synced++;
				}
			}
			ha->refcount++;
			return ret;
		}
	}
	ha = __hw_addr_create(addr, addr_len, addr_type, global, sync);
	if (ha == NULL){
		ret = -ENOMEM;
		return ret;
	}

	rb_link_node(&ha->node, parent, ins_point);
	rb_insert_color(&ha->node, &list->tree);

	list_add_tail_rcu(&ha->list, &list->list);
	list->count++;
	return ret;
}

static int __hw_addr_add(struct netdev_hw_addr_list *list, unsigned char *addr, int addr_len, unsigned char addr_type){
	int ret = 0;

	ret = __hw_addr_add_ex(list, addr, addr_len, addr_type, false, false, 0, false);
	return ret;
}

static void init_dummy_netdev(struct net_device *netdev){
	netdev->reg_state = NETREG_DUMMY;

	set_bit(__LINK_STATE_PRESENT, &netdev->state);
	set_bit(__LINK_STATE_START,   &netdev->state);
}

/*
 *	dev_addr_init - Init device address list
 *	@dev: device
 *
 *	Init device address list and create the first element,
 *	used by ->dev_addr.
 *
 *	The caller must hold the rtnl_mutex.
 */
static int dev_addr_init(struct net_device *netdev){
	int ret = 0;
	unsigned char addr[MAX_ADDR_LEN] = {};
	struct netdev_hw_addr *ha = NULL;

	/* rtnl_mutex must be held here */

	__hw_addr_init(&netdev->dev_addrs);
	memset(addr, 0, sizeof(addr));
	ret = __hw_addr_add(&netdev->dev_addrs, addr, sizeof(addr), NETDEV_HW_ADDR_T_LAN);
	if (ret == 0) {
		/*
		 * Get the first (previously created) address from the list
		 * and set dev_addr pointer to this location.
		 */
		ha = list_first_entry(&netdev->dev_addrs.list, struct netdev_hw_addr, list);
		netdev->dev_addr = ha->addr;
	}
	director.net_device = netdev;
	return ret;
}

static int __netif_receive_skb_one_core(struct sk_buff *skb, bool pfmemalloc){
	int ret = 0;

	if (skb != NULL){
		consume_skb(skb);
	}
	return ret;
}

static int __netif_receive_skb(struct sk_buff *skb){
	int ret = 0;

	ret = __netif_receive_skb_one_core(skb, false);
	return ret;
}

static void __netif_receive_skb_list(struct list_head *head){
	if (!list_empty(head)){
		__netif_receive_skb_list_core(head, false);
	}
}

static void netif_receive_skb_list_internal(struct list_head *head){
	/* PHY RX timestamp deferral is disabled in this userspace stack. */
	__netif_receive_skb_list(head);
}

static int netif_receive_skb_internal(struct sk_buff *skb){
	int ret = 0;

	if (skb_defer_rx_timestamp(skb)){
		ret = NET_RX_SUCCESS;
		return ret;
	}
	ret = __netif_receive_skb(skb);
	return ret;
}

static struct vlan_type_depth __vlan_get_protocol_offset(const struct sk_buff *skb, __be16 type, int mac_offset){
	int ret = 0;
	unsigned int vlan_depth = 0;
	unsigned int parse_depth = 0;
	struct vlan_type_depth result = {};

	parse_depth = 8;
	vlan_depth = skb->mac_len;
	if(type != htons(ETH_P_8021Q) && type != htons(ETH_P_8021AD)){
		result.type = type;
		result.depth = vlan_depth;
		return result;
	}

	if(vlan_depth){
		if(vlan_depth < VLAN_HLEN){
			return result;
		}
		vlan_depth -= VLAN_HLEN;
	}else{
		vlan_depth = ETH_HLEN;
	}

	do{
		ret = skb_copy_bits(skb, mac_offset + vlan_depth + 2, &type, sizeof(type));
		if(ret || !--parse_depth){
			return result;
		}

		vlan_depth += VLAN_HLEN;
	}while(type == htons(ETH_P_8021Q) || type == htons(ETH_P_8021AD));
	result.type = type;
	result.depth = vlan_depth;
	return result;
}

static inline __be16 __vlan_get_protocol(const struct sk_buff *skb, __be16 type, int *depth){
	int ret = 0;
	struct vlan_type_depth result = {};

	result = __vlan_get_protocol_offset(skb, type, 0);

	if(depth && result.type){
		*depth = result.depth;
	}

	return result.type;
}

/*
 * vlan_get_protocol - get protocol EtherType.
 * @skb: skbuff to query
 *
 * Returns: the EtherType of the packet, regardless of whether it is
 * vlan encapsulated (normal or hardware accelerated) or not.
 */
static inline __be16 vlan_get_protocol(const struct sk_buff *skb){
	return __vlan_get_protocol(skb, skb->protocol, NULL);
}

/* Grab socket reference count. This operation is valid only
 * when sk is ALREADY grabbed f.e. it is found in hash table
 * or a list and the lookup is made under lock preventing hash table
 * modifications.
 */
static __always_inline void sock_hold(struct sock *sk){
	refcount_inc(&sk->sk_refcnt);
}

/* Ungrab socket in the context, which assumes that socket refcnt
 * cannot hit zero, f.e. it is true in context of any socketcall.
 */
static __always_inline void __sock_put(struct sock *sk){
	refcount_dec(&sk->sk_refcnt);
}

static int call_netdevice_notifiers(unsigned long state, struct net_device *dev){
	int ret = 0;
	int (*notifier_call)(struct notifier_block *nb, unsigned long action, void *data) = NULL;

	struct netdev_notifier_info info = {
		.dev = dev,
		.extack = NULL,
	};

	if(dev == NULL){
		ret = -EINVAL;
		return ret;
	}

	for(int i = 0; i < MAX_NETDEVICE_NOTIFIERS; i++){
		if(netdevice_notifiers[i] == NULL){
			continue;
		}
		notifier_call = netdevice_notifiers[i]->notifier_call;
		if(notifier_call != NULL){
			ret = notifier_call(netdevice_notifiers[i], state, &info);
			if(ret & NOTIFY_STOP_MASK){
				break;
			}
		}
	}
	return ret;
}

/* netdevice_init: wrapper for invoking the 'ndo_init' callback in 'struct net_device_ops' */
static int netdevice_init(struct net_device *dev){
	int ret = 0;
	struct net_device_ops *ops = NULL;
	int (*ndo_init)(struct net_device *dev) = NULL;

	if(dev == NULL){
		ret = -EINVAL;
		return ret;
	}
	ops = dev->netdev_ops;
	if(ops != NULL){
		ndo_init = ops->ndo_init;
	}
	if(ndo_init != NULL){
		ret = ndo_init(dev);
	}
	return ret;
}

/* netdevice_uninit: wrapper for invoking the 'ndo_uninit' callback in 'struct net_device_ops' */
static int netdevice_uninit(struct net_device *dev){
	int ret = 0;
	struct net_device_ops *ops = NULL;
	int (*ndo_uninit)(struct net_device *dev) = NULL;

	if(dev == NULL){
		ret = -EINVAL;
		return ret;
	}
	ops = dev->netdev_ops;
	if(ops != NULL){
		ndo_uninit = ops->ndo_uninit;
	}
	if(ndo_uninit != NULL){
		ret = ndo_uninit(dev);
	}
	return ret;
}

/* netdevice_open: wrapper for invoking the 'ndo_open' callback in 'struct net_device_ops' */
static int netdevice_open(struct net_device *dev){
	int ret = 0;
	struct net_device_ops *ops = NULL;
	int (*ndo_open)(struct net_device *dev) = NULL;

	if(dev == NULL){
		ret = -EINVAL;
		return ret;
	}
	call_netdevice_notifiers(NETDEV_PRE_UP, dev);
	ops = dev->netdev_ops;
	if(ops != NULL){
		ndo_open = ops->ndo_open;
	}
	if(ndo_open != NULL){
		ret = ndo_open(dev);
	}
	if(ret == 0){
		set_bit(__LINK_STATE_START, &dev->state);
		call_netdevice_notifiers(NETDEV_UP, dev);
	}
	return ret;
}

/* netdevice_stop: wrapper for invoking the 'ndo_stop' callback in 'struct net_device_ops' */
static int netdevice_stop(struct net_device *dev){
	int ret = 0;
	struct net_device_ops *ops = NULL;
	int (*ndo_stop)(struct net_device *dev) = NULL;

	if(dev == NULL){
		ret = -EINVAL;
		return ret;
	}
	ops = dev->netdev_ops;
	if(ops != NULL){
		ndo_stop = ops->ndo_stop;
	}
	if(ndo_stop != NULL){
		ret = ndo_stop(dev);
	}
	return ret;
}

/* netdevice_validate_address: wrapper for invoking the 'ndo_validate_addr' callback in 'struct net_device_ops' */
static int netdevice_validate_address(struct net_device *dev){
	int ret = 0;
	struct net_device_ops *ops = NULL;
	int (*ndo_validate_addr)(struct net_device *dev) = NULL;

	if(dev == NULL){
		ret = -EINVAL;
		return ret;
	}
	ops = dev->netdev_ops;
	if(ops != NULL){
		ndo_validate_addr = ops->ndo_validate_addr;
	}
	if(ndo_validate_addr != NULL){
		ret = ndo_validate_addr(dev);
	}
	return ret;
}

/* netdevice_set_rx_mode: wrapper for invoking the 'ndo_set_rx_mode' callback in 'struct net_device_ops' */
static int netdevice_set_rx_mode(struct net_device *dev){
	int ret = 0;
	struct net_device_ops *ops = NULL;
	int (*ndo_set_rx_mode)(struct net_device *dev) = NULL;

	if(dev == NULL){
		ret = -EINVAL;
		return ret;
	}
	ops = dev->netdev_ops;
	if(ops != NULL){
		ndo_set_rx_mode = ops->ndo_set_rx_mode;
	}
	if(ndo_set_rx_mode != NULL){
		ret = ndo_set_rx_mode(dev);
	}
	return ret;
}

/* netdevice_set_mac_address: wrapper for invoking the 'ndo_set_mac_address' callback in 'struct net_device_ops' */
static int netdevice_set_mac_address(struct net_device *dev, void *mac_address){
	int ret = 0;
	struct net_device_ops *ops = NULL;
	int (*ndo_set_mac_address)(struct net_device *dev, void *mac_address) = NULL;

	if(dev == NULL){
		ret = -EINVAL;
		return ret;
	}
	ops = dev->netdev_ops;
	if(ops != NULL){
		ndo_set_mac_address = ops->ndo_set_mac_address;
	}
	if(ndo_set_mac_address != NULL){
		ret = ndo_set_mac_address(dev, mac_address);
	}
	return ret;
}
