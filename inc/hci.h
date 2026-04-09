/* base header used for the hci related operations */

#define bt_cb(skb)			((struct bt_skb_cb *)((skb)->cb))

#define hci_skb_pkt_type(skb)		bt_cb((skb))->pkt_type
#define hci_skb_pkt_status(skb)		bt_cb((skb))->pkt_status
#define hci_skb_pkt_seqnum(skb)		bt_cb((skb))->pkt_seqnum
#define hci_skb_expect(skb)		bt_cb((skb))->expect
#define hci_skb_opcode(skb)		bt_cb((skb))->hci.opcode
#define hci_skb_event(skb)		bt_cb((skb))->hci.req_event
#define hci_skb_sk(skb)			bt_cb((skb))->hci.sk

/* Command opcode pack/unpack */
#define hci_opcode_pack(ogf, ocf)	((__u16) ((ocf & 0x03ff)|(ogf << 10)))
#define hci_opcode_ogf(op)		(op >> 10)
#define hci_opcode_ocf(op)		(op & 0x03ff)

/* ACL handle and flags pack/unpack */
#define hci_handle_pack(h, f)		((__u16) ((h & 0x0fff)|(f << 12)))
#define hci_handle(h)			(h & 0x0fff)
#define hci_flags(h)			(h >> 12)

/* ISO handle and flags pack/unpack */
#define hci_iso_flags_pb(f)		(f & 0x0003)
#define hci_iso_flags_ts(f)		((f >> 2) & 0x0001)
#define hci_iso_flags_pack(pb, ts)	((pb & 0x03) | ((ts & 0x01) << 2))

/* ISO data length and flags pack/unpack */
#define hci_iso_data_len_pack(h, f)	((__u16) ((h) | ((f) << 14)))
#define hci_iso_data_len(h)		((h) & 0x3fff)
#define hci_iso_data_flags(h)		((h) >> 14)

#define hci_dev_set_flag(hdev, nr)             set_bit((nr), (hdev)->dev_flags)
#define hci_dev_clear_flag(hdev, nr)           clear_bit((nr), (hdev)->dev_flags)
#define hci_dev_change_flag(hdev, nr)          change_bit((nr), (hdev)->dev_flags)
#define hci_dev_test_flag(hdev, nr)            test_bit((nr), (hdev)->dev_flags)
#define hci_dev_test_and_set_flag(hdev, nr)    test_and_set_bit((nr), (hdev)->dev_flags)
#define hci_dev_test_and_clear_flag(hdev, nr)  test_and_clear_bit((nr), (hdev)->dev_flags)
#define hci_dev_test_and_change_flag(hdev, nr) test_and_change_bit((nr), (hdev)->dev_flags)

#define hci_set_quirk(hdev, nr)			set_bit((nr), (hdev)->quirk_flags)
#define hci_clear_quirk(hdev, nr)		clear_bit((nr), (hdev)->quirk_flags)
#define hci_test_quirk(hdev, nr)		test_bit((nr), (hdev)->quirk_flags)

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

/* Driver specific OGF (Opcode Group Field)
 * Commands in this group may have different meanings across different drivers.
 */
#define HCI_DRV_OGF_DRIVER_SPECIFIC	0x01

#define HCI_REQ_START			BIT(0)
#define HCI_REQ_SKB			BIT(1)

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

#define HCI_REQ_DONE			0
#define HCI_REQ_PEND			1
#define HCI_REQ_CANCELED		2

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

#define HCI_DRV_STATUS_SUCCESS			0x00
#define HCI_DRV_STATUS_UNSPECIFIED_ERROR	0x01
#define HCI_DRV_STATUS_UNKNOWN_COMMAND		0x02
#define HCI_DRV_STATUS_INVALID_PARAMETERS	0x03

#define HCI_DRV_MAX_DRIVER_NAME_LENGTH		32

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

#define HCI_LINK_KEY_SIZE			16
#define HCI_MAX_CPB_DATA_SIZE			252

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

/* HCI device flags */
enum {
	HCI_UP,
	HCI_INIT,
	HCI_RUNNING,

	HCI_PSCAN,
	HCI_ISCAN,
	HCI_AUTH,
	HCI_ENCRYPT,
	HCI_INQUIRY,

	HCI_RAW,

	HCI_RESET,
};

enum hci_devcoredump_pkt_type {
	HCI_DEVCOREDUMP_PKT_INIT,
	HCI_DEVCOREDUMP_PKT_SKB,
	HCI_DEVCOREDUMP_PKT_PATTERN,
	HCI_DEVCOREDUMP_PKT_COMPLETE,
	HCI_DEVCOREDUMP_PKT_ABORT,
};

