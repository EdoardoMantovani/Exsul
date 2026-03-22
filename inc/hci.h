/* base header used for the hci related operations */

#define bt_cb(skb) ((struct bt_skb_cb *)((skb)->cb))

#define hci_skb_pkt_type(skb) bt_cb((skb))->pkt_type
#define hci_skb_pkt_status(skb) bt_cb((skb))->pkt_status
#define hci_skb_pkt_seqnum(skb) bt_cb((skb))->pkt_seqnum
#define hci_skb_expect(skb) bt_cb((skb))->expect
#define hci_skb_opcode(skb) bt_cb((skb))->hci.opcode
#define hci_skb_event(skb) bt_cb((skb))->hci.req_event
#define hci_skb_sk(skb) bt_cb((skb))->hci.sk

/* HCI timeouts */
#define HCI_DISCONN_TIMEOUT		msecs_to_jiffies(2000)	/* 2 seconds */
#define HCI_PAIRING_TIMEOUT		msecs_to_jiffies(60000)	/* 60 seconds */
#define HCI_INIT_TIMEOUT		msecs_to_jiffies(10000)	/* 10 seconds */
#define HCI_CMD_TIMEOUT			msecs_to_jiffies(2000)	/* 2 seconds */
#define HCI_NCMD_TIMEOUT		msecs_to_jiffies(4000)	/* 4 seconds */
#define HCI_ACL_TX_TIMEOUT		msecs_to_jiffies(45000)	/* 45 seconds */
#define HCI_AUTO_OFF_TIMEOUT		msecs_to_jiffies(2000)	/* 2 seconds */
#define HCI_ACL_CONN_TIMEOUT		msecs_to_jiffies(20000)	/* 20 seconds */
#define HCI_LE_CONN_TIMEOUT		msecs_to_jiffies(20000)	/* 20 seconds */
#define HCI_ISO_TX_TIMEOUT		usecs_to_jiffies(0x7fffff) /* 8388607 usecs */

/* HCI data types */
#define HCI_COMMAND_PKT			0x01
#define HCI_ACLDATA_PKT			0x02
#define HCI_SCODATA_PKT			0x03
#define HCI_EVENT_PKT			0x04
#define HCI_ISODATA_PKT			0x05
#define HCI_DIAG_PKT			0xf0
#define HCI_DRV_PKT			0xf1
#define HCI_VENDOR_PKT			0xff

/* HCI packet types */
#define HCI_DM1				0x0008
#define HCI_DM3				0x0400
#define HCI_DM5				0x4000
#define HCI_DH1				0x0010
#define HCI_DH3				0x0800
#define HCI_DH5				0x8000

#define HCI_HV1				0x0020
#define HCI_HV2				0x0040
#define HCI_HV3				0x0080

/* eSCO packet types */
#define ESCO_HV1			0x0001
#define ESCO_HV2			0x0002
#define ESCO_HV3			0x0004
#define ESCO_EV3			0x0008
#define ESCO_EV4			0x0010
#define ESCO_EV5			0x0020
#define ESCO_2EV3			0x0040
#define ESCO_3EV3			0x0080
#define ESCO_2EV5			0x0100
#define ESCO_3EV5			0x0200

/* Bluetooth versions */
#define BLUETOOTH_VER_1_1		1
#define BLUETOOTH_VER_1_2		2
#define BLUETOOTH_VER_2_0		3
#define BLUETOOTH_VER_2_1		4
#define BLUETOOTH_VER_4_0		6

/* ---- HCI Packet structures ---- */
#define HCI_COMMAND_HDR_SIZE	 	3
#define HCI_EVENT_HDR_SIZE  		2
#define HCI_ACL_HDR_SIZE     		4
#define HCI_SCO_HDR_SIZE     		3
#define HCI_ISO_HDR_SIZE     		4

/* Reserv for core and drivers use */
#define BT_SKB_RESERVE			8

