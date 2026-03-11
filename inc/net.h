/* contains generic functions from the networking subsystem */

#ifndef MAX_ADDR_LEN
#define MAX_ADDR_LEN	  32
#endif

#ifndef INIT_NETDEV_GROUP
#define INIT_NETDEV_GROUP 0
#endif

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

/**
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
	IFF_802_1Q_VLAN			= 1<<0,
	IFF_EBRIDGE			= 1<<1,
	IFF_BONDING			= 1<<2,
	IFF_ISATAP			= 1<<3,
	IFF_WAN_HDLC			= 1<<4,
	IFF_XMIT_DST_RELEASE		= 1<<5,
	IFF_DONT_BRIDGE			= 1<<6,
	IFF_DISABLE_NETPOLL		= 1<<7,
	IFF_MACVLAN_PORT		= 1<<8,
	IFF_BRIDGE_PORT			= 1<<9,
	IFF_OVS_DATAPATH		= 1<<10,
	IFF_TX_SKB_SHARING		= 1<<11,
	IFF_UNICAST_FLT			= 1<<12,
	IFF_TEAM_PORT			= 1<<13,
	IFF_SUPP_NOFCS			= 1<<14,
	IFF_LIVE_ADDR_CHANGE		= 1<<15,
	IFF_MACVLAN			= 1<<16,
	IFF_XMIT_DST_RELEASE_PERM	= 1<<17,
	IFF_L3MDEV_MASTER		= 1<<18,
	IFF_NO_QUEUE			= 1<<19,
	IFF_OPENVSWITCH			= 1<<20,
	IFF_L3MDEV_SLAVE		= 1<<21,
	IFF_TEAM			= 1<<22,
	IFF_RXFH_CONFIGURED		= 1<<23,
	IFF_PHONY_HEADROOM		= 1<<24,
	IFF_MACSEC			= 1<<25,
	IFF_NO_RX_HANDLER		= 1<<26,
	IFF_FAILOVER			= 1<<27,
	IFF_FAILOVER_SLAVE		= 1<<28,
	IFF_L3MDEV_RX_HANDLER		= 1<<29,
	IFF_NO_ADDRCONF			= BIT_ULL(30),
	IFF_TX_SKB_NO_LINEAR		= BIT_ULL(31),
};

struct netdev_hw_addr_list {
	struct list_head	list;
	int			count;

	/* Auxiliary tree for faster lookup on addition and deletion */
	struct rb_root		tree;
};

struct netdev_hw_addr {
	struct list_head	list;
	struct rb_node		node;
	unsigned char		addr[MAX_ADDR_LEN];
	unsigned char		type;
	bool			global_use;
	int			sync_cnt;
	int			refcount;
	int			synced;
	struct rcu_head		rcu_head;
};

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
		if (diff < 0) {
			ins_point = &parent->rb_left;
		} else if (diff > 0) {
			ins_point = &parent->rb_right;
		} else {
			if (exclusive){
				ret = -EEXIST;
				return ret;
			}

			if (global) {
				/* check if addr is already used as global */
				if (ha->global_use){
					return ret;
				}else{
					ha->global_use = true;
				}
			}
			if (sync) {
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
	return __hw_addr_add_ex(list, addr, addr_len, addr_type, false, false, 0, false);
}

/**
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
		// TODO
		//ha = list_first_entry(&netdev->dev_addrs.list, struct netdev_hw_addr, list);
		netdev->dev_addr = ha->addr;
	}
	return ret;
}

/* Initialize the core of a dummy net device.
 * The setup steps dummy netdevs need which normal netdevs get by going
 * through register_netdevice().
 */
static void init_dummy_netdev(struct net_device *netdev){
	/* make sure we BUG if trying to hit standard
	 * register/unregister code path
	 */
	netdev->reg_state = NETREG_DUMMY;

	/* a dummy interface is started by default */
	set_bit(__LINK_STATE_PRESENT, &netdev->state);
	set_bit(__LINK_STATE_START, &netdev->state);

	/* Note : We dont allocate pcpu_refcnt for dummy devices,
	 * because users of this 'device' dont need to change
	 * its refcount.
	 */
}