/* Bitmask of connection flags */
enum hci_conn_flags {
	HCI_CONN_FLAG_REMOTE_WAKEUP = BIT(0),
	HCI_CONN_FLAG_DEVICE_PRIVACY = BIT(1),
	HCI_CONN_FLAG_ADDRESS_RESOLUTION = BIT(2),
	HCI_CONN_FLAG_PAST = BIT(3),
};

/*
 * BR/EDR and/or LE controller flags: the flags defined here should represent
 * states from the controller.
 */
enum {
	HCI_SETUP,
	HCI_CONFIG,
	HCI_DEBUGFS_CREATED,
	HCI_POWERING_DOWN,
	HCI_AUTO_OFF,
	HCI_RFKILLED,
	HCI_MGMT,
	HCI_BONDABLE,
	HCI_SERVICE_CACHE,
	HCI_KEEP_DEBUG_KEYS,
	HCI_USE_DEBUG_KEYS,
	HCI_UNREGISTER,
	HCI_UNCONFIGURED,
	HCI_USER_CHANNEL,
	HCI_EXT_CONFIGURED,
	HCI_LE_ADV,
	HCI_LE_ADV_0,
	HCI_LE_PER_ADV,
	HCI_LE_SCAN,
	HCI_SSP_ENABLED,
	HCI_SC_ENABLED,
	HCI_SC_ONLY,
	HCI_PRIVACY,
	HCI_LIMITED_PRIVACY,
	HCI_RPA_EXPIRED,
	HCI_RPA_RESOLVING,
	HCI_LE_ENABLED,
	HCI_ADVERTISING,
	HCI_ADVERTISING_CONNECTABLE,
	HCI_CONNECTABLE,
	HCI_DISCOVERABLE,
	HCI_LIMITED_DISCOVERABLE,
	HCI_LINK_SECURITY,
	HCI_PERIODIC_INQ,
	HCI_FAST_CONNECTABLE,
	HCI_BREDR_ENABLED,
	HCI_LE_SCAN_INTERRUPTED,
	HCI_WIDEBAND_SPEECH_ENABLED,
	HCI_EVENT_FILTER_CONFIGURED,
	HCI_PA_SYNC,
	HCI_SCO_FLOWCTL,

	HCI_DUT_MODE,
	HCI_VENDOR_DIAG,
	HCI_FORCE_BREDR_SMP,
	HCI_FORCE_STATIC_ADDR,
	HCI_LL_RPA_RESOLUTION,
	HCI_CMD_PENDING,
	HCI_FORCE_NO_MITM,
	HCI_QUALITY_REPORT,
	HCI_OFFLOAD_CODECS_ENABLED,
	HCI_LE_SIMULTANEOUS_ROLES,
	HCI_CMD_DRAIN_WORKQUEUE,

	HCI_MESH_EXPERIMENTAL,
	HCI_MESH,
	HCI_MESH_SENDING,

	__HCI_NUM_FLAGS,
};

/* HCI device quirks */
enum {
	/* When this quirk is set, the HCI Reset command is send when
	 * closing the transport instead of when opening it.
	 *
	 * This quirk must be set before hci_register_dev is called.
	 */
	HCI_QUIRK_RESET_ON_CLOSE,

	/* When this quirk is set, the device is turned into a raw-only
	 * device and it will stay in unconfigured state.
	 *
	 * This quirk must be set before hci_register_dev is called.
	 */
	HCI_QUIRK_RAW_DEVICE,

	/* When this quirk is set, the buffer sizes reported by
	 * HCI Read Buffer Size command are corrected if invalid.
	 *
	 * This quirk must be set before hci_register_dev is called.
	 */
	HCI_QUIRK_FIXUP_BUFFER_SIZE,

	/* When this quirk is set, then a controller that does not
	 * indicate support for Inquiry Result with RSSI is assumed to
	 * support it anyway. Some early Bluetooth 1.2 controllers had
	 * wrongly configured local features that will require forcing
	 * them to enable this mode. Getting RSSI information with the
	 * inquiry responses is preferred since it allows for a better
	 * user experience.
	 *
	 * This quirk must be set before hci_register_dev is called.
	 */
	HCI_QUIRK_FIXUP_INQUIRY_MODE,