#define BTPROTO_L2CAP			0
#define BTPROTO_HCI			1
#define BTPROTO_SCO			2
#define BTPROTO_RFCOMM			3
#define BTPROTO_BNEP			4
#define BTPROTO_CMTP			5
#define BTPROTO_HIDP			6
#define BTPROTO_AVDTP			7
#define BTPROTO_ISO			8
#define BTPROTO_LAST			BTPROTO_ISO

/* Link modes */
#define HCI_LM_ACCEPT			0x8000
#define HCI_LM_MASTER			0x0001
#define HCI_LM_AUTH			0x0002
#define HCI_LM_ENCRYPT			0x0004
#define HCI_LM_TRUSTED			0x0008
#define HCI_LM_RELIABLE			0x0010
#define HCI_LM_SECURE			0x0020
#define HCI_LM_FIPS			0x0040

/* HCI data types */
#define HCI_COMMAND_PKT			0x01
#define HCI_ACLDATA_PKT			0x02
#define HCI_SCODATA_PKT			0x03
#define HCI_EVENT_PKT			0x04
#define HCI_ISODATA_PKT			0x05
#define HCI_DIAG_PKT			0xf0
#define HCI_DRV_PKT			0xf1
#define HCI_VENDOR_PKT			0xff

#define SOL_HCI				0
#define SOL_L2CAP			6
#define SOL_SCO				17
#define SOL_RFCOMM			18

#define BT_SECURITY			4

/* Baseband links */
#define SCO_LINK			0x00
#define ACL_LINK			0x01
#define ESCO_LINK			0x02
/* Low Energy links do not have defined link type. Use invented one */
#define LE_LINK				0x80
#define CIS_LINK			0x82
#define BIS_LINK			0x83
#define PA_LINK				0x84
#define INVALID_LINK			0xff

#define SMP_CMD_MAX			0x0e
#define SMP_PASSKEY_ENTRY_FAILED	0x01
#define SMP_OOB_NOT_AVAIL		0x02
#define SMP_AUTH_REQUIREMENTS		0x03
#define SMP_CONFIRM_FAILED		0x04
#define SMP_PAIRING_NOTSUPP		0x05
#define SMP_ENC_KEY_SIZE		0x06
#define SMP_CMD_NOTSUPP			0x07
#define SMP_UNSPECIFIED			0x08
#define SMP_REPEATED_ATTEMPTS		0x09
#define SMP_INVALID_PARAMS		0x0a
#define SMP_DHKEY_CHECK_FAILED		0x0b
#define SMP_NUMERIC_COMP_FAILED		0x0c
#define SMP_BREDR_PAIRING_IN_PROGRESS	0x0d
#define SMP_CROSS_TRANSP_NOT_ALLOWED	0x0e
#define SMP_KEY_REJECTED		0x0f

#define SMP_MIN_ENC_KEY_SIZE		7
#define SMP_MAX_ENC_KEY_SIZE		16

/* Default LE RPA expiry time, 15 minutes */
#define HCI_DEFAULT_RPA_TIMEOUT		(15 * 60)

#define HCI_MAX_EXT_AD_LENGTH		251

/* Management interface */
#define DISCOV_TYPE_BREDR		(BIT(BDADDR_BREDR))
#define DISCOV_TYPE_LE			(BIT(BDADDR_LE_PUBLIC) | \
					 BIT(BDADDR_LE_RANDOM))
#define DISCOV_TYPE_INTERLEAVED		(BIT(BDADDR_BREDR) | \
					 BIT(BDADDR_LE_PUBLIC) | \
					 BIT(BDADDR_LE_RANDOM))

/* These LE scan and inquiry parameters were chosen according to LE General
 * Discovery Procedure specification.
 */
