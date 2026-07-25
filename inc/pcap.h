/* generic pcap layer, used for creating/reading/writing a pcap file */

#define	LINKTYPE_CAN_SOCKETCAN			227
#define	LINKTYPE_IPV4				228
#define	LINKTYPE_IPV6				229
#define	LINKTYPE_IEEE802_15_4_NOFCS		230
#define	LINKTYPE_DBUS				231
#define	LINKTYPE_DVB_CI				235
#define	LINKTYPE_MUX27010			236
#define	LINKTYPE_STANAG_5066_D_PDU		237
#define	LINKTYPE_NFLOG				239
#define	LINKTYPE_NETANALYZER			240
#define	LINKTYPE_NETANALYZER_TRANSPARENT	241
#define	LINKTYPE_IPOIB				242
#define	LINKTYPE_MPEG_2_TS			243
#define	LINKTYPE_NG40				244
#define	LINKTYPE_NFC_LLCP			245
#define	LINKTYPE_INFINIBAND			247
#define	LINKTYPE_SCTP				248
#define	LINKTYPE_USBPCAP			249
#define	LINKTYPE_RTAC_SERIAL			250
#define	LINKTYPE_BLUETOOTH_LE_LL		251
#define	LINKTYPE_NETLINK			253
#define	LINKTYPE_BLUETOOTH_LINUX_MONITOR	254
#define	LINKTYPE_BLUETOOTH_BREDR_BB		255
#define	LINKTYPE_BLUETOOTH_LE_LL_WITH_PHDR	256
#define	LINKTYPE_PROFIBUS_DL			257
#define	LINKTYPE_PKTAP				258
#define	LINKTYPE_EPON				259
#define	LINKTYPE_IPMI_HPM_2			260
#define	LINKTYPE_ZWAVE_R1_R2			261
#define	LINKTYPE_ZWAVE_R3			262
#define	LINKTYPE_WATTSTOPPER_DLM		263
#define	LINKTYPE_ISO_14443			264
#define	LINKTYPE_RDS				265
#define	LINKTYPE_USB_DARWIN			266
#define	LINKTYPE_SDLC				268

typedef struct pcap_hdr_s {
	u32 magic_number;   /* magic number */
	u16 version_major;  /* major version number */
	u16 version_minor;  /* minor version number */
	s32 thiszone;       /* GMT to local correction */
	u32 sigfigs;        /* accuracy of timestamps */
	u32 snaplen;        /* max length of captured packets, in octets */
	u32 network;        /* data link type */
}pcap_hdr_t;

typedef struct pcaprec_hdr_s {
	u32 ts_sec;   /* timestamp seconds */
	u32 ts_usec;  /* timestamp microseconds */
	u32 incl_len; /* number of octets of packet saved in file */
	u32 orig_len; /* actual length of packet */
}pcaprec_hdr_t;

/* pcap_create: simplified version of 'https://github.com/pipe01/InfiniEmu/blob/3d9eba873ac9ea986bf60fbf07c5ef73b12de886/src/pcap.c#L38' */
static int pcap_create(void){
	int ret = 0;
	pcap_hdr_t hdr = {
		.magic_number = 0xa1b2c3d4,
		.version_major = 2,
		.version_minor = 4,
		.thiszone = 0,
		.sigfigs = 0,
		.snaplen = 65535,
		.network = LINKTYPE_IPV4,
	};

	#ifdef __linux__
	director.pcap_fd = open(PCAP_DUMP_DEFAULT_NAME, O_WRONLY | O_CREAT | O_TRUNC | O_DSYNC | O_RSYNC, 0644);
	#else
	director.pcap_fd = open(PCAP_DUMP_DEFAULT_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	#endif
	lseek(director.pcap_fd, 0, SEEK_SET);
	write(director.pcap_fd, &hdr, sizeof(hdr));
	lseek(director.pcap_fd, sizeof(hdr), SEEK_SET);
	return ret;
}

/* pcap_close: simply close the generated pcap file */
static int pcap_close(void){
	int ret = 0;

	if(director.pcap_fd < 0){
		return ret;
	}
	lseek(director.pcap_fd, 0, SEEK_SET);
	close(director.pcap_fd);
	return ret;
}

/* pcap_write_packet: simplified version of 'https://github.com/pipe01/InfiniEmu/blob/3d9eba873ac9ea986bf60fbf07c5ef73b12de886/src/pcap.c#L71' */
static int pcap_write_packet(u8 *data, int length){
	int ret = 0;
	pcaprec_hdr_t hdr = {
		.ts_sec = 0,
		.ts_usec = microseconds_now_real(),
		.incl_len = length,
		.orig_len = length,
	};

	write(director.pcap_fd, &hdr, sizeof(hdr));
	lseek(director.pcap_fd, sizeof(hdr), SEEK_SET);
	write(director.pcap_fd, data, length);
	lseek(director.pcap_fd, length, SEEK_SET);
	fsync(director.pcap_fd);
	return ret;
}