	/* When this quirk is set, then the HCI Read Local Supported
	 * Commands command is not supported. In general Bluetooth 1.2
	 * and later controllers should support this command. However
	 * some controllers indicate Bluetooth 1.2 support, but do
	 * not support this command.
	 *
	 * This quirk must be set before hci_register_dev is called.
	 */
	HCI_QUIRK_BROKEN_LOCAL_COMMANDS,

	/* When this quirk is set, then no stored link key handling
	 * is performed. This is mainly due to the fact that the
	 * HCI Delete Stored Link Key command is advertised, but
	 * not supported.
	 *
	 * This quirk must be set before hci_register_dev is called.
	 */
	HCI_QUIRK_BROKEN_STORED_LINK_KEY,

	/* When this quirk is set, an external configuration step
	 * is required and will be indicated with the controller
	 * configuration.
	 *
	 * This quirk can be set before hci_register_dev is called or
	 * during the hdev->setup vendor callback.
	 */
	HCI_QUIRK_EXTERNAL_CONFIG,

	/* When this quirk is set, the public Bluetooth address
	 * initially reported by HCI Read BD Address command
	 * is considered invalid. Controller configuration is
	 * required before this device can be used.
	 *
	 * This quirk can be set before hci_register_dev is called or
	 * during the hdev->setup vendor callback.
	 */
	HCI_QUIRK_INVALID_BDADDR,

	/* When this quirk is set, the public Bluetooth address
	 * initially reported by HCI Read BD Address command
	 * is considered invalid. The public BD Address can be
	 * specified in the fwnode property 'local-bd-address'.
	 * If this property does not exist or is invalid controller
	 * configuration is required before this device can be used.
	 *
	 * This quirk can be set before hci_register_dev is called or
	 * during the hdev->setup vendor callback.
	 */
	HCI_QUIRK_USE_BDADDR_PROPERTY,

	/* When this quirk is set, the Bluetooth Device Address provided by
	 * the 'local-bd-address' fwnode property is incorrectly specified in
	 * big-endian order.
	 *
	 * This quirk can be set before hci_register_dev is called or
	 * during the hdev->setup vendor callback.
	 */
	HCI_QUIRK_BDADDR_PROPERTY_BROKEN,

	/* When this quirk is set, the duplicate filtering during
	 * scanning is based on Bluetooth devices addresses. To allow
	 * RSSI based updates, restart scanning if needed.
	 *
	 * This quirk can be set before hci_register_dev is called or
	 * during the hdev->setup vendor callback.
	 */
	HCI_QUIRK_STRICT_DUPLICATE_FILTER,

	/* When this quirk is set, LE scan and BR/EDR inquiry is done
	 * simultaneously, otherwise it's interleaved.
	 *
	 * This quirk can be set before hci_register_dev is called or
	 * during the hdev->setup vendor callback.
	 */
	HCI_QUIRK_SIMULTANEOUS_DISCOVERY,

	/* When this quirk is set, the enabling of diagnostic mode is
	 * not persistent over HCI Reset. Every time the controller
	 * is brought up it needs to be reprogrammed.
	 *
	 * This quirk can be set before hci_register_dev is called or
	 * during the hdev->setup vendor callback.
	 */
	HCI_QUIRK_NON_PERSISTENT_DIAG,

	/* When this quirk is set, setup() would be run after every
	 * open() and not just after the first open().
	 *
	 * This quirk can be set before hci_register_dev is called or
	 * during the hdev->setup vendor callback.
	 *
	 */
	HCI_QUIRK_NON_PERSISTENT_SETUP,

	/* When this quirk is set, wide band speech is supported by
	 * the driver since no reliable mechanism exist to report
	 * this from the hardware, a driver flag is use to convey
	 * this support
	 *
	 * This quirk must be set before hci_register_dev is called.
	 */
	HCI_QUIRK_WIDEBAND_SPEECH_SUPPORTED,

	/* When this quirk is set consider Sync Flow Control as supported by
	 * the driver.
	 *
	 * This quirk must be set before hci_register_dev is called.
	 */
	HCI_QUIRK_SYNC_FLOWCTL_SUPPORTED,

	/* When this quirk is set, the LE states reported through the
	 * HCI_LE_READ_SUPPORTED_STATES are invalid/broken.
	 *
	 * This mechanism is necessary as many controllers have been seen has
	 * having trouble initiating a connectable advertisement despite the
	 * state combination being reported as supported.
	 *
	 * This quirk can be set before hci_register_dev is called or
	 * during the hdev->setup vendor callback.
	 */
	HCI_QUIRK_BROKEN_LE_STATES,