#define DISCOV_LE_SCAN_WIN		0x0012 /* 11.25 msec */
#define DISCOV_LE_SCAN_INT		0x0012 /* 11.25 msec */
#define DISCOV_LE_SCAN_INT_FAST		0x0060 /* 60 msec */
#define DISCOV_LE_SCAN_WIN_FAST		0x0030 /* 30 msec */
#define DISCOV_LE_SCAN_INT_CONN		0x0060 /* 60 msec */
#define DISCOV_LE_SCAN_WIN_CONN		0x0060 /* 60 msec */
#define DISCOV_LE_SCAN_INT_SLOW1	0x0800 /* 1.28 sec */
#define DISCOV_LE_SCAN_WIN_SLOW1	0x0012 /* 11.25 msec */
#define DISCOV_LE_SCAN_INT_SLOW2	0x1000 /* 2.56 sec */
#define DISCOV_LE_SCAN_WIN_SLOW2	0x0024 /* 22.5 msec */
#define DISCOV_CODED_SCAN_INT_FAST	0x0120 /* 180 msec */
#define DISCOV_CODED_SCAN_WIN_FAST	0x0090 /* 90 msec */
#define DISCOV_CODED_SCAN_INT_SLOW1	0x1800 /* 3.84 sec */
#define DISCOV_CODED_SCAN_WIN_SLOW1	0x0036 /* 33.75 msec */
#define DISCOV_CODED_SCAN_INT_SLOW2	0x3000 /* 7.68 sec */
#define DISCOV_CODED_SCAN_WIN_SLOW2	0x006c /* 67.5 msec */
#define DISCOV_LE_TIMEOUT		10240	/* msec */
#define DISCOV_INTERLEAVED_TIMEOUT	5120	/* msec */
#define DISCOV_INTERLEAVED_INQUIRY_LEN	0x04
#define DISCOV_BREDR_INQUIRY_LEN	0x08
#define DISCOV_LE_RESTART_DELAY		msecs_to_jiffies(200)	/* msec */
#define DISCOV_LE_FAST_ADV_INT_MIN	0x00A0	/* 100 msec */
#define DISCOV_LE_FAST_ADV_INT_MAX	0x00F0	/* 150 msec */
#define DISCOV_LE_PER_ADV_INT_MIN	0x00A0	/* 200 msec */
#define DISCOV_LE_PER_ADV_INT_MAX	0x00A0	/* 200 msec */
#define DISCOV_LE_ADV_MESH_MIN		0x00A0  /* 100 msec */
#define DISCOV_LE_ADV_MESH_MAX		0x00A0  /* 100 msec */
#define INTERVAL_TO_MS(x)		(((x) * 10) / 0x10)

#define HCI_MIN_ADV_MONITOR_HANDLE		1
#define HCI_MAX_ADV_MONITOR_NUM_HANDLES		32
#define HCI_MAX_ADV_MONITOR_NUM_PATTERNS	16
#define HCI_ADV_MONITOR_EXT_NONE		1
#define HCI_ADV_MONITOR_EXT_MSFT		2

#define HCI_MAX_SHORT_NAME_LENGTH		10

#define HCI_CONN_HANDLE_MAX			0x0eff
#define HCI_CONN_HANDLE_UNSET(_handle)		(_handle > HCI_CONN_HANDLE_MAX)

/* Default min/max age of connection information (1s/3s) */
#define DEFAULT_CONN_INFO_MIN_AGE		1000
#define DEFAULT_CONN_INFO_MAX_AGE		3000

/* Min encryption key size to match with SMP */
#define HCI_MIN_ENC_KEY_SIZE			7

/* Default LE RPA expiry time, 15 minutes */
#define HCI_DEFAULT_RPA_TIMEOUT			(15 * 60)

/* Default min/max age of connection information (1s/3s) */
#define DEFAULT_CONN_INFO_MIN_AGE		1000
#define DEFAULT_CONN_INFO_MIN_AGE		1000
/* Default authenticated payload timeout 30s */
#define DEFAULT_AUTH_PAYLOAD_TIMEOUT   		0x0bb8

