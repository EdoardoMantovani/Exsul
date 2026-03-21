/* base header used for the hci related operations */

#define bt_cb(skb) ((struct bt_skb_cb *)((skb)->cb))

#define hci_skb_pkt_type(skb) bt_cb((skb))->pkt_type
#define hci_skb_pkt_status(skb) bt_cb((skb))->pkt_status
#define hci_skb_pkt_seqnum(skb) bt_cb((skb))->pkt_seqnum
#define hci_skb_expect(skb) bt_cb((skb))->expect
#define hci_skb_opcode(skb) bt_cb((skb))->hci.opcode
#define hci_skb_event(skb) bt_cb((skb))->hci.req_event
#define hci_skb_sk(skb) bt_cb((skb))->hci.sk

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

/* Default LE RPA expiry time, 15 minutes */
#define HCI_DEFAULT_RPA_TIMEOUT		(15 * 60)

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

enum {
	DISCOVERY_STOPPED,
	DISCOVERY_STARTING,
	DISCOVERY_FINDING,
	DISCOVERY_RESOLVING,
	DISCOVERY_STOPPING,
};

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
void *hci_sent_cmd_data(struct hci_dev *hdev, __u16 opcode){
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
	u8 (*classify_pkt_type)(struct hci_dev *hdev, struct sk_buff *skb) = NULL;

	if (hdev->classify_pkt_type != NULL){
		classify_pkt_type = hdev->classify_pkt_type;
		classify_pkt_type(hdev, skb);
	}
	return hci_skb_pkt_type(skb);
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
	hdev->discovery.state = DISCOVERY_STOPPED;
	INIT_LIST_HEAD(&hdev->discovery.all);
	INIT_LIST_HEAD(&hdev->discovery.unknown);
	INIT_LIST_HEAD(&hdev->discovery.resolve);
	hdev->discovery.report_invalid_rssi = true;
	hdev->discovery.rssi = HCI_RSSI_INVALID;
}

static inline void hci_discovery_filter_clear(struct hci_dev *hdev){
	hdev->discovery.result_filtering = false;
	hdev->discovery.report_invalid_rssi = true;
	hdev->discovery.rssi = HCI_RSSI_INVALID;
	hdev->discovery.uuid_count = 0;

	kfree(hdev->discovery.uuids);
	hdev->discovery.uuids =NULL;
}

struct sk_buff *hci_cmd_sync_alloc(struct hci_dev *hdev, u16 opcode, u32 plen, const void *param, struct sock *sk){
	int len = HCI_COMMAND_HDR_SIZE + plen;
	struct hci_command_hdr *hdr = NULL;
	struct sk_buff *skb = NULL;

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

	hci_skb_pkt_type(skb) = HCI_COMMAND_PKT;
	hci_skb_opcode(skb) = opcode;

	/* Grab a reference if command needs to be associated with a sock (e.g.
	 * likely mgmt socket that initiated the command).
	 */
	if (sk) {
		hci_skb_sk(skb) = sk;
		sock_hold(sk);
	}

	return skb;
}