	/* When this quirk is set, then erroneous data reporting
	 * is ignored. This is mainly due to the fact that the HCI
	 * Read Default Erroneous Data Reporting command is advertised,
	 * but not supported; these controllers often reply with unknown
	 * command and tend to lock up randomly. Needing a hard reset.
	 *
	 * This quirk can be set before hci_register_dev is called or
	 * during the hdev->setup vendor callback.
	 */
	HCI_QUIRK_BROKEN_ERR_DATA_REPORTING,

	/*
	 * When this quirk is set, then the hci_suspend_notifier is not
	 * registered. This is intended for devices which drop completely
	 * from the bus on system-suspend and which will show up as a new
	 * HCI after resume.
	 */
	HCI_QUIRK_NO_SUSPEND_NOTIFIER,

	/*
	 * When this quirk is set, LE tx power is not queried on startup
	 * and the min/max tx power values default to HCI_TX_POWER_INVALID.
	 *
	 * This quirk can be set before hci_register_dev is called or
	 * during the hdev->setup vendor callback.
	 */
	HCI_QUIRK_BROKEN_READ_TRANSMIT_POWER,

	/* When this quirk is set, HCI_OP_SET_EVENT_FLT requests with
	 * HCI_FLT_CLEAR_ALL are ignored and event filtering is
	 * completely avoided. A subset of the CSR controller
	 * clones struggle with this and instantly lock up.
	 *
	 * Note that devices using this must (separately) disable
	 * runtime suspend, because event filtering takes place there.
	 */
	HCI_QUIRK_BROKEN_FILTER_CLEAR_ALL,

	/*
	 * When this quirk is set, disables the use of
	 * HCI_OP_ENHANCED_SETUP_SYNC_CONN command to setup SCO connections.
	 *
	 * This quirk can be set before hci_register_dev is called or
	 * during the hdev->setup vendor callback.
	 */
	HCI_QUIRK_BROKEN_ENHANCED_SETUP_SYNC_CONN,

	/*
	 * When this quirk is set, the HCI_OP_LE_SET_EXT_SCAN_ENABLE command is
	 * disabled. This is required for some Broadcom controllers which
	 * erroneously claim to support extended scanning.
	 *
	 * This quirk can be set before hci_register_dev is called or
	 * during the hdev->setup vendor callback.
	 */
	HCI_QUIRK_BROKEN_EXT_SCAN,

	/*
	 * When this quirk is set, the HCI_OP_GET_MWS_TRANSPORT_CONFIG command is
	 * disabled. This is required for some Broadcom controllers which
	 * erroneously claim to support MWS Transport Layer Configuration.
	 *
	 * This quirk can be set before hci_register_dev is called or
	 * during the hdev->setup vendor callback.
	 */
	HCI_QUIRK_BROKEN_MWS_TRANSPORT_CONFIG,

	/* When this quirk is set, max_page for local extended features
	 * is set to 1, even if controller reports higher number. Some
	 * controllers (e.g. RTL8723CS) report more pages, but they
	 * don't actually support features declared there.
	 */
	HCI_QUIRK_BROKEN_LOCAL_EXT_FEATURES_PAGE_2,

	/*
	 * When this quirk is set, the HCI_OP_LE_SET_RPA_TIMEOUT command is
	 * skipped during initialization. This is required for the Actions
	 * Semiconductor ATS2851 based controllers, which erroneously claims
	 * to support it.
	 */
	HCI_QUIRK_BROKEN_SET_RPA_TIMEOUT,

	/*
	 * When this quirk is set, the HCI_OP_LE_EXT_CREATE_CONN command is
	 * disabled. This is required for the Actions Semiconductor ATS2851
	 * based controllers, which erroneously claims to support it.
	 */
	HCI_QUIRK_BROKEN_EXT_CREATE_CONN,

	/*
	 * When this quirk is set, the command WRITE_AUTH_PAYLOAD_TIMEOUT is
	 * skipped. This is required for the Actions Semiconductor ATS2851
	 * based controllers, due to a race condition in pairing process.
	 */
	HCI_QUIRK_BROKEN_WRITE_AUTH_PAYLOAD_TIMEOUT,

	/* When this quirk is set, MSFT extension monitor tracking by
	 * address filter is supported. Since tracking quantity of each
	 * pattern is limited, this feature supports tracking multiple
	 * devices concurrently if controller supports multiple
	 * address filters.
	 *
	 * This quirk must be set before hci_register_dev is called.
	 */
	HCI_QUIRK_USE_MSFT_EXT_ADDRESS_FILTER,