#define HCI_MAX_PAGES				3

/* The core spec defines 127 as the "not available" value */
#define HCI_TX_POWER_INVALID			127
#define HCI_RSSI_INVALID			127

#define HCI_LE_SET_PHY_1M			0x01
#define HCI_LE_SET_PHY_2M			0x02
#define HCI_LE_SET_PHY_CODED			0x04

#define HCI_OP_LE_SET_EXT_SCAN_PARAMS		0x2041

#define HCI_MAX_ADV_INSTANCES			5
#define HCI_DEFAULT_ADV_DURATION		2

#define HCI_ADV_TX_POWER_NO_PREFERENCE		0x7F

#define HCI_OP_WRITE_PAGE_SCAN_TYPE		0x0c47
#define PAGE_SCAN_TYPE_STANDARD			0x00
#define PAGE_SCAN_TYPE_INTERLACED		0x01
#define HCI_OP_READ_RSSI			0x1405

typedef void (*hci_req_complete_t)(struct hci_dev *hdev, u8 status, u16 opcode);
typedef void (*hci_req_complete_skb_t)(struct hci_dev *hdev, u8 status, u16 opcode, struct sk_buff *skb);

/* BD Address */
typedef struct {
	__u8 b[6];
} __packed bdaddr_t;

struct hci_command_hdr {
	__le16	opcode;		/* OCF & OGF */
	__u8	plen;
} __packed;

struct hci_event_hdr {
	__u8	evt;
	__u8	plen;
} __packed;

struct hci_acl_hdr {
	__le16	handle;		/* Handle & Flags(PB, BC) */
	__le16	dlen;
} __packed;

struct hci_sco_hdr {
	__le16	handle;
	__u8	dlen;
} __packed;

struct hci_iso_hdr {
	__le16	handle;
	__le16	dlen;
	__u8	data[];
} __packed;

struct hci_conn_hash {
	struct list_head list;
	unsigned int     acl_num;
	unsigned int     sco_num;
	unsigned int     cis_num;
	unsigned int     bis_num;
	unsigned int     pa_num;
	unsigned int     le_num;
	unsigned int     le_num_peripheral;
};

struct hci_request {
	struct hci_dev		*hdev;
	struct sk_buff_head	cmd_q;

	/* If something goes wrong when building the HCI request, the error
	 * value is stored in this field.
	 */
	int			err;
};

struct discovery_state {
	int			type;
	enum {
		DISCOVERY_STOPPED,
		DISCOVERY_STARTING,
		DISCOVERY_FINDING,
		DISCOVERY_RESOLVING,
		DISCOVERY_STOPPING,
	} state;
	struct list_head	all;	/* All devices found during inquiry */
	struct list_head	unknown;	/* Name state not known */
	struct list_head	resolve;	/* Name needs to be resolved */
	__u32			timestamp;
	bdaddr_t		last_adv_addr;
	u8			last_adv_addr_type;
	s8			last_adv_rssi;
	u32			last_adv_flags;
	u8			last_adv_data[HCI_MAX_EXT_AD_LENGTH];
	u8			last_adv_data_len;
	bool			report_invalid_rssi;
	bool			result_filtering;
	bool			limited;
	s8			rssi;
	u16			uuid_count;
	u8			(*uuids)[16];
	unsigned long		name_resolve_timeout;
	u32			lock;
};

struct hci_drv_handler {
	int (*func)(struct hci_dev *hdev, void *data, u16 data_len);
	size_t data_len;
};

struct hci_drv {
	size_t common_handler_count;
	const struct hci_drv_handler *common_handlers;

	size_t specific_handler_count;
	const struct hci_drv_handler *specific_handlers;
};

struct l2cap_ctrl {
	u8	sframe:1,
		poll:1,
		final:1,
		fcs:1,
		sar:2,
		super:2;

	u16	reqseq;
	u16	txseq;
	u8	retries;
	__le16  psm;
	bdaddr_t bdaddr;
	struct l2cap_chan *chan;
};

struct hci_ctrl {
	struct sock *sk;
	u16 opcode;
	u8 req_flags;
	u8 req_event;
	union {
		hci_req_complete_t req_complete;
		hci_req_complete_skb_t req_complete_skb;
	};
};

struct mgmt_ctrl {
	struct hci_dev *hdev;
	u16 opcode;
};

struct bt_skb_cb {
	u8 pkt_type;
	u8 force_active;
	u16 expect;
	u16 pkt_seqnum;
	u8 incoming:1;
	u8 pkt_status:2;
	union {
		struct l2cap_ctrl l2cap;
		struct hci_ctrl hci;
		struct mgmt_ctrl mgmt;
	};
};

static inline struct hci_event_hdr *hci_event_hdr(const struct sk_buff *skb){
	return (struct hci_event_hdr *) skb->data;
}

static inline struct hci_acl_hdr *hci_acl_hdr(const struct sk_buff *skb){
	return (struct hci_acl_hdr *) skb->data;
}

static inline struct hci_sco_hdr *hci_sco_hdr(const struct sk_buff *skb){
	return (struct hci_sco_hdr *) skb->data;
}

static inline struct hci_iso_hdr *hci_iso_hdr(const struct sk_buff *skb){
	return (struct hci_iso_hdr *)skb->data;
}

/* Get data from the previously sent command */
static void *hci_cmd_data(struct sk_buff *skb, __u16 opcode){
	struct hci_command_hdr *hdr = NULL;

	if (!skb || skb->len < HCI_COMMAND_HDR_SIZE){
		return NULL;
	}

	hdr = (void *)skb->data;

	if (hdr->opcode != cpu_to_le16(opcode)){
		return NULL;
	}

	return skb->data + HCI_COMMAND_HDR_SIZE;
}

/* Get data from the previously sent command */
static void *hci_sent_cmd_data(struct hci_dev *hdev, __u16 opcode){
	void *data = NULL;

	/* Check if opcode matches last sent command */
	data = hci_cmd_data(hdev->sent_cmd, opcode);
	if (data == NULL){
		/* Check if opcode matches last request */
		data = hci_cmd_data(hdev->req_skb, opcode);
	}
	return data;
}

/* Send ACL data */
static void hci_add_acl_hdr(struct sk_buff *skb, __u16 handle, __u16 flags){
	struct hci_acl_hdr *hdr = NULL;
	int len = skb->len;

	skb_push(skb, HCI_ACL_HDR_SIZE);
	skb_reset_transport_header(skb);
	hdr = (struct hci_acl_hdr *)skb_transport_header(skb);
	hdr->handle = cpu_to_le16(hci_handle_pack(handle, flags));
	hdr->dlen   = cpu_to_le16(len);
}

static u8 hci_dev_classify_pkt_type(struct hci_dev *hdev, struct sk_buff *skb){
	struct bt_skb_cb *cb = NULL;
	u8 (*classify_pkt_type)(struct hci_dev *hdev, struct sk_buff *skb) = NULL;

	if (hdev->classify_pkt_type != NULL){
		classify_pkt_type = hdev->classify_pkt_type;
		classify_pkt_type(hdev, skb);
	}
	cb = (struct bt_skb_cb *)skb->cb;
	return cb->pkt_type;
}

static inline void hci_cpu_to_le24(__u32 val, __u8 dst[3]){
	dst[0] = val & 0xff;
	dst[1] = (val & 0xff00) >> 8;
	dst[2] = (val & 0xff0000) >> 16;
}