	/*
	 * When this quirk is set, LE Coded PHY shall not be used. This is
	 * required for some Intel controllers which erroneously claim to
	 * support it but it causes problems with extended scanning.
	 *
	 * This quirk can be set before hci_register_dev is called or
	 * during the hdev->setup vendor callback.
	 */
	HCI_QUIRK_BROKEN_LE_CODED,

	/*
	 * When this quirk is set, the HCI_OP_READ_ENC_KEY_SIZE command is
	 * skipped during an HCI_EV_ENCRYPT_CHANGE event. This is required
	 * for Actions Semiconductor ATS2851 based controllers, which erroneously
	 * claim to support it.
	 */
	HCI_QUIRK_BROKEN_READ_ENC_KEY_SIZE,

	/*
	 * When this quirk is set, the reserved bits of Primary/Secondary_PHY
	 * inside the LE Extended Advertising Report events are discarded.
	 * This is required for some Apple/Broadcom controllers which
	 * abuse these reserved bits for unrelated flags.
	 *
	 * This quirk can be set before hci_register_dev is called or
	 * during the hdev->setup vendor callback.
	 */
	HCI_QUIRK_FIXUP_LE_EXT_ADV_REPORT_PHY,

	/* When this quirk is set, the HCI_OP_READ_VOICE_SETTING command is
	 * skipped. This is required for a subset of the CSR controller clones
	 * which erroneously claim to support it.
	 *
	 * This quirk must be set before hci_register_dev is called.
	 */
	HCI_QUIRK_BROKEN_READ_VOICE_SETTING,

	/* When this quirk is set, the HCI_OP_READ_PAGE_SCAN_TYPE command is
	 * skipped. This is required for a subset of the CSR controller clones
	 * which erroneously claim to support it.
	 *
	 * This quirk must be set before hci_register_dev is called.
	 */
	HCI_QUIRK_BROKEN_READ_PAGE_SCAN_TYPE,

	__HCI_NUM_QUIRKS,
};

typedef void (*hci_req_complete_t)(struct hci_dev *hdev, u8 status, u16 opcode);
typedef void (*hci_req_complete_skb_t)(struct hci_dev *hdev, u8 status, u16 opcode, struct sk_buff *skb);

typedef void (*coredump_t)(struct hci_dev *hdev);
typedef void (*dmp_hdr_t)(struct hci_dev *hdev, struct sk_buff *skb);
typedef void (*notify_change_t)(struct hci_dev *hdev, int state);

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

struct hci_conn {
	struct list_head list;
	/* ... */
};

/* ----- HCI callbacks ----- */
struct hci_cb {
	struct list_head list;

	char *name;

	void (*connect_cfm)	(struct hci_conn *conn, __u8 status);
	void (*disconn_cfm)	(struct hci_conn *conn, __u8 status);
	void (*security_cfm)	(struct hci_conn *conn, __u8 status, __u8 encrypt);
	void (*key_change_cfm)	(struct hci_conn *conn, __u8 status);
	void (*role_switch_cfm)	(struct hci_conn *conn, __u8 status, __u8 role);
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

struct hci_devcoredump_skb_cb {
	u16 pkt_type;
};

struct hci_devcoredump_skb_pattern {
	u8 pattern;
	u32 len;
} __packed;

/* struct hci_devcoredump - Devcoredump state
 *
 * @supported: Indicates if FW dump collection is supported by driver
 * @state: Current state of dump collection
 * @timeout: Indicates a timeout for collecting the devcoredump
 *
 * @alloc_size: Total size of the dump
 * @head: Start of the dump
 * @tail: Pointer to current end of dump
 * @end: head + alloc_size for easy comparisons
 *
 * @dump_q: Dump queue for state machine to process
 * @dump_rx: Devcoredump state machine work
 * @dump_timeout: Devcoredump timeout work
 *
 * @coredump: Called from the driver's .coredump() function.
 * @dmp_hdr: Create a dump header to identify controller/fw/driver info
 * @notify_change: Notify driver when devcoredump state has changed
 */
struct hci_devcoredump {
	bool		supported;

	enum devcoredump_state {
		HCI_DEVCOREDUMP_IDLE,
		HCI_DEVCOREDUMP_ACTIVE,
		HCI_DEVCOREDUMP_DONE,
		HCI_DEVCOREDUMP_ABORT,
		HCI_DEVCOREDUMP_TIMEOUT,
	} state;

	unsigned long	timeout;

	size_t		alloc_size;
	char		*head;
	char		*tail;
	char		*end;

	struct sk_buff_head	dump_q;
	struct work_struct	dump_rx;
	struct delayed_work	dump_timeout;

	coredump_t		coredump;
	dmp_hdr_t		dmp_hdr;
	notify_change_t		notify_change;
};

/* -----  HCI Commands ---- */
#define HCI_OP_NOP				0x0000
#define HCI_OP_INQUIRY				0x0401
struct hci_cp_inquiry {
	__u8     lap[3];
	__u8     length;
	__u8     num_rsp;
} __packed;

#define HCI_OP_INQUIRY_CANCEL			0x0402

#define HCI_OP_PERIODIC_INQ			0x0403

#define HCI_OP_EXIT_PERIODIC_INQ		0x0404

#define HCI_OP_CREATE_CONN			0x0405
struct hci_cp_create_conn {
	bdaddr_t bdaddr;
	__le16   pkt_type;
	__u8     pscan_rep_mode;
	__u8     pscan_mode;
	__le16   clock_offset;
	__u8     role_switch;
} __packed;

#define HCI_OP_DISCONNECT			0x0406
struct hci_cp_disconnect {
	__le16   handle;
	__u8     reason;
} __packed;

#define HCI_OP_ADD_SCO				0x0407
struct hci_cp_add_sco {
	__le16   handle;
	__le16   pkt_type;
} __packed;

#define HCI_OP_CREATE_CONN_CANCEL		0x0408
struct hci_cp_create_conn_cancel {
	bdaddr_t bdaddr;
} __packed;

#define HCI_OP_ACCEPT_CONN_REQ			0x0409
struct hci_cp_accept_conn_req {
	bdaddr_t bdaddr;
	__u8     role;
} __packed;

#define HCI_OP_REJECT_CONN_REQ			0x040a
struct hci_cp_reject_conn_req {
	bdaddr_t bdaddr;
	__u8     reason;
} __packed;

#define HCI_OP_LINK_KEY_REPLY			0x040b
struct hci_cp_link_key_reply {
	bdaddr_t bdaddr;
	__u8     link_key[HCI_LINK_KEY_SIZE];
} __packed;

#define HCI_OP_LINK_KEY_NEG_REPLY		0x040c
struct hci_cp_link_key_neg_reply {
	bdaddr_t bdaddr;
} __packed;

#define HCI_OP_PIN_CODE_REPLY			0x040d
struct hci_cp_pin_code_reply {
	bdaddr_t bdaddr;
	__u8     pin_len;
	__u8     pin_code[16];
} __packed;
struct hci_rp_pin_code_reply {
	__u8     status;
	bdaddr_t bdaddr;
} __packed;

#define HCI_OP_PIN_CODE_NEG_REPLY		0x040e
struct hci_cp_pin_code_neg_reply {
	bdaddr_t bdaddr;
} __packed;
struct hci_rp_pin_code_neg_reply {
	__u8     status;
	bdaddr_t bdaddr;
} __packed;

#define HCI_OP_CHANGE_CONN_PTYPE		0x040f
struct hci_cp_change_conn_ptype {
	__le16   handle;
	__le16   pkt_type;
} __packed;

#define HCI_OP_AUTH_REQUESTED			0x0411
struct hci_cp_auth_requested {
	__le16   handle;
} __packed;

#define HCI_OP_SET_CONN_ENCRYPT			0x0413
struct hci_cp_set_conn_encrypt {
	__le16   handle;
	__u8     encrypt;
} __packed;

#define HCI_OP_CHANGE_CONN_LINK_KEY		0x0415
struct hci_cp_change_conn_link_key {
	__le16   handle;
} __packed;

#define HCI_OP_REMOTE_NAME_REQ			0x0419
struct hci_cp_remote_name_req {
	bdaddr_t bdaddr;
	__u8     pscan_rep_mode;
	__u8     pscan_mode;
	__le16   clock_offset;
} __packed;

#define HCI_OP_REMOTE_NAME_REQ_CANCEL		0x041a
struct hci_cp_remote_name_req_cancel {
	bdaddr_t bdaddr;
} __packed;

struct hci_rp_remote_name_req_cancel {
	__u8     status;
	bdaddr_t bdaddr;
} __packed;

#define HCI_OP_READ_REMOTE_FEATURES		0x041b
struct hci_cp_read_remote_features {
	__le16   handle;
} __packed;

#define HCI_OP_READ_REMOTE_EXT_FEATURES		0x041c
struct hci_cp_read_remote_ext_features {
	__le16   handle;
	__u8     page;
} __packed;

#define HCI_OP_READ_REMOTE_VERSION		0x041d
struct hci_cp_read_remote_version {
	__le16   handle;
} __packed;

#define HCI_OP_READ_CLOCK_OFFSET		0x041f
struct hci_cp_read_clock_offset {
	__le16   handle;
} __packed;

#define HCI_OP_SETUP_SYNC_CONN			0x0428
struct hci_cp_setup_sync_conn {
	__le16   handle;
	__le32   tx_bandwidth;
	__le32   rx_bandwidth;
	__le16   max_latency;
	__le16   voice_setting;
	__u8     retrans_effort;
	__le16   pkt_type;
} __packed;

#define HCI_OP_ACCEPT_SYNC_CONN_REQ		0x0429
struct hci_cp_accept_sync_conn_req {
	bdaddr_t bdaddr;
	__le32   tx_bandwidth;
	__le32   rx_bandwidth;
	__le16   max_latency;
	__le16   content_format;
	__u8     retrans_effort;
	__le16   pkt_type;
} __packed;

#define HCI_OP_REJECT_SYNC_CONN_REQ		0x042a
struct hci_cp_reject_sync_conn_req {
	bdaddr_t bdaddr;
	__u8     reason;
} __packed;

#define HCI_OP_IO_CAPABILITY_REPLY		0x042b
struct hci_cp_io_capability_reply {
	bdaddr_t bdaddr;
	__u8     capability;
	__u8     oob_data;
	__u8     authentication;
} __packed;

#define HCI_OP_USER_CONFIRM_REPLY		0x042c
struct hci_cp_user_confirm_reply {
	bdaddr_t bdaddr;
} __packed;
struct hci_rp_user_confirm_reply {
	__u8     status;
	bdaddr_t bdaddr;
} __packed;

#define HCI_OP_USER_CONFIRM_NEG_REPLY		0x042d

#define HCI_OP_USER_PASSKEY_REPLY		0x042e
struct hci_cp_user_passkey_reply {
	bdaddr_t bdaddr;
	__le32	passkey;
} __packed;

#define HCI_OP_USER_PASSKEY_NEG_REPLY		0x042f

#define HCI_OP_REMOTE_OOB_DATA_REPLY		0x0430
struct hci_cp_remote_oob_data_reply {
	bdaddr_t bdaddr;
	__u8     hash[16];
	__u8     rand[16];
} __packed;

#define HCI_OP_REMOTE_OOB_DATA_NEG_REPLY	0x0433
struct hci_cp_remote_oob_data_neg_reply {
	bdaddr_t bdaddr;
} __packed;

#define HCI_OP_IO_CAPABILITY_NEG_REPLY		0x0434
struct hci_cp_io_capability_neg_reply {
	bdaddr_t bdaddr;
	__u8     reason;
} __packed;

#define HCI_OP_ENHANCED_SETUP_SYNC_CONN		0x043d
struct hci_coding_format {
	__u8	id;
	__le16	cid;
	__le16	vid;
} __packed;

struct hci_drv_cmd_hdr {
	__le16	opcode;
	__le16	len;
} __packed;

struct hci_drv_ev_hdr {
	__le16	opcode;
	__le16	len;
} __packed;

#define HCI_DRV_EV_CMD_STATUS			0x0000
struct hci_drv_ev_cmd_status {
	__le16	opcode;
	__u8	status;
} __packed;

#define HCI_DRV_EV_CMD_COMPLETE			0x0001
struct hci_drv_ev_cmd_complete {
	__le16	opcode;
	__u8	status;
	__u8	data[];
} __packed;

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

static inline __u8 hci_conn_lookup_type(struct hci_dev *hdev, __u16 handle){
	__u8 type = INVALID_LINK;
	struct hci_conn *hci_conn = NULL;
	struct hci_conn_hash *h = NULL;

	h = &hdev->conn_hash;
	// TODO
	#if 0
	list_for_each_entry_rcu(hci_conn, &h->list, list) {
		if (hci_conn->handle == handle) {
			type = hci_conn->type;
			break;
		}
	}
	#endif
	return type;
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

static int hci_send_frame(struct hci_dev *hdev, struct sk_buff *skb){
	int err;
	int (*hci_send)(struct hci_dev *hdev, struct sk_buff *skb) = NULL;
	struct bt_skb_cb *bt_skb_cb = NULL;


	bt_skb_cb = (struct bt_skb_cb *)skb->cb;
	hci_send = hdev->send;

	printf("%s type %d len %d", hdev->name, bt_skb_cb->pkt_type, skb->len);

	/* Time stamp */
	__net_timestamp(skb);

	/* decide if enable a monitor-mode like function here or not. */
	#if 0
	/* Send copy to monitor */
	hci_send_to_monitor(hdev, skb);

	if (atomic_load(&hdev->promisc)) {
		/* Send copy to the sockets */
		hci_send_to_sock(hdev, skb);
	}
	#endif

	/* Get rid of skb owner, prior to sending to the driver. */
	skb_orphan(skb);

	if (!test_bit(HCI_RUNNING, &hdev->flags)) {
		kfree_skb(skb);
		return -EINVAL;
	}

	if (bt_skb_cb->pkt_type == HCI_DRV_PKT) {
		/* Intercept HCI Drv packet here and don't go with hdev->send
		 * callback.
		 */
		err = hci_drv_process_cmd(hdev, skb);
		kfree_skb(skb);
		return err;
	}

	err = hci_send(hdev, skb);
	if (err < 0) {
		printf("[hdev] sending frame failed (%d)", err);
		kfree_skb(skb);
		return err;
	}

	return 0;
}

static void hci_cmd_sync_add(struct hci_request *req, u16 opcode, u32 plen, const void *param, u8 event, struct sock *sk){
	struct sk_buff *skb = NULL;
	struct hci_dev *hdev = NULL;
	struct bt_skb_cb *bt_skb_cb = NULL;

	hdev = req->hdev;
	printf("[hdev] opcode 0x%4.4x plen %d", opcode, plen);

	/* If an error occurred during request building, there is no point in
	 * queueing the HCI command. We can simply return.
	 */
	if (req->err){
		return;
	}
	skb = hci_cmd_sync_alloc(hdev, opcode, plen, param, sk);
	if (skb == NULL) {
		printf("[hdev] no memory for command (opcode 0x%4.4x)", opcode);
		req->err = -ENOMEM;
		return;
	}
	bt_skb_cb = (struct bt_skb_cb *)skb->cb;
	if (skb_queue_empty(&req->cmd_q)){
		bt_skb_cb->hci.req_flags |= HCI_REQ_START;
	}
	bt_skb_cb->hci.req_event = event;

	skb_queue_tail(&req->cmd_q, skb);
}

static void hci_cmd_sync_complete(struct hci_dev *hdev, u8 result, u16 opcode, struct sk_buff *skb){
	struct sock *sk = NULL;
	struct bt_skb_cb *bt_skb_cb = NULL;

	printf("[hdev] result 0x%2.2x", result);

	if (hdev->req_status != HCI_REQ_PEND){
		return;
	}
	hdev->req_result = result;
	hdev->req_status = HCI_REQ_DONE;

	/* Free the request command so it is not used as response */
	kfree_skb(hdev->req_skb);
	hdev->req_skb = NULL;

	if (skb) {
		bt_skb_cb = (struct bt_skb_cb *)skb->cb;
		sk = bt_skb_cb->hci.sk;

		/* Drop sk reference if set */
		if (sk != NULL){
			sock_put(sk);
		}
		hdev->req_rsp = skb_get(skb);
	}

	wake_up_interruptible(&hdev->req_wait_q);
}

static int hci_req_sync_run(struct hci_request *req){
	struct sk_buff *skb = NULL;
	struct hci_dev *hdev = NULL;
	struct bt_skb_cb *bt_skb_cb = NULL;

	hdev = req->hdev;
	printf("[hdev] length %u", skb_queue_len(&req->cmd_q));

	/* If an error occurred during request building, remove all HCI
	 * commands queued on the HCI request queue.
	 */
	if (req->err) {
		skb_queue_purge(&req->cmd_q);
		return req->err;
	}

	/* Do not allow empty requests */
	if (skb_queue_empty(&req->cmd_q)){
		return -ENODATA;
	}
	skb = skb_peek_tail(&req->cmd_q);
	bt_skb_cb = skb->cb;
	bt_skb_cb->hci.req_complete_skb = hci_cmd_sync_complete;
	bt_skb_cb->hci.req_flags |= HCI_REQ_SKB;

	skb_queue_splice_tail(&req->cmd_q, &hdev->cmd_q);

	queue_work(hdev->workqueue, &hdev->cmd_work);

	return 0;
}