static inline unsigned int hci_conn_num(struct hci_dev *hdev, __u8 type){
	int ret = 0;
	struct hci_conn_hash *h = NULL;

	h = &hdev->conn_hash;
	switch (type) {
	case ACL_LINK:
		return h->acl_num;
	case LE_LINK:
		return h->le_num;
	case SCO_LINK:
	case ESCO_LINK:
		return h->sco_num;
	case CIS_LINK:
		return h->cis_num;
	case BIS_LINK:
		return h->bis_num;
	case PA_LINK:
		return h->pa_num;
	default:
		return 0;
	}
}

static inline unsigned int hci_conn_count(struct hci_dev *hdev){
	struct hci_conn_hash *c = &hdev->conn_hash;

	return c->acl_num + c->sco_num + c->le_num + c->cis_num + c->bis_num + c->pa_num;
}

static inline unsigned int hci_iso_count(struct hci_dev *hdev){
	struct hci_conn_hash *c = &hdev->conn_hash;

	return c->cis_num + c->bis_num;
}

static inline struct sk_buff *bt_skb_alloc(unsigned int len, int how){
	struct sk_buff *skb = NULL;

	skb = alloc_skb(len + BT_SKB_RESERVE, how);
	if (skb != NULL){
		skb_reserve(skb, BT_SKB_RESERVE);
	}
	return skb;
}

static void hci_request_init(struct hci_request *req, struct hci_dev *hdev){
	skb_queue_head_init(&req->cmd_q);
	req->hdev = hdev;
	req->err = 0;
}

static inline void discovery_init(struct hci_dev *hdev){
	struct discovery_state *discovery = &hdev->discovery;
	discovery->state = DISCOVERY_STOPPED;
	INIT_LIST_HEAD(&discovery->all);
	INIT_LIST_HEAD(&discovery->unknown);
	INIT_LIST_HEAD(&discovery->resolve);
	discovery->report_invalid_rssi = true;
	discovery->rssi = HCI_RSSI_INVALID;
}

static inline void hci_discovery_filter_clear(struct hci_dev *hdev){
	struct discovery_state *discovery = &hdev->discovery;
	discovery->result_filtering = false;
	discovery->report_invalid_rssi = true;
	discovery->rssi = HCI_RSSI_INVALID;
	discovery->uuid_count = 0;

	kfree(discovery->uuids);
	discovery->uuids = NULL;
}

static struct sk_buff *hci_cmd_sync_alloc(struct hci_dev *hdev, u16 opcode, u32 plen, const void *param, struct sock *sk){
	int len = HCI_COMMAND_HDR_SIZE + plen;
	struct hci_command_hdr *hdr = NULL;
	struct sk_buff *skb = NULL;
	struct bt_skb_cb *cb = NULL;

	skb = bt_skb_alloc(len, GFP_ATOMIC);
	if (skb == NULL){
		return NULL;
	}

	hdr = skb_put(skb, HCI_COMMAND_HDR_SIZE);
	hdr->opcode = cpu_to_le16(opcode);
	hdr->plen   = plen;

	if (plen){
		skb_put_data(skb, param, plen);
	}

	cb = (struct bt_skb_cb *)skb->cb;
	cb->pkt_type = HCI_COMMAND_PKT;
	cb->hci.opcode = opcode;

	return skb;
}

static void hci_cmd_sync_init(struct hci_dev *hdev){
	/* still TODO */
	#if 0
	INIT_WORK(&hdev->cmd_sync_work, hci_cmd_sync_work);
	INIT_LIST_HEAD(&hdev->cmd_sync_work_list);
	mutex_init(&hdev->cmd_sync_work_lock);
	mutex_init(&hdev->unregister_lock);

	INIT_WORK(&hdev->cmd_sync_cancel_work, hci_cmd_sync_cancel_work);
	INIT_WORK(&hdev->reenable_adv_work, reenable_adv);
	INIT_DELAYED_WORK(&hdev->le_scan_disable, le_scan_disable);
	INIT_DELAYED_WORK(&hdev->adv_instance_expire, adv_timeout_expire);
	INIT_DELAYED_WORK(&hdev->interleave_scan, interleave_scan_work);
	#endif
}
