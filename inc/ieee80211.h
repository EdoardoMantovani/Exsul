#ifndef __IEEE80211_H__
#define __IEEE80211_H__

#ifdef __linux__
#include <openssl/evp.h>
#include <openssl/hmac.h>
#undef sk_free
#endif

#ifndef IFNAMSIZ
#define IFNAMSIZ 16
#endif

/* macros used for the generic wpa-supplicant authentication */
#define CONNECTION_EAPOL_VERSION                2
#define CONNECTION_EAPOL_TYPE_KEY               3
#define CONNECTION_EAPOL_KEY_DESC_RSN           2
#define CONNECTION_EAPOL_KEY_FIXED_LEN          95
#define CONNECTION_EAPOL_KEY_MIN_LEN            (4 + CONNECTION_EAPOL_KEY_FIXED_LEN)
#define CONNECTION_EAPOL_KEY_INFO_VERSION_MASK  0x0007
#define CONNECTION_EAPOL_KEY_INFO_PAIRWISE      0x0008
#define CONNECTION_EAPOL_KEY_INFO_INSTALL       0x0040
#define CONNECTION_EAPOL_KEY_INFO_ACK           0x0080
#define CONNECTION_EAPOL_KEY_INFO_MIC           0x0100
#define CONNECTION_EAPOL_KEY_INFO_SECURE        0x0200
#define CONNECTION_EAPOL_KEY_INFO_ERROR         0x0400
#define CONNECTION_EAPOL_KEY_INFO_REQUEST       0x0800
#define CONNECTION_EAPOL_KEY_INFO_ENCRYPTED     0x1000
#define CONNECTION_STA_BASE_FALLBACK_SIZE       1024
#define CONNECTION_STA_PRIV_FALLBACK_SIZE       4096
#define CONNECTION_STA_PRIV_MAX_SIZE            65536
#define CONNECTION_STA_ALLOC_FALLBACK_SIZE      8192

#define EXSUL_FIF_BCN_PRBRESP_PROMISC		(1U << 4)
#define EXSUL_FIF_OTHER_BSS			(1U << 6)
#define EXSUL_FIF_PROBE_REQ			(1U << 8)
#define EXSUL_RX_DUMP_FILTER_FLAGS		(EXSUL_FIF_BCN_PRBRESP_PROMISC | EXSUL_FIF_OTHER_BSS)

/*
 * The maximum number of IPv4 addresses listed for ARP filtering. If the number
 * of addresses for an interface increase beyond this value, hardware ARP
 * filtering will be disabled.
 */
#define IEEE80211_BSS_ARP_ADDR_LIST_LEN		4

#define WLAN_SA_QUERY_TR_ID_LEN			2
#define WLAN_MEMBERSHIP_LEN			8
#define WLAN_USER_POSITION_LEN			16

#define NUM_DEFAULT_KEYS			4
#define NUM_DEFAULT_MGMT_KEYS			2
#define NUM_DEFAULT_BEACON_KEYS			2
#define INVALID_PTK_KEYIDX			2 /* Keyidx always pointing to a NULL key for PTK */

#define IEEE80211_MAX_CNTDWN_COUNTERS_NUM 	2

#define IEEE80211_LINK_UNSPECIFIED		0xf
#define IEEE80211_TX_CTRL_MLO_LINK_UNSPEC	u32_encode_bits(IEEE80211_LINK_UNSPECIFIED, IEEE80211_TX_CTRL_MLO_LINK)

#define WLAN_AUTH_CHALLENGE_LEN			128
#define WLAN_CAPABILITY_ESS			(1<<0)
#define WLAN_CAPABILITY_IBSS			(1<<1)

#define WLAN_CAPABILITY_CF_POLLABLE		(1<<2)
#define WLAN_CAPABILITY_CF_POLL_REQUEST		(1<<3)
#define WLAN_CAPABILITY_PRIVACY			(1<<4)
#define WLAN_CAPABILITY_SHORT_PREAMBLE		(1<<5)
#define WLAN_CAPABILITY_PBCC			(1<<6)
#define WLAN_CAPABILITY_CHANNEL_AGILITY		(1<<7)

/* 802.11h */
#define WLAN_CAPABILITY_SPECTRUM_MGMT		(1<<8)
#define WLAN_CAPABILITY_QOS			(1<<9)
#define WLAN_CAPABILITY_SHORT_SLOT_TIME		(1<<10)
#define WLAN_CAPABILITY_APSD			(1<<11)
#define WLAN_CAPABILITY_RADIO_MEASURE		(1<<12)
#define WLAN_CAPABILITY_DSSS_OFDM		(1<<13)
#define WLAN_CAPABILITY_DEL_BACK		(1<<14)
#define WLAN_CAPABILITY_IMM_BACK		(1<<15)

#define IEEE80211_HT_OP_MODE_PROTECTION			3
#define	IEEE80211_HT_OP_MODE_PROTECTION_NONE		0
#define	IEEE80211_HT_OP_MODE_PROTECTION_NONMEMBER	1
#define	IEEE80211_HT_OP_MODE_PROTECTION_20MHZ		2
#define	IEEE80211_HT_OP_MODE_PROTECTION_NONHT_MIXED	3

#define AVG_PKT_SIZE				1024
#define IEEE80211_CB_PTR			96
#define IEEE80211_MAX_CHAINS			4
#define IEEE80211_HT_MCS_MASK_LEN 		10
#define IEEE80211_HE_PPE_THRES_MAX_LEN		25
#define IEEE80211_EHT_PPE_THRES_MAX_LEN		32
#define CONFIG_MAC80211_STA_HASH_MAX_SIZE	0
#define IEEE80211_MAX_NAN_INSTANCE_ID		255
/* there are 40 bytes if you don't need the rateset to be kept */
#define IEEE80211_TX_INFO_DRIVER_DATA_SIZE	40

/* if you do need the rateset, then you have less space */
#define IEEE80211_TX_INFO_RATE_DRIVER_DATA_SIZE 24

/* maximum number of rate stages */
#define IEEE80211_TX_MAX_RATES			4

/* maximum number of rate table entries */
#define IEEE80211_TX_RATE_TABLE_SIZE		4

/* The per TXQ device queue limit in airtime */
#define IEEE80211_DEFAULT_AQL_TXQ_LIMIT_L	5000
#define IEEE80211_DEFAULT_AQL_TXQ_LIMIT_H	12000

#define MCS_GROUP_RATES				14

/* The per interface airtime threshold to switch to lower queue limit */
#define IEEE80211_AQL_THRESHOLD			24000
#define IEEE80211_INVAL_HW_QUEUE		0xff
#define IEEE80211_DEFAULT_UAPSD_QUEUES		0
#define IEEE80211_DEFAULT_MAX_SP_LEN		0

/* power level hasn't been configured (or set to automatic) */
#define IEEE80211_UNSET_POWER_LEVEL		INT_MIN
#define IEEE80211_IRQSAFE_QUEUE_LIMIT		128

/* Extended Channel Switching capability to be set in the 1st byte of
 * the @WLAN_EID_EXT_CAPABILITY information element
 */
#define WLAN_EXT_CAPA1_EXT_CHANNEL_SWITCHING	BIT(2)

/* Multiple BSSID capability is set in the 6th bit of 3rd byte of the
 * @WLAN_EID_EXT_CAPABILITY information element
 */
#define WLAN_EXT_CAPA3_MULTI_BSSID_SUPPORT	BIT(6)

/* Timing Measurement protocol for time sync is set in the 7th bit of 3rd byte
 * of the @WLAN_EID_EXT_CAPABILITY information element
 */
#define WLAN_EXT_CAPA3_TIMING_MEASUREMENT_SUPPORT	BIT(7)

/* TDLS capabilities in the 4th byte of @WLAN_EID_EXT_CAPABILITY */
#define WLAN_EXT_CAPA4_TDLS_BUFFER_STA		BIT(4)
#define WLAN_EXT_CAPA4_TDLS_PEER_PSM		BIT(5)
#define WLAN_EXT_CAPA4_TDLS_CHAN_SWITCH		BIT(6)

/* Interworking capabilities are set in 7th bit of 4th byte of the
 * @WLAN_EID_EXT_CAPABILITY information element
 */
#define WLAN_EXT_CAPA4_INTERWORKING_ENABLED	BIT(7)

/*
 * TDLS capabililites to be enabled in the 5th byte of the
 * @WLAN_EID_EXT_CAPABILITY information element
 */
#define WLAN_EXT_CAPA5_TDLS_ENABLED		BIT(5)
#define WLAN_EXT_CAPA5_TDLS_PROHIBITED		BIT(6)
#define WLAN_EXT_CAPA5_TDLS_CH_SW_PROHIBITED	BIT(7)

#define WLAN_EXT_CAPA8_TDLS_WIDE_BW_ENABLED	BIT(5)
#define WLAN_EXT_CAPA8_OPMODE_NOTIF		BIT(6)

/* Defines the maximal number of MSDUs in an A-MSDU. */
#define WLAN_EXT_CAPA8_MAX_MSDU_IN_AMSDU_LSB	BIT(7)
#define WLAN_EXT_CAPA9_MAX_MSDU_IN_AMSDU_MSB	BIT(0)

/*
 * Fine Timing Measurement Initiator - bit 71 of @WLAN_EID_EXT_CAPABILITY
 * information element
 */
#define WLAN_EXT_CAPA9_FTM_INITIATOR		BIT(7)

/* Defines support for TWT Requester and TWT Responder */
#define WLAN_EXT_CAPA10_TWT_REQUESTER_SUPPORT	BIT(5)
#define WLAN_EXT_CAPA10_TWT_RESPONDER_SUPPORT	BIT(6)

/* 802.11n HT capabilities masks (for cap_info) */
#define IEEE80211_HT_CAP_LDPC_CODING		0x0001
#define IEEE80211_HT_CAP_SUP_WIDTH_20_40	0x0002
#define IEEE80211_HT_CAP_SM_PS			0x000C
#define	IEEE80211_HT_CAP_SM_PS_SHIFT		2
#define IEEE80211_HT_CAP_GRN_FLD		0x0010
#define IEEE80211_HT_CAP_SGI_20			0x0020
#define IEEE80211_HT_CAP_SGI_40			0x0040
#define IEEE80211_HT_CAP_TX_STBC		0x0080
#define IEEE80211_HT_CAP_RX_STBC		0x0300
#define	IEEE80211_HT_CAP_RX_STBC_SHIFT		8
#define IEEE80211_HT_CAP_DELAY_BA		0x0400
#define IEEE80211_HT_CAP_MAX_AMSDU		0x0800
#define IEEE80211_HT_CAP_DSSSCCK40		0x1000
#define IEEE80211_HT_CAP_RESERVED		0x2000
#define IEEE80211_HT_CAP_40MHZ_INTOLERANT	0x4000
#define IEEE80211_HT_CAP_LSIG_TXOP_PROT		0x8000

/* 802.11n HT extended capabilities masks (for extended_ht_cap_info) */
#define IEEE80211_HT_EXT_CAP_PCO		0x0001
#define IEEE80211_HT_EXT_CAP_PCO_TIME		0x0006
#define	IEEE80211_HT_EXT_CAP_PCO_TIME_SHIFT	1
#define IEEE80211_HT_EXT_CAP_MCS_FB		0x0300
#define	IEEE80211_HT_EXT_CAP_MCS_FB_SHIFT	8
#define IEEE80211_HT_EXT_CAP_HTC_SUP		0x0400
#define IEEE80211_HT_EXT_CAP_RD_RESPONDER	0x0800

/* 802.11n HT capability AMPDU settings (for ampdu_params_info) */
#define IEEE80211_HT_AMPDU_PARM_FACTOR		0x03
#define IEEE80211_HT_AMPDU_PARM_DENSITY		0x1C
#define	IEEE80211_HT_AMPDU_PARM_DENSITY_SHIFT	2

/* for rx_highest */
#define IEEE80211_VHT_MAX_NSTS_TOTAL_SHIFT	13
#define IEEE80211_VHT_MAX_NSTS_TOTAL_MASK	(7 << IEEE80211_VHT_MAX_NSTS_TOTAL_SHIFT)

/* for tx_highest */
#define IEEE80211_VHT_EXT_NSS_BW_CAPABLE	(1 << 13)

/* 802.11ax HE PHY capabilities */
#define IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_40MHZ_IN_2G		0x02
#define IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_40MHZ_80MHZ_IN_5G	0x04
#define IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_160MHZ_IN_5G		0x08
#define IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_80PLUS80_MHZ_IN_5G	0x10
#define IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_MASK_ALL		0x1e

/*
 * The TX headroom reserved by mac80211 for its own tx_status functions.
 * This is enough for the radiotap header.
 */
#define IEEE80211_TX_STATUS_HEADROOM	ALIGN(14, 4)

/*
 * A-MPDU buffer sizes
 * According to HT size varies from 8 to 64 frames
 * HE adds the ability to have up to 256 frames.
 * EHT adds the ability to have up to 1K frames.
 */
#define IEEE80211_MIN_AMPDU_BUF		0x8
#define IEEE80211_MAX_AMPDU_BUF_HT	0x40
#define IEEE80211_MAX_AMPDU_BUF_HE	0x100
#define IEEE80211_MAX_AMPDU_BUF_EHT	0x400

/* Spatial Multiplexing Power Save Modes (for capability) */
#define WLAN_HT_CAP_SM_PS_STATIC	0
#define WLAN_HT_CAP_SM_PS_DYNAMIC	1
#define WLAN_HT_CAP_SM_PS_INVALID	2
#define WLAN_HT_CAP_SM_PS_DISABLED	3

/* for SM power control field lower two bits */
#define WLAN_HT_SMPS_CONTROL_DISABLED	0
#define WLAN_HT_SMPS_CONTROL_STATIC	1
#define WLAN_HT_SMPS_CONTROL_DYNAMIC	3

#ifndef IEEE80211_MIN_HDR_LEN
#define IEEE80211_MIN_HDR_LEN		10
#endif

#define IEEE80211_WEP_IV_LEN		4
#define IEEE80211_WEP_ICV_LEN		4
#define IEEE80211_CCMP_HDR_LEN		8
#define IEEE80211_CCMP_MIC_LEN		8
#define IEEE80211_CCMP_PN_LEN		6
#define IEEE80211_CCMP_256_HDR_LEN	8
#define IEEE80211_CCMP_256_MIC_LEN	16
#define IEEE80211_CCMP_256_PN_LEN	6
#define IEEE80211_TKIP_IV_LEN		8
#define IEEE80211_TKIP_ICV_LEN		4
#define IEEE80211_CMAC_PN_LEN		6
#define IEEE80211_GMAC_PN_LEN		6
#define IEEE80211_GCMP_HDR_LEN		8
#define IEEE80211_GCMP_MIC_LEN		16
#define IEEE80211_GCMP_PN_LEN		6
#define IEEE80211_CMAC_128_MIC_LEN	8
#define IEEE80211_CMAC_256_MIC_LEN	16
#define IEEE80211_GMAC_MIC_LEN		16
#define FILS_NONCE_LEN			16
#define FILS_MAX_KEK_LEN		64
#define FILS_ERP_MAX_USERNAME_LEN	16
#define FILS_ERP_MAX_REALM_LEN		253
#define FILS_ERP_MAX_RRK_LEN		64
#define PMK_MAX_LEN			64
#define SAE_PASSWORD_MAX_LEN		128

/* Debugfs flags to enable/disable use of RX/TX airtime in scheduler */
#define AIRTIME_USE_TX			BIT(0)
#define AIRTIME_USE_RX			BIT(1)

#define SUITE(oui, id)			(((oui) << 8) | (id))

/* cipher suite selectors */
#define WLAN_CIPHER_SUITE_USE_GROUP		SUITE(0x000FAC, 0)
#define WLAN_CIPHER_SUITE_WEP40			SUITE(0x000FAC, 1)
#define WLAN_CIPHER_SUITE_TKIP			SUITE(0x000FAC, 2)
/* reserved: 					SUITE(0x000FAC, 3) */
#define WLAN_CIPHER_SUITE_CCMP			SUITE(0x000FAC, 4)
#define WLAN_CIPHER_SUITE_WEP104		SUITE(0x000FAC, 5)
#define WLAN_CIPHER_SUITE_AES_CMAC		SUITE(0x000FAC, 6)
#define WLAN_CIPHER_SUITE_GCMP			SUITE(0x000FAC, 8)
#define WLAN_CIPHER_SUITE_GCMP_256		SUITE(0x000FAC, 9)
#define WLAN_CIPHER_SUITE_CCMP_256		SUITE(0x000FAC, 10)
#define WLAN_CIPHER_SUITE_BIP_GMAC_128		SUITE(0x000FAC, 11)
#define WLAN_CIPHER_SUITE_BIP_GMAC_256		SUITE(0x000FAC, 12)
#define WLAN_CIPHER_SUITE_BIP_CMAC_256		SUITE(0x000FAC, 13)
#define WLAN_CIPHER_SUITE_SMS4			SUITE(0x001472, 1)

/* AKM suite selectors */
#define WLAN_AKM_SUITE_8021X			SUITE(0x000FAC, 1)
#define WLAN_AKM_SUITE_PSK			SUITE(0x000FAC, 2)
#define WLAN_AKM_SUITE_FT_8021X			SUITE(0x000FAC, 3)
#define WLAN_AKM_SUITE_FT_PSK			SUITE(0x000FAC, 4)
#define WLAN_AKM_SUITE_8021X_SHA256		SUITE(0x000FAC, 5)
#define WLAN_AKM_SUITE_PSK_SHA256		SUITE(0x000FAC, 6)
#define WLAN_AKM_SUITE_TDLS			SUITE(0x000FAC, 7)
#define WLAN_AKM_SUITE_SAE			SUITE(0x000FAC, 8)
#define WLAN_AKM_SUITE_FT_OVER_SAE		SUITE(0x000FAC, 9)
#define WLAN_AKM_SUITE_AP_PEER_KEY		SUITE(0x000FAC, 10)
#define WLAN_AKM_SUITE_8021X_SUITE_B		SUITE(0x000FAC, 11)
#define WLAN_AKM_SUITE_8021X_SUITE_B_192	SUITE(0x000FAC, 12)
#define WLAN_AKM_SUITE_FT_8021X_SHA384		SUITE(0x000FAC, 13)
#define WLAN_AKM_SUITE_FILS_SHA256		SUITE(0x000FAC, 14)
#define WLAN_AKM_SUITE_FILS_SHA384		SUITE(0x000FAC, 15)
#define WLAN_AKM_SUITE_FT_FILS_SHA256		SUITE(0x000FAC, 16)
#define WLAN_AKM_SUITE_FT_FILS_SHA384		SUITE(0x000FAC, 17)
#define WLAN_AKM_SUITE_OWE			SUITE(0x000FAC, 18)
#define WLAN_AKM_SUITE_FT_PSK_SHA384		SUITE(0x000FAC, 19)
#define WLAN_AKM_SUITE_PSK_SHA384		SUITE(0x000FAC, 20)

#define WLAN_AKM_SUITE_WFA_DPP			SUITE(WLAN_OUI_WFA, 2)

#define AVG_PKT_SIZE				1024

/* Number of bits for an average sized packet */
#define MCS_NBITS				(AVG_PKT_SIZE << 3)

/* Number of kilo-symbols (symbols * 1024) for a packet with (bps) bits per
 * symbol. We use k-symbols to avoid rounding in the _TIME macros below.
 */
#define MCS_N_KSYMS(bps)			DIV_ROUND_UP(MCS_NBITS << 10, (bps))

/* Transmission time (1024 usec) for a packet containing (ksyms) * k-symbols */
#define HE_SYMBOL_TIME(gi, ksyms)					\
	(gi == HE_GI_08 ?						\
	 ((ksyms) * 16 * 17) / 20 :		/* 13.6 us per sym */	\
	 (gi == HE_GI_16 ?						\
	  ((ksyms) * 16 * 18) / 20 :		/* 14.4 us per sym */	\
	  ((ksyms) * 16)			/* 16.0 us per sym */	\
	 ))

/* Transmit duration for the raw data part of an average sized packet */
#define HE_DURATION(streams, gi, bps) \
	((u32)HE_SYMBOL_TIME(gi, MCS_N_KSYMS((streams) * (bps))))

#define HE_DURATION_S(shift, streams, gi, bps)		\
	(HE_DURATION(streams, gi, bps) >> shift)

/* These should match the values in enum nl80211_he_gi */
#define HE_GI_08 0
#define HE_GI_16 1
#define HE_GI_32 2

/* gi in HE/EHT is identical. It matches enum nl80211_eht_gi as well */
#define EHT_GI_08 HE_GI_08
#define EHT_GI_16 HE_GI_16
#define EHT_GI_32 HE_GI_32

#define EHT_DURATION(streams, gi, bps)			\
	HE_DURATION(streams, gi, bps)
#define EHT_DURATION_S(shift, streams, gi, bps)		\
	HE_DURATION_S(shift, streams, gi, bps)

#define BW_20			0
#define BW_40			1
#define BW_80			2
#define BW_160			3
#define BW_320			4

#define CHAN2G(_freq, _flags)  {	\
	.band = NL80211_BAND_2GHZ,	\
	.center_freq = (_freq),		\
	.hw_value = (_freq),		\
	.flags = (_flags),		\
}

#define CHAN5G(_freq) {			\
	.band = NL80211_BAND_5GHZ,	\
	.center_freq = (_freq),		\
	.hw_value = (_freq),		\
}

/*Define group sort order: HT40 -> SGI -> #streams */
#define IEEE80211_MAX_STREAMS		4
#define IEEE80211_HT_STREAM_GROUPS	4 /* BW(=2) * SGI(=2) */
#define IEEE80211_VHT_STREAM_GROUPS	8 /* BW(=4) * SGI(=2) */

#define IEEE80211_HE_MAX_STREAMS	8
#define IEEE80211_HE_STREAM_GROUPS	12 /* BW(=4) * GI(=3) */

#define IEEE80211_EHT_MAX_STREAMS	8
#define IEEE80211_EHT_STREAM_GROUPS	15 /* BW(=5) * GI(=3) */

#define IEEE80211_HT_GROUPS_NB	(IEEE80211_MAX_STREAMS *	\
				 IEEE80211_HT_STREAM_GROUPS)
#define IEEE80211_VHT_GROUPS_NB	(IEEE80211_MAX_STREAMS *	\
					 IEEE80211_VHT_STREAM_GROUPS)
#define IEEE80211_HE_GROUPS_NB	(IEEE80211_HE_MAX_STREAMS *	\
				 IEEE80211_HE_STREAM_GROUPS)
#define IEEE80211_EHT_GROUPS_NB	(IEEE80211_EHT_MAX_STREAMS *	\
				 IEEE80211_EHT_STREAM_GROUPS)

#define IEEE80211_HT_GROUP_0	0
#define IEEE80211_VHT_GROUP_0	(IEEE80211_HT_GROUP_0 + IEEE80211_HT_GROUPS_NB)
#define IEEE80211_HE_GROUP_0	(IEEE80211_VHT_GROUP_0 + IEEE80211_VHT_GROUPS_NB)
#define IEEE80211_EHT_GROUP_0	(IEEE80211_HE_GROUP_0 + IEEE80211_HE_GROUPS_NB)

#define EHT_GROUP_IDX(_streams, _gi, _bw)				\
	(IEEE80211_EHT_GROUP_0 +					\
	 IEEE80211_EHT_MAX_STREAMS * 3 * (_bw) +			\
	 IEEE80211_EHT_MAX_STREAMS * (_gi) +				\
	 (_streams) - 1)


#define EHT_BW2VBPS(_bw, r5, r4, r3, r2, r1)				((_bw) == BW_320 ? r5 : BW2VBPS(_bw, r4, r3, r2, r1))

#define __EHT_GROUP(_streams, _gi, _bw, _s)						\
	[EHT_GROUP_IDX(_streams, _gi, _bw)] = {						\
	.shift = _s,									\
	.duration = {									\
		EHT_DURATION_S(_s, _streams, _gi,					\
			       EHT_BW2VBPS(_bw,  1960,   980,  490,  234,  117)),	\
		EHT_DURATION_S(_s, _streams, _gi,					\
			       EHT_BW2VBPS(_bw,  3920,  1960,  980,  468,  234)),	\
		EHT_DURATION_S(_s, _streams, _gi,					\
			       EHT_BW2VBPS(_bw,  5880,  2937, 1470,  702,  351)),	\
		EHT_DURATION_S(_s, _streams, _gi,					\
			       EHT_BW2VBPS(_bw,  7840,  3920, 1960,  936,  468)),	\
		EHT_DURATION_S(_s, _streams, _gi,					\
			       EHT_BW2VBPS(_bw, 11760,  5880, 2940, 1404,  702)),	\
		EHT_DURATION_S(_s, _streams, _gi,					\
			       EHT_BW2VBPS(_bw, 15680,  7840, 3920, 1872,  936)),	\
		EHT_DURATION_S(_s, _streams, _gi,					\
			       EHT_BW2VBPS(_bw, 17640,  8820, 4410, 2106, 1053)),	\
		EHT_DURATION_S(_s, _streams, _gi,					\
			       EHT_BW2VBPS(_bw, 19600,  9800, 4900, 2340, 1170)),	\
		EHT_DURATION_S(_s, _streams, _gi,					\
			       EHT_BW2VBPS(_bw, 23520, 11760, 5880, 2808, 1404)),	\
		EHT_DURATION_S(_s, _streams, _gi,					\
			       EHT_BW2VBPS(_bw, 26133, 13066, 6533, 3120, 1560)),	\
		EHT_DURATION_S(_s, _streams, _gi,					\
			       EHT_BW2VBPS(_bw, 29400, 14700, 7350, 3510, 1755)),	\
		EHT_DURATION_S(_s, _streams, _gi,					\
			       EHT_BW2VBPS(_bw, 32666, 16333, 8166, 3900, 1950)),	\
		EHT_DURATION_S(_s, _streams, _gi,					\
			       EHT_BW2VBPS(_bw, 35280, 17640, 8820, 4212, 2106)),	\
		EHT_DURATION_S(_s, _streams, _gi,					\
			       EHT_BW2VBPS(_bw, 39200, 19600, 9800, 4680, 2340))	\
		}									\
}

#define GROUP_SHIFT(duration)                                           		\
        _MAX(0, 16 - __builtin_clz(duration))

#define EHT_GROUP_SHIFT(_streams, _gi, _bw)						\
	GROUP_SHIFT(EHT_DURATION(_streams, _gi,						\
				 EHT_BW2VBPS(_bw, 1960, 980, 490, 234, 117)))

#define EHT_GROUP(_streams, _gi, _bw)				\
	__EHT_GROUP(_streams, _gi, _bw,				\
		   EHT_GROUP_SHIFT(_streams, _gi, _bw))

#define EHT_GROUP_RANGE(_gi, _bw)	\
	EHT_GROUP(1, _gi, _bw),		\
	EHT_GROUP(2, _gi, _bw),		\
	EHT_GROUP(3, _gi, _bw),		\
	EHT_GROUP(4, _gi, _bw),		\
	EHT_GROUP(5, _gi, _bw),		\
	EHT_GROUP(6, _gi, _bw),		\
	EHT_GROUP(7, _gi, _bw),		\
	EHT_GROUP(8, _gi, _bw)

/* Transmission time (in 1024 * usec) for a packet containing (ksyms) * 1024
 * symbols.
 */
#define MCS_SYMBOL_TIME(sgi, ksyms)					\
	(sgi ?								\
	  ((ksyms) * 4 * 18) / 20 :		/* 3.6 us per sym */	\
	  ((ksyms) * 4)			/* 4.0 us per sym */		\
	)

/* Transmit duration for the raw data part of an average sized packet */
#define MCS_DURATION(streams, sgi, bps) 				\
	((u32)MCS_SYMBOL_TIME(sgi, MCS_N_KSYMS((streams) * (bps))))

#define MCS_DURATION_S(shift, streams, sgi, bps)			\
	((u16)((MCS_DURATION(streams, sgi, bps) >> shift)))

#define HT_GROUP_IDX(_streams, _sgi, _ht40)	\
	IEEE80211_HT_GROUP_0 +			\
	IEEE80211_MAX_STREAMS * 2 * _ht40 +	\
	IEEE80211_MAX_STREAMS * _sgi +		\
	_streams - 1

/* MCS rate information for an MCS group */
#define __MCS_GROUP(_streams, _sgi, _ht40, _s)				\
	[HT_GROUP_IDX(_streams, _sgi, _ht40)] = {			\
	.shift = _s,							\
	.duration = {							\
		MCS_DURATION_S(_s, _streams, _sgi, _ht40 ? 54 : 26),	\
		MCS_DURATION_S(_s, _streams, _sgi, _ht40 ? 108 : 52),	\
		MCS_DURATION_S(_s, _streams, _sgi, _ht40 ? 162 : 78),	\
		MCS_DURATION_S(_s, _streams, _sgi, _ht40 ? 216 : 104),	\
		MCS_DURATION_S(_s, _streams, _sgi, _ht40 ? 324 : 156),	\
		MCS_DURATION_S(_s, _streams, _sgi, _ht40 ? 432 : 208),	\
		MCS_DURATION_S(_s, _streams, _sgi, _ht40 ? 486 : 234),	\
		MCS_DURATION_S(_s, _streams, _sgi, _ht40 ? 540 : 260)	\
	}								\
}

#define MCS_GROUP_SHIFT(_streams, _sgi, _ht40)				\
	GROUP_SHIFT(MCS_DURATION(_streams, _sgi, _ht40 ? 54 : 26))

#define MCS_GROUP(_streams, _sgi, _ht40)				\
	__MCS_GROUP(_streams, _sgi, _ht40,				\
		    MCS_GROUP_SHIFT(_streams, _sgi, _ht40))

#define VHT_GROUP_IDX(_streams, _sgi, _bw)				\
	(IEEE80211_VHT_GROUP_0 +					\
	 IEEE80211_MAX_STREAMS * 2 * (_bw) +				\
	 IEEE80211_MAX_STREAMS * (_sgi) +				\
	 (_streams) - 1)

#define BW2VBPS(_bw, r4, r3, r2, r1)					\
	(_bw == BW_160 ? r4 : _bw == BW_80 ? r3 : _bw == BW_40 ? r2 : r1)

#define __VHT_GROUP(_streams, _sgi, _bw, _s)				\
	[VHT_GROUP_IDX(_streams, _sgi, _bw)] = {			\
	.shift = _s,							\
	.duration = {							\
		MCS_DURATION_S(_s, _streams, _sgi,			\
			       BW2VBPS(_bw,  234,  117,  54,  26)),	\
		MCS_DURATION_S(_s, _streams, _sgi,			\
			       BW2VBPS(_bw,  468,  234, 108,  52)),	\
		MCS_DURATION_S(_s, _streams, _sgi,			\
			       BW2VBPS(_bw,  702,  351, 162,  78)),	\
		MCS_DURATION_S(_s, _streams, _sgi,			\
			       BW2VBPS(_bw,  936,  468, 216, 104)),	\
		MCS_DURATION_S(_s, _streams, _sgi,			\
			       BW2VBPS(_bw, 1404,  702, 324, 156)),	\
		MCS_DURATION_S(_s, _streams, _sgi,			\
			       BW2VBPS(_bw, 1872,  936, 432, 208)),	\
		MCS_DURATION_S(_s, _streams, _sgi,			\
			       BW2VBPS(_bw, 2106, 1053, 486, 234)),	\
		MCS_DURATION_S(_s, _streams, _sgi,			\
			       BW2VBPS(_bw, 2340, 1170, 540, 260)),	\
		MCS_DURATION_S(_s, _streams, _sgi,			\
			       BW2VBPS(_bw, 2808, 1404, 648, 312)),	\
		MCS_DURATION_S(_s, _streams, _sgi,			\
			       BW2VBPS(_bw, 3120, 1560, 720, 346))	\
        }								\
}

#define VHT_GROUP_SHIFT(_streams, _sgi, _bw)				\
	GROUP_SHIFT(MCS_DURATION(_streams, _sgi,			\
				 BW2VBPS(_bw, 234, 117,  54,  26)))

#define VHT_GROUP(_streams, _sgi, _bw)					\
	__VHT_GROUP(_streams, _sgi, _bw,				\
		    VHT_GROUP_SHIFT(_streams, _sgi, _bw))


#define HE_GROUP_IDX(_streams, _gi, _bw)				\
	(IEEE80211_HE_GROUP_0 +						\
	 IEEE80211_HE_MAX_STREAMS * 3 * (_bw) +				\
	 IEEE80211_HE_MAX_STREAMS * (_gi) +				\
	 (_streams) - 1)

#define __HE_GROUP(_streams, _gi, _bw, _s)				\
	[HE_GROUP_IDX(_streams, _gi, _bw)] = {				\
	.shift = _s,							\
	.duration = {							\
		HE_DURATION_S(_s, _streams, _gi,			\
			      BW2VBPS(_bw,   979,  489,  230,  115)),	\
		HE_DURATION_S(_s, _streams, _gi,			\
			      BW2VBPS(_bw,  1958,  979,  475,  230)),	\
		HE_DURATION_S(_s, _streams, _gi,			\
			      BW2VBPS(_bw,  2937, 1468,  705,  345)),	\
		HE_DURATION_S(_s, _streams, _gi,			\
			      BW2VBPS(_bw,  3916, 1958,  936,  475)),	\
		HE_DURATION_S(_s, _streams, _gi,			\
			      BW2VBPS(_bw,  5875, 2937, 1411,  705)),	\
		HE_DURATION_S(_s, _streams, _gi,			\
			      BW2VBPS(_bw,  7833, 3916, 1872,  936)),	\
		HE_DURATION_S(_s, _streams, _gi,			\
			      BW2VBPS(_bw,  8827, 4406, 2102, 1051)),	\
		HE_DURATION_S(_s, _streams, _gi,			\
			      BW2VBPS(_bw,  9806, 4896, 2347, 1166)),	\
		HE_DURATION_S(_s, _streams, _gi,			\
			      BW2VBPS(_bw, 11764, 5875, 2808, 1411)),	\
		HE_DURATION_S(_s, _streams, _gi,			\
			      BW2VBPS(_bw, 13060, 6523, 3124, 1555)),	\
		HE_DURATION_S(_s, _streams, _gi,			\
			      BW2VBPS(_bw, 14702, 7344, 3513, 1756)),	\
		HE_DURATION_S(_s, _streams, _gi,			\
			      BW2VBPS(_bw, 16329, 8164, 3902, 1944))	\
        }								\
}

#define HE_GROUP_SHIFT(_streams, _gi, _bw)				\
	GROUP_SHIFT(HE_DURATION(_streams, _gi,				\
				BW2VBPS(_bw,   979,  489,  230,  115)))

#define HE_GROUP(_streams, _gi, _bw)					\
	__HE_GROUP(_streams, _gi, _bw,					\
		   HE_GROUP_SHIFT(_streams, _gi, _bw))

/*
 * TDLS capabililites to be enabled in the 5th byte of the
 * @WLAN_EID_EXT_CAPABILITY information element
 */
#define WLAN_EXT_CAPA5_TDLS_ENABLED		BIT(5)
#define WLAN_EXT_CAPA5_TDLS_PROHIBITED		BIT(6)
#define WLAN_EXT_CAPA5_TDLS_CH_SW_PROHIBITED	BIT(7)

#define WLAN_EXT_CAPA8_TDLS_WIDE_BW_ENABLED	BIT(5)
#define WLAN_EXT_CAPA8_OPMODE_NOTIF		BIT(6)

/* Defines the maximal number of MSDUs in an A-MSDU. */
#define WLAN_EXT_CAPA8_MAX_MSDU_IN_AMSDU_LSB	BIT(7)
#define WLAN_EXT_CAPA9_MAX_MSDU_IN_AMSDU_MSB	BIT(0)

/* multi-link device */
#define IEEE80211_MLD_MAX_NUM_LINKS 15
#define IEEE80211_MAX_PN_LEN 16

/* convert frequencies */
#define MHZ_TO_KHZ(freq) ((freq) * 1000)
#define KHZ_TO_MHZ(freq) ((freq) / 1000)
#define PR_KHZ(f) KHZ_TO_MHZ(f), f % 1000
#define KHZ_F "%d.%03d"

/* convert powers */
#define DBI_TO_MBI(gain) ((gain) * 100)
#define MBI_TO_DBI(gain) ((gain) / 100)
#define DBM_TO_MBM(gain) ((gain) * 100)
#define MBM_TO_DBM(gain) ((gain) / 100)

#define FCS_LEN 4

#define IEEE80211_FCTL_VERS		0x0003
#define IEEE80211_FCTL_FTYPE		0x000c
#define IEEE80211_FCTL_STYPE		0x00f0
#define IEEE80211_FCTL_TODS		0x0100
#define IEEE80211_FCTL_FROMDS		0x0200
#define IEEE80211_FCTL_MOREFRAGS	0x0400
#define IEEE80211_FCTL_RETRY		0x0800
#define IEEE80211_FCTL_PM		0x1000
#define IEEE80211_FCTL_MOREDATA		0x2000
#define IEEE80211_FCTL_PROTECTED	0x4000
#define IEEE80211_FCTL_ORDER		0x8000
#define IEEE80211_FCTL_CTL_EXT		0x0f00

#define IEEE80211_SCTL_FRAG		0x000F
#define IEEE80211_SCTL_SEQ		0xFFF0

#define IEEE80211_FTYPE_MGMT		0x0000
#define IEEE80211_FTYPE_CTL		0x0004
#define IEEE80211_FTYPE_DATA		0x0008
#define IEEE80211_FTYPE_EXT		0x000c

/* management */
#define IEEE80211_STYPE_ASSOC_REQ	0x0000
#define IEEE80211_STYPE_ASSOC_RESP	0x0010
#define IEEE80211_STYPE_REASSOC_REQ	0x0020
#define IEEE80211_STYPE_REASSOC_RESP	0x0030
#define IEEE80211_STYPE_PROBE_REQ	0x0040
#define IEEE80211_STYPE_PROBE_RESP	0x0050
#define IEEE80211_STYPE_BEACON		0x0080
#define IEEE80211_STYPE_ATIM		0x0090
#define IEEE80211_STYPE_DISASSOC	0x00A0
#define IEEE80211_STYPE_AUTH		0x00B0
#define IEEE80211_STYPE_DEAUTH		0x00C0
#define IEEE80211_STYPE_ACTION		0x00D0

/* control */
#define IEEE80211_STYPE_TRIGGER		0x0020
#define IEEE80211_STYPE_CTL_EXT		0x0060
#define IEEE80211_STYPE_BACK_REQ	0x0080
#define IEEE80211_STYPE_BACK		0x0090
#define IEEE80211_STYPE_PSPOLL		0x00A0
#define IEEE80211_STYPE_RTS		0x00B0
#define IEEE80211_STYPE_CTS		0x00C0
#define IEEE80211_STYPE_ACK		0x00D0
#define IEEE80211_STYPE_CFEND		0x00E0
#define IEEE80211_STYPE_CFENDACK	0x00F0

/* data */
#define IEEE80211_STYPE_DATA			0x0000
#define IEEE80211_STYPE_DATA_CFACK		0x0010
#define IEEE80211_STYPE_DATA_CFPOLL		0x0020
#define IEEE80211_STYPE_DATA_CFACKPOLL		0x0030
#define IEEE80211_STYPE_NULLFUNC		0x0040
#define IEEE80211_STYPE_CFACK			0x0050
#define IEEE80211_STYPE_CFPOLL			0x0060
#define IEEE80211_STYPE_CFACKPOLL		0x0070
#define IEEE80211_STYPE_QOS_DATA		0x0080
#define IEEE80211_STYPE_QOS_DATA_CFACK		0x0090
#define IEEE80211_STYPE_QOS_DATA_CFPOLL		0x00A0
#define IEEE80211_STYPE_QOS_DATA_CFACKPOLL	0x00B0
#define IEEE80211_STYPE_QOS_NULLFUNC		0x00C0
#define IEEE80211_STYPE_QOS_CFACK		0x00D0
#define IEEE80211_STYPE_QOS_CFPOLL		0x00E0
#define IEEE80211_STYPE_QOS_CFACKPOLL		0x00F0

/* extension, added by 802.11ad */
#define IEEE80211_STYPE_DMG_BEACON		0x0000
#define IEEE80211_STYPE_S1G_BEACON		0x0010

/* miscellaneous IEEE 802.11 constants */
#define IEEE80211_MAX_FRAG_THRESHOLD	2352
#define IEEE80211_MAX_RTS_THRESHOLD	2353
#define IEEE80211_MAX_AID		2007
#define IEEE80211_MAX_AID_S1G		8191
#define IEEE80211_MAX_TIM_LEN		251
#define IEEE80211_MAX_MESH_PEERINGS	63

/* bits unique to S1G beacon */
#define IEEE80211_S1G_BCN_NEXT_TBTT	0x100
#define IEEE80211_S1G_BCN_CSSID		0x200
#define IEEE80211_S1G_BCN_ANO		0x400

/* S1G encoding types */
#define IEEE80211_S1G_TIM_ENC_MODE_BLOCK	0
#define IEEE80211_S1G_TIM_ENC_MODE_SINGLE	1
#define IEEE80211_S1G_TIM_ENC_MODE_OLB		2

/* Maximum size for the MA-UNITDATA primitive, 802.11 standard section
   6.2.1.1.2.

   802.11e clarifies the figure in section 7.1.2. The frame body is
   up to 2304 octets long (maximum MSDU size) plus any crypt overhead. */
#define IEEE80211_MAX_DATA_LEN		2304
/* 802.11ad extends maximum MSDU size for DMG (freq > 40Ghz) networks
 * to 7920 bytes, see 8.2.3 General frame format
 */
#define IEEE80211_MAX_DATA_LEN_DMG	7920
/* 30 byte 4 addr hdr, 2 byte QoS, 2304 byte MSDU, 12 byte crypt, 4 byte FCS */
#define IEEE80211_MAX_FRAME_LEN		2352

/* Maximal size of an A-MSDU that can be transported in a HT BA session */
#define IEEE80211_MAX_MPDU_LEN_HT_BA		4095

/* Maximal size of an A-MSDU */
#define IEEE80211_MAX_MPDU_LEN_HT_3839		3839
#define IEEE80211_MAX_MPDU_LEN_HT_7935		7935

#define IEEE80211_MAX_MPDU_LEN_VHT_3895		3895
#define IEEE80211_MAX_MPDU_LEN_VHT_7991		7991
#define IEEE80211_MAX_MPDU_LEN_VHT_11454	11454

#define IEEE80211_MAX_SSID_LEN		32

#define IEEE80211_MAX_MESH_ID_LEN	32

#define IEEE80211_FIRST_TSPEC_TSID	8
#define IEEE80211_NUM_TIDS		16

/* number of user priorities 802.11 uses */
#define IEEE80211_NUM_UPS		8
/* number of ACs */
#define IEEE80211_NUM_ACS		4

#define IEEE80211_QOS_CTL_LEN		2
/* 1d tag mask */
#define IEEE80211_QOS_CTL_TAG1D_MASK		0x0007
/* TID mask */
#define IEEE80211_QOS_CTL_TID_MASK		0x000f
/* EOSP */
#define IEEE80211_QOS_CTL_EOSP			0x0010
/* ACK policy */
#define IEEE80211_QOS_CTL_ACK_POLICY_NORMAL	0x0000
#define IEEE80211_QOS_CTL_ACK_POLICY_NOACK	0x0020
#define IEEE80211_QOS_CTL_ACK_POLICY_NO_EXPL	0x0040
#define IEEE80211_QOS_CTL_ACK_POLICY_BLOCKACK	0x0060
#define IEEE80211_QOS_CTL_ACK_POLICY_MASK	0x0060
/* A-MSDU 802.11n */
#define IEEE80211_QOS_CTL_A_MSDU_PRESENT	0x0080
/* Mesh Control 802.11s */
#define IEEE80211_QOS_CTL_MESH_CONTROL_PRESENT  0x0100

/* Mesh Power Save Level */
#define IEEE80211_QOS_CTL_MESH_PS_LEVEL		0x0200
/* Mesh Receiver Service Period Initiated */
#define IEEE80211_QOS_CTL_RSPI			0x0400

/* U-APSD queue for WMM IEs sent by AP */
#define IEEE80211_WMM_IE_AP_QOSINFO_UAPSD	(1<<7)
#define IEEE80211_WMM_IE_AP_QOSINFO_PARAM_SET_CNT_MASK	0x0f

/* U-APSD queues for WMM IEs sent by STA */
#define IEEE80211_WMM_IE_STA_QOSINFO_AC_VO	(1<<0)
#define IEEE80211_WMM_IE_STA_QOSINFO_AC_VI	(1<<1)
#define IEEE80211_WMM_IE_STA_QOSINFO_AC_BK	(1<<2)
#define IEEE80211_WMM_IE_STA_QOSINFO_AC_BE	(1<<3)
#define IEEE80211_WMM_IE_STA_QOSINFO_AC_MASK	0x0f

/* U-APSD max SP length for WMM IEs sent by STA */
#define IEEE80211_WMM_IE_STA_QOSINFO_SP_ALL	0x00
#define IEEE80211_WMM_IE_STA_QOSINFO_SP_2	0x01
#define IEEE80211_WMM_IE_STA_QOSINFO_SP_4	0x02
#define IEEE80211_WMM_IE_STA_QOSINFO_SP_6	0x03
#define IEEE80211_WMM_IE_STA_QOSINFO_SP_MASK	0x03
#define IEEE80211_WMM_IE_STA_QOSINFO_SP_SHIFT	5

#define IEEE80211_HT_CTL_LEN		4

/* trigger type within common_info of trigger frame */
#define IEEE80211_TRIGGER_TYPE_MASK		0xf
#define IEEE80211_TRIGGER_TYPE_BASIC		0x0
#define IEEE80211_TRIGGER_TYPE_BFRP		0x1
#define IEEE80211_TRIGGER_TYPE_MU_BAR		0x2
#define IEEE80211_TRIGGER_TYPE_MU_RTS		0x3
#define IEEE80211_TRIGGER_TYPE_BSRP		0x4
#define IEEE80211_TRIGGER_TYPE_GCR_MU_BAR	0x5
#define IEEE80211_TRIGGER_TYPE_BQRP		0x6
#define IEEE80211_TRIGGER_TYPE_NFRP		0x7

/* UL-bandwidth within common_info of trigger frame */
#define IEEE80211_TRIGGER_ULBW_MASK		0xc0000
#define IEEE80211_TRIGGER_ULBW_20MHZ		0x0
#define IEEE80211_TRIGGER_ULBW_40MHZ		0x1
#define IEEE80211_TRIGGER_ULBW_80MHZ		0x2
#define IEEE80211_TRIGGER_ULBW_160_80P80MHZ	0x3

/* HE Operation defines */
#define IEEE80211_HE_OPERATION_DFLT_PE_DURATION_MASK	0x00000007
#define IEEE80211_HE_OPERATION_TWT_REQUIRED		0x00000008
#define IEEE80211_HE_OPERATION_RTS_THRESHOLD_MASK	0x00003ff0
#define IEEE80211_HE_OPERATION_RTS_THRESHOLD_OFFSET	4
#define IEEE80211_HE_OPERATION_VHT_OPER_INFO		0x00004000
#define IEEE80211_HE_OPERATION_CO_HOSTED_BSS		0x00008000
#define IEEE80211_HE_OPERATION_ER_SU_DISABLE		0x00010000
#define IEEE80211_HE_OPERATION_6GHZ_OP_INFO		0x00020000
#define IEEE80211_HE_OPERATION_BSS_COLOR_MASK		0x3f000000
#define IEEE80211_HE_OPERATION_BSS_COLOR_OFFSET		24
#define IEEE80211_HE_OPERATION_PARTIAL_BSS_COLOR	0x40000000
#define IEEE80211_HE_OPERATION_BSS_COLOR_DISABLED	0x80000000

#define IEEE80211_6GHZ_CTRL_REG_LPI_AP			0
#define IEEE80211_6GHZ_CTRL_REG_SP_AP			1
#define IEEE80211_6GHZ_CTRL_REG_VLP_AP			2
#define IEEE80211_6GHZ_CTRL_REG_INDOOR_LPI_AP		3
#define IEEE80211_6GHZ_CTRL_REG_INDOOR_SP_AP_OLD	4
#define IEEE80211_6GHZ_CTRL_REG_INDOOR_SP_AP		8

/* interface handling */
#define MAC80211_SUPPORTED_FEATURES_TX	(NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM |    \
					 NETIF_F_HW_CSUM | NETIF_F_SG | 	  \
					 NETIF_F_HIGHDMA | NETIF_F_GSO_SOFTWARE | \
					 NETIF_F_HW_TC)
#define MAC80211_SUPPORTED_FEATURES_RX	(NETIF_F_RXCSUM)
#define MAC80211_SUPPORTED_FEATURES	(MAC80211_SUPPORTED_FEATURES_TX | MAC80211_SUPPORTED_FEATURES_RX)
/*
 * wiphy_dereference - fetch RCU pointer when updates are prevented by wiphy mtx
 * @wiphy: the wiphy to check the locking on
 * @p: The pointer to read, prior to dereferencing
 *
 * Return: the value of the specified RCU-protected pointer, but omit the
 * READ_ONCE(), because caller holds the wiphy mutex used for updates.
 */
#define wiphy_dereference(wiphy, p)			rcu_dereference_protected(p, 1)

#define sdata_dereference(p, sdata)			wiphy_dereference(sdata->local->hw.wiphy, p)

#define for_each_sta_info(local, _addr, _sta, _tmp)			\
	rhl_for_each_entry_rcu(_sta, _tmp,				\
			       sta_info_hash_lookup(local, _addr), hash_node)

#define for_each_link_sta_info(local, _addr, _sta, _tmp)		\
	rhl_for_each_entry_rcu(_sta, _tmp,				\
			       link_sta_info_hash_lookup(local, _addr),	\
			       link_hash_node)

/*
 * for_each_sdata_link_rcu() must be used under RCU read lock.
 */
#define for_each_sdata_link_rcu(_local, _link)						\
	/* outer loop just to define the variables ... */				\
	for (struct ieee80211_sub_if_data *___sdata = NULL;				\
	     !___sdata;									\
	     ___sdata = (void *)~0 /* always stop */)					\
	for (int ___link_id = ARRAY_SIZE(___sdata->link);		\
	     ___link_id; ___link_id = 0 /* always stop */)		\
	list_for_each_entry(___sdata, &(_local)->interfaces, list)	\
	if (___link_id == ARRAY_SIZE(___sdata->link) &&			\
	    ieee80211_sdata_running(___sdata))				\
	for (___link_id = 0;						\
	     ___link_id < ARRAY_SIZE((___sdata)->link);					\
	     ___link_id++)								\
	if ((_link = rcu_dereference((___sdata)->link[___link_id])))

#define for_each_link_data(sdata, __link)					\
	/* outer loop just to define the variable ... */			\
	for (struct ieee80211_sub_if_data *__sdata = (sdata); __sdata;		\
		__sdata = NULL /* always stop */)				\
	for (int __link_id = 0;							\
	     __link_id < ARRAY_SIZE((__sdata)->link); __link_id++)		\
		if ((!(__sdata)->vif.valid_links ||				\
		     (__sdata)->vif.valid_links & BIT(__link_id)) &&		\
		    ((__link) = sdata_dereference((__sdata)->link[__link_id],	\
						  (__sdata))))

/*
 * ieee80211_set_sband_iftype_data - set sband iftype data array
 * @sband: the sband to initialize
 * @iftd: the iftype data array
 */
#define ieee80211_set_sband_iftype_data(sband, iftd)	\
	_ieee80211_set_sband_iftype_data(sband, iftd, ARRAY_SIZE(iftd))

/*
 * for_each_sband_iftype_data - iterate sband iftype data entries
 * @sband: the sband whose iftype_data array to iterate
 * @i: iterator counter
 * @iftd: iftype data pointer to set
 */
#define for_each_sband_iftype_data(sband, i, iftd)				\
	for (i = 0, iftd = (const void __force *)&(sband)->iftype_data[i];	\
	     i < (sband)->n_iftype_data;					\
	     i++, iftd = (const void __force *)&(sband)->iftype_data[i])

/* generic connection state used for simulating the wpa-supplicant authentication component */
enum connection_state {
	CONNECTION_IDLE,
	CONNECTION_AUTH_SENT,
	CONNECTION_ASSOC_SENT,
	CONNECTION_WAIT_EAPOL_M1,
	CONNECTION_WAIT_EAPOL_M3,
	CONNECTION_CONNECTED,
	CONNECTION_FAILED,
};

enum connection_tx_kind {
	CONNECTION_TX_MANAGEMENT,
	CONNECTION_TX_EAPOL,
};

/*
 * enum ieee80211_interface_iteration_flags - interface iteration flags
 * @IEEE80211_IFACE_ITER_NORMAL: Iterate over all interfaces that have
 *	been added to the driver; However, note that during hardware
 *	reconfiguration (after restart_hw) it will iterate over a new
 *	interface and over all the existing interfaces even if they
 *	haven't been re-added to the driver yet.
 * @IEEE80211_IFACE_ITER_RESUME_ALL: During resume, iterate over all
 *	interfaces, even if they haven't been re-added to the driver yet.
 * @IEEE80211_IFACE_ITER_ACTIVE: Iterate only active interfaces (netdev is up).
 * @IEEE80211_IFACE_SKIP_SDATA_NOT_IN_DRIVER: Skip any interfaces where SDATA
 *	is not in the driver.  This may fix crashes during firmware recovery
 *	for instance.
 */
enum ieee80211_interface_iteration_flags {
	IEEE80211_IFACE_ITER_NORMAL		 = 0,
	IEEE80211_IFACE_ITER_RESUME_ALL		 = BIT(0),
	IEEE80211_IFACE_ITER_ACTIVE		 = BIT(1),
	IEEE80211_IFACE_SKIP_SDATA_NOT_IN_DRIVER = BIT(2),
};

/*
 * enum mac80211_rx_flags - receive flags
 *
 * These flags are used with the @flag member of &struct ieee80211_rx_status.
 * @RX_FLAG_MMIC_ERROR: Michael MIC error was reported on this frame.
 *	Use together with %RX_FLAG_MMIC_STRIPPED.
 * @RX_FLAG_DECRYPTED: This frame was decrypted in hardware.
 * @RX_FLAG_MMIC_STRIPPED: the Michael MIC is stripped off this frame,
 *	verification has been done by the hardware.
 * @RX_FLAG_IV_STRIPPED: The IV and ICV are stripped from this frame.
 *	If this flag is set, the stack cannot do any replay detection
 *	hence the driver or hardware will have to do that.
 * @RX_FLAG_PN_VALIDATED: Currently only valid for CCMP/GCMP frames, this
 *	flag indicates that the PN was verified for replay protection.
 *	Note that this flag is also currently only supported when a frame
 *	is also decrypted (ie. @RX_FLAG_DECRYPTED must be set)
 * @RX_FLAG_DUP_VALIDATED: The driver should set this flag if it did
 *	de-duplication by itself.
 * @RX_FLAG_FAILED_FCS_CRC: Set this flag if the FCS check failed on
 *	the frame.
 * @RX_FLAG_FAILED_PLCP_CRC: Set this flag if the PCLP check failed on
 *	the frame.
 * @RX_FLAG_MACTIME: The timestamp passed in the RX status (@mactime
 *	field) is valid if this field is non-zero, and the position
 *	where the timestamp was sampled depends on the value.
 * @RX_FLAG_MACTIME_START: The timestamp passed in the RX status (@mactime
 *	field) is valid and contains the time the first symbol of the MPDU
 *	was received. This is useful in monitor mode and for proper IBSS
 *	merging.
 * @RX_FLAG_MACTIME_END: The timestamp passed in the RX status (@mactime
 *	field) is valid and contains the time the last symbol of the MPDU
 *	(including FCS) was received.
 * @RX_FLAG_MACTIME_PLCP_START: The timestamp passed in the RX status (@mactime
 *	field) is valid and contains the time the SYNC preamble was received.
 * @RX_FLAG_MACTIME_IS_RTAP_TS64: The timestamp passed in the RX status @mactime
 *	is only for use in the radiotap timestamp header, not otherwise a valid
 *	@mactime value. Note this is a separate flag so that we continue to see
 *	%RX_FLAG_MACTIME as unset. Also note that in this case the timestamp is
 *	reported to be 64 bits wide, not just 32.
 * @RX_FLAG_NO_SIGNAL_VAL: The signal strength value is not present.
 *	Valid only for data frames (mainly A-MPDU)
 * @RX_FLAG_AMPDU_DETAILS: A-MPDU details are known, in particular the reference
 *	number (@ampdu_reference) must be populated and be a distinct number for
 *	each A-MPDU
 * @RX_FLAG_AMPDU_LAST_KNOWN: last subframe is known, should be set on all
 *	subframes of a single A-MPDU
 * @RX_FLAG_AMPDU_IS_LAST: this subframe is the last subframe of the A-MPDU
 * @RX_FLAG_AMPDU_DELIM_CRC_ERROR: A delimiter CRC error has been detected
 *	on this subframe
 * @RX_FLAG_MIC_STRIPPED: The mic was stripped of this packet. Decryption was
 *	done by the hardware
 * @RX_FLAG_ONLY_MONITOR: Report frame only to monitor interfaces without
 *	processing it in any regular way.
 *	This is useful if drivers offload some frames but still want to report
 *	them for sniffing purposes.
 * @RX_FLAG_SKIP_MONITOR: Process and report frame to all interfaces except
 *	monitor interfaces.
 *	This is useful if drivers offload some frames but still want to report
 *	them for sniffing purposes.
 * @RX_FLAG_AMSDU_MORE: Some drivers may prefer to report separate A-MSDU
 *	subframes instead of a one huge frame for performance reasons.
 *	All, but the last MSDU from an A-MSDU should have this flag set. E.g.
 *	if an A-MSDU has 3 frames, the first 2 must have the flag set, while
 *	the 3rd (last) one must not have this flag set. The flag is used to
 *	deal with retransmission/duplication recovery properly since A-MSDU
 *	subframes share the same sequence number. Reported subframes can be
 *	either regular MSDU or singly A-MSDUs. Subframes must not be
 *	interleaved with other frames.
 * @RX_FLAG_RADIOTAP_TLV_AT_END: This frame contains radiotap TLVs in the
 *	skb->data (before the 802.11 header).
 *	If used, the SKB's mac_header pointer must be set to point
 *	to the 802.11 header after the TLVs, and any padding added after TLV
 *	data to align to 4 must be cleared by the driver putting the TLVs
 *	in the skb.
 * @RX_FLAG_ALLOW_SAME_PN: Allow the same PN as same packet before.
 *	This is used for AMSDU subframes which can have the same PN as
 *	the first subframe.
 * @RX_FLAG_ICV_STRIPPED: The ICV is stripped from this frame. CRC checking must
 *	be done in the hardware.
 * @RX_FLAG_AMPDU_EOF_BIT: Value of the EOF bit in the A-MPDU delimiter for this
 *	frame
 * @RX_FLAG_AMPDU_EOF_BIT_KNOWN: The EOF value is known
 * @RX_FLAG_RADIOTAP_HE: HE radiotap data is present
 *	(&struct ieee80211_radiotap_he, mac80211 will fill in
 *
 *	 - DATA3_DATA_MCS
 *	 - DATA3_DATA_DCM
 *	 - DATA3_CODING
 *	 - DATA5_GI
 *	 - DATA5_DATA_BW_RU_ALLOC
 *	 - DATA6_NSTS
 *	 - DATA3_STBC
 *
 *	from the RX info data, so leave those zeroed when building this data)
 * @RX_FLAG_RADIOTAP_HE_MU: HE MU radiotap data is present
 *	(&struct ieee80211_radiotap_he_mu)
 * @RX_FLAG_RADIOTAP_LSIG: L-SIG radiotap data is present
 * @RX_FLAG_NO_PSDU: use the frame only for radiotap reporting, with
 *	the "0-length PSDU" field included there.  The value for it is
 *	in &struct ieee80211_rx_status.  Note that if this value isn't
 *	known the frame shouldn't be reported.
 * @RX_FLAG_8023: the frame has an 802.3 header (decap offload performed by
 *	hardware or driver)
 * @RX_FLAG_RADIOTAP_VHT: VHT radiotap data is present
 */
enum mac80211_rx_flags {
	RX_FLAG_MMIC_ERROR		= BIT(0),
	RX_FLAG_DECRYPTED		= BIT(1),
	RX_FLAG_ONLY_MONITOR		= BIT(2),
	RX_FLAG_MMIC_STRIPPED		= BIT(3),
	RX_FLAG_IV_STRIPPED		= BIT(4),
	RX_FLAG_FAILED_FCS_CRC		= BIT(5),
	RX_FLAG_FAILED_PLCP_CRC 	= BIT(6),
	RX_FLAG_MACTIME_IS_RTAP_TS64	= BIT(7),
	RX_FLAG_NO_SIGNAL_VAL		= BIT(8),
	RX_FLAG_AMPDU_DETAILS		= BIT(9),
	RX_FLAG_PN_VALIDATED		= BIT(10),
	RX_FLAG_DUP_VALIDATED		= BIT(11),
	RX_FLAG_AMPDU_LAST_KNOWN	= BIT(12),
	RX_FLAG_AMPDU_IS_LAST		= BIT(13),
	RX_FLAG_AMPDU_DELIM_CRC_ERROR	= BIT(14),
	/* one free bit at 15 */
	RX_FLAG_MACTIME			= BIT(16) | BIT(17),
	RX_FLAG_MACTIME_PLCP_START	= 1 << 16,
	RX_FLAG_MACTIME_START		= 2 << 16,
	RX_FLAG_MACTIME_END		= 3 << 16,
	RX_FLAG_SKIP_MONITOR		= BIT(18),
	RX_FLAG_AMSDU_MORE		= BIT(19),
	RX_FLAG_RADIOTAP_TLV_AT_END	= BIT(20),
	RX_FLAG_MIC_STRIPPED		= BIT(21),
	RX_FLAG_ALLOW_SAME_PN		= BIT(22),
	RX_FLAG_ICV_STRIPPED		= BIT(23),
	RX_FLAG_AMPDU_EOF_BIT		= BIT(24),
	RX_FLAG_AMPDU_EOF_BIT_KNOWN	= BIT(25),
	RX_FLAG_RADIOTAP_HE		= BIT(26),
	RX_FLAG_RADIOTAP_HE_MU		= BIT(27),
	RX_FLAG_RADIOTAP_LSIG		= BIT(28),
	RX_FLAG_NO_PSDU			= BIT(29),
	RX_FLAG_8023			= BIT(30),
	RX_FLAG_RADIOTAP_VHT		= BIT(31),
};

/*
 * enum ieee80211_packet_rx_flags - packet RX flags
 * @IEEE80211_RX_AMSDU: a-MSDU packet
 * @IEEE80211_RX_MALFORMED_ACTION_FRM: action frame is malformed
 * @IEEE80211_RX_DEFERRED_RELEASE: frame was subjected to receive reordering
 *
 * These are per-frame flags that are attached to a frame in the
 * @rx_flags field of &struct ieee80211_rx_status.
 */
enum ieee80211_packet_rx_flags {
	IEEE80211_RX_AMSDU			= BIT(3),
	IEEE80211_RX_MALFORMED_ACTION_FRM	= BIT(4),
	IEEE80211_RX_DEFERRED_RELEASE		= BIT(5),
};

enum ieee80211_radiotap_zero_len_psdu_type {
	IEEE80211_RADIOTAP_ZERO_LEN_PSDU_SOUNDING	= 0,
	IEEE80211_RADIOTAP_ZERO_LEN_PSDU_NOT_CAPTURED	= 1,
	IEEE80211_RADIOTAP_ZERO_LEN_PSDU_VENDOR		= 0xff,
};

/*
 * enum ieee80211_sta_info_flags - Stations flags
 *
 * These flags are used with &struct sta_info's @flags member, but
 * only indirectly with set_sta_flag() and friends.
 *
 * @WLAN_STA_AUTH: Station is authenticated.
 * @WLAN_STA_ASSOC: Station is associated.
 * @WLAN_STA_PS_STA: Station is in power-save mode
 * @WLAN_STA_AUTHORIZED: Station is authorized to send/receive traffic.
 *	This bit is always checked so needs to be enabled for all stations
 *	when virtual port control is not in use.
 * @WLAN_STA_SHORT_PREAMBLE: Station is capable of receiving short-preamble
 *	frames.
 * @WLAN_STA_WDS: Station is one of our WDS peers.
 * @WLAN_STA_CLEAR_PS_FILT: Clear PS filter in hardware (using the
 *	IEEE80211_TX_CTL_CLEAR_PS_FILT control flag) when the next
 *	frame to this station is transmitted.
 * @WLAN_STA_MFP: Management frame protection is used with this STA.
 * @WLAN_STA_BLOCK_BA: Used to deny ADDBA requests (both TX and RX)
 *	during suspend/resume and station removal.
 * @WLAN_STA_PS_DRIVER: driver requires keeping this station in
 *	power-save mode logically to flush frames that might still
 *	be in the queues
 * @WLAN_STA_PSPOLL: Station sent PS-poll while driver was keeping
 *	station in power-save mode, reply when the driver unblocks.
 * @WLAN_STA_TDLS_PEER: Station is a TDLS peer.
 * @WLAN_STA_TDLS_PEER_AUTH: This TDLS peer is authorized to send direct
 *	packets. This means the link is enabled.
 * @WLAN_STA_TDLS_INITIATOR: We are the initiator of the TDLS link with this
 *	station.
 * @WLAN_STA_TDLS_CHAN_SWITCH: This TDLS peer supports TDLS channel-switching
 * @WLAN_STA_TDLS_OFF_CHANNEL: The local STA is currently off-channel with this
 *	TDLS peer
 * @WLAN_STA_TDLS_WIDER_BW: This TDLS peer supports working on a wider bw on
 *	the BSS base channel.
 * @WLAN_STA_UAPSD: Station requested unscheduled SP while driver was
 *	keeping station in power-save mode, reply when the driver
 *	unblocks the station.
 * @WLAN_STA_SP: Station is in a service period, so don't try to
 *	reply to other uAPSD trigger frames or PS-Poll.
 * @WLAN_STA_4ADDR_EVENT: 4-addr event was already sent for this frame.
 * @WLAN_STA_INSERTED: This station is inserted into the hash table.
 * @WLAN_STA_RATE_CONTROL: rate control was initialized for this station.
 * @WLAN_STA_TOFFSET_KNOWN: toffset calculated for this station is valid.
 * @WLAN_STA_MPSP_OWNER: local STA is owner of a mesh Peer Service Period.
 * @WLAN_STA_MPSP_RECIPIENT: local STA is recipient of a MPSP.
 * @WLAN_STA_PS_DELIVER: station woke up, but we're still blocking TX
 *	until pending frames are delivered
 * @WLAN_STA_USES_ENCRYPTION: This station was configured for encryption,
 *	so drop all packets without a key later.
 * @WLAN_STA_DECAP_OFFLOAD: This station uses rx decap offload
 *
 * @NUM_WLAN_STA_FLAGS: number of defined flags
 */
enum ieee80211_sta_info_flags {
	WLAN_STA_AUTH,
	WLAN_STA_ASSOC,
	WLAN_STA_PS_STA,
	WLAN_STA_AUTHORIZED,
	WLAN_STA_SHORT_PREAMBLE,
	WLAN_STA_WDS,
	WLAN_STA_CLEAR_PS_FILT,
	WLAN_STA_MFP,
	WLAN_STA_BLOCK_BA,
	WLAN_STA_PS_DRIVER,
	WLAN_STA_PSPOLL,
	WLAN_STA_TDLS_PEER,
	WLAN_STA_TDLS_PEER_AUTH,
	WLAN_STA_TDLS_INITIATOR,
	WLAN_STA_TDLS_CHAN_SWITCH,
	WLAN_STA_TDLS_OFF_CHANNEL,
	WLAN_STA_TDLS_WIDER_BW,
	WLAN_STA_UAPSD,
	WLAN_STA_SP,
	WLAN_STA_4ADDR_EVENT,
	WLAN_STA_INSERTED,
	WLAN_STA_RATE_CONTROL,
	WLAN_STA_TOFFSET_KNOWN,
	WLAN_STA_MPSP_OWNER,
	WLAN_STA_MPSP_RECIPIENT,
	WLAN_STA_PS_DELIVER,
	WLAN_STA_USES_ENCRYPTION,
	WLAN_STA_DECAP_OFFLOAD,

	NUM_WLAN_STA_FLAGS,
};

/*
 * enum ieee80211_sdata_state_bits - virtual interface state bits
 * @SDATA_STATE_RUNNING: virtual interface is up & running; this
 *	mirrors netif_running() but is separate for interface type
 *	change handling while the interface is up
 * @SDATA_STATE_OFFCHANNEL: This interface is currently in offchannel
 *	mode, so queues are stopped
 * @SDATA_STATE_OFFCHANNEL_BEACON_STOPPED: Beaconing was stopped due
 *	to offchannel, reset when offchannel returns
 */
enum ieee80211_sdata_state_bits {
	SDATA_STATE_RUNNING,
	SDATA_STATE_OFFCHANNEL,
	SDATA_STATE_OFFCHANNEL_BEACON_STOPPED,
};

/*
 * enum ieee80211_ac_numbers - AC numbers as used in mac80211
 * @IEEE80211_AC_VO: voice
 * @IEEE80211_AC_VI: video
 * @IEEE80211_AC_BE: best effort
 * @IEEE80211_AC_BK: background
 */
enum ieee80211_ac_numbers {
	IEEE80211_AC_VO		= 0,
	IEEE80211_AC_VI		= 1,
	IEEE80211_AC_BE		= 2,
	IEEE80211_AC_BK		= 3,
};

/*
 * enum ieee80211_ampdu_mlme_action - A-MPDU actions
 *
 * These flags are used with the ampdu_action() callback in
 * &struct ieee80211_ops to indicate which action is needed.
 *
 * Note that drivers MUST be able to deal with a TX aggregation
 * session being stopped even before they OK'ed starting it by
 * calling ieee80211_start_tx_ba_cb_irqsafe, because the peer
 * might receive the addBA frame and send a delBA right away!
 *
 * @IEEE80211_AMPDU_RX_START: start RX aggregation
 * @IEEE80211_AMPDU_RX_STOP: stop RX aggregation
 * @IEEE80211_AMPDU_TX_START: start TX aggregation, the driver must either
 *	call ieee80211_start_tx_ba_cb_irqsafe() or
 *	call ieee80211_start_tx_ba_cb_irqsafe() with status
 *	%IEEE80211_AMPDU_TX_START_DELAY_ADDBA to delay addba after
 *	ieee80211_start_tx_ba_cb_irqsafe is called, or just return the special
 *	status %IEEE80211_AMPDU_TX_START_IMMEDIATE.
 * @IEEE80211_AMPDU_TX_OPERATIONAL: TX aggregation has become operational
 * @IEEE80211_AMPDU_TX_STOP_CONT: stop TX aggregation but continue transmitting
 *	queued packets, now unaggregated. After all packets are transmitted the
 *	driver has to call ieee80211_stop_tx_ba_cb_irqsafe().
 * @IEEE80211_AMPDU_TX_STOP_FLUSH: stop TX aggregation and flush all packets,
 *	called when the station is removed. There's no need or reason to call
 *	ieee80211_stop_tx_ba_cb_irqsafe() in this case as mac80211 assumes the
 *	session is gone and removes the station.
 * @IEEE80211_AMPDU_TX_STOP_FLUSH_CONT: called when TX aggregation is stopped
 *	but the driver hasn't called ieee80211_stop_tx_ba_cb_irqsafe() yet and
 *	now the connection is dropped and the station will be removed. Drivers
 *	should clean up and drop remaining packets when this is called.
 */
enum ieee80211_ampdu_mlme_action {
	IEEE80211_AMPDU_RX_START,
	IEEE80211_AMPDU_RX_STOP,
	IEEE80211_AMPDU_TX_START,
	IEEE80211_AMPDU_TX_STOP_CONT,
	IEEE80211_AMPDU_TX_STOP_FLUSH,
	IEEE80211_AMPDU_TX_STOP_FLUSH_CONT,
	IEEE80211_AMPDU_TX_OPERATIONAL,
};

#define UL(x)		(_UL(x))
#define ULL(x)		(_ULL(x))
#define BIT(nr) 	(UL(1) << (nr))
#define BIT_ULL(nr)	(ULL(1) << (nr))

/* enum rate_control_capabilities - rate control capabilities */
enum rate_control_capabilities {
	/*
	 * @RATE_CTRL_CAPA_VHT_EXT_NSS_BW:
	 * Support for extended NSS BW support (dot11VHTExtendedNSSCapable)
	 * Note that this is only looked at if the minimum number of chains
	 * that the AP uses is < the number of TX chains the hardware has,
	 * otherwise the NSS difference doesn't bother us.
	 */
	RATE_CTRL_CAPA_VHT_EXT_NSS_BW = BIT(0),
	/*
	 * @RATE_CTRL_CAPA_AMPDU_TRIGGER:
	 * mac80211 should start A-MPDU sessions on tx
	 */
	RATE_CTRL_CAPA_AMPDU_TRIGGER = BIT(1),
};

/*
 * enum mac80211_tx_info_flags - flags to describe transmission information/status
 *
 * These flags are used with the @flags member of &ieee80211_tx_info.
 *
 * @IEEE80211_TX_CTL_REQ_TX_STATUS: require TX status callback for this frame.
 * @IEEE80211_TX_CTL_ASSIGN_SEQ: The driver has to assign a sequence
 *	number to this frame, taking care of not overwriting the fragment
 *	number and increasing the sequence number only when the
 *	IEEE80211_TX_CTL_FIRST_FRAGMENT flag is set. mac80211 will properly
 *	assign sequence numbers to QoS-data frames but cannot do so correctly
 *	for non-QoS-data and management frames because beacons need them from
 *	that counter as well and mac80211 cannot guarantee proper sequencing.
 *	If this flag is set, the driver should instruct the hardware to
 *	assign a sequence number to the frame or assign one itself. Cf. IEEE
 *	802.11-2007 7.1.3.4.1 paragraph 3. This flag will always be set for
 *	beacons and always be clear for frames without a sequence number field.
 * @IEEE80211_TX_CTL_NO_ACK: tell the low level not to wait for an ack
 * @IEEE80211_TX_CTL_CLEAR_PS_FILT: clear powersave filter for destination
 *	station
 * @IEEE80211_TX_CTL_FIRST_FRAGMENT: this is a first fragment of the frame
 * @IEEE80211_TX_CTL_SEND_AFTER_DTIM: send this frame after DTIM beacon
 * @IEEE80211_TX_CTL_AMPDU: this frame should be sent as part of an A-MPDU
 * @IEEE80211_TX_CTL_INJECTED: Frame was injected, internal to mac80211.
 * @IEEE80211_TX_STAT_TX_FILTERED: The frame was not transmitted
 *	because the destination STA was in powersave mode. Note that to
 *	avoid race conditions, the filter must be set by the hardware or
 *	firmware upon receiving a frame that indicates that the station
 *	went to sleep (must be done on device to filter frames already on
 *	the queue) and may only be unset after mac80211 gives the OK for
 *	that by setting the IEEE80211_TX_CTL_CLEAR_PS_FILT (see above),
 *	since only then is it guaranteed that no more frames are in the
 *	hardware queue.
 * @IEEE80211_TX_STAT_ACK: Frame was acknowledged
 * @IEEE80211_TX_STAT_AMPDU: The frame was aggregated, so status
 * 	is for the whole aggregation.
 * @IEEE80211_TX_STAT_AMPDU_NO_BACK: no block ack was returned,
 * 	so consider using block ack request (BAR).
 * @IEEE80211_TX_CTL_RATE_CTRL_PROBE: internal to mac80211, can be
 *	set by rate control algorithms to indicate probe rate, will
 *	be cleared for fragmented frames (except on the last fragment)
 * @IEEE80211_TX_INTFL_OFFCHAN_TX_OK: Internal to mac80211. Used to indicate
 *	that a frame can be transmitted while the queues are stopped for
 *	off-channel operation.
 * @IEEE80211_TX_CTL_HW_80211_ENCAP: This frame uses hardware encapsulation
 *	(header conversion)
 * @IEEE80211_TX_INTFL_RETRIED: completely internal to mac80211,
 *	used to indicate that a frame was already retried due to PS
 * @IEEE80211_TX_INTFL_DONT_ENCRYPT: completely internal to mac80211,
 *	used to indicate frame should not be encrypted
 * @IEEE80211_TX_CTL_NO_PS_BUFFER: This frame is a response to a poll
 *	frame (PS-Poll or uAPSD) or a non-bufferable MMPDU and must
 *	be sent although the station is in powersave mode.
 * @IEEE80211_TX_CTL_MORE_FRAMES: More frames will be passed to the
 *	transmit function after the current frame, this can be used
 *	by drivers to kick the DMA queue only if unset or when the
 *	queue gets full.
 * @IEEE80211_TX_INTFL_RETRANSMISSION: This frame is being retransmitted
 *	after TX status because the destination was asleep, it must not
 *	be modified again (no seqno assignment, crypto, etc.)
 * @IEEE80211_TX_INTFL_MLME_CONN_TX: This frame was transmitted by the MLME
 *	code for connection establishment, this indicates that its status
 *	should kick the MLME state machine.
 * @IEEE80211_TX_INTFL_NL80211_FRAME_TX: Frame was requested through nl80211
 *	MLME command (internal to mac80211 to figure out whether to send TX
 *	status to user space)
 * @IEEE80211_TX_CTL_LDPC: tells the driver to use LDPC for this frame
 * @IEEE80211_TX_CTL_STBC: Enables Space-Time Block Coding (STBC) for this
 *	frame and selects the maximum number of streams that it can use.
 * @IEEE80211_TX_CTL_TX_OFFCHAN: Marks this packet to be transmitted on
 *	the off-channel channel when a remain-on-channel offload is done
 *	in hardware -- normal packets still flow and are expected to be
 *	handled properly by the device.
 * @IEEE80211_TX_INTFL_TKIP_MIC_FAILURE: Marks this packet to be used for TKIP
 *	testing. It will be sent out with incorrect Michael MIC key to allow
 *	TKIP countermeasures to be tested.
 * @IEEE80211_TX_CTL_NO_CCK_RATE: This frame will be sent at non CCK rate.
 *	This flag is actually used for management frame especially for P2P
 *	frames not being sent at CCK rate in 2GHz band.
 * @IEEE80211_TX_STATUS_EOSP: This packet marks the end of service period,
 *	when its status is reported the service period ends. For frames in
 *	an SP that mac80211 transmits, it is already set; for driver frames
 *	the driver may set this flag. It is also used to do the same for
 *	PS-Poll responses.
 * @IEEE80211_TX_CTL_USE_MINRATE: This frame will be sent at lowest rate.
 *	This flag is used to send nullfunc frame at minimum rate when
 *	the nullfunc is used for connection monitoring purpose.
 * @IEEE80211_TX_CTL_DONTFRAG: Don't fragment this packet even if it
 *	would be fragmented by size (this is optional, only used for
 *	monitor injection).
 * @IEEE80211_TX_STAT_NOACK_TRANSMITTED: A frame that was marked with
 *	IEEE80211_TX_CTL_NO_ACK has been successfully transmitted without
 *	any errors (like issues specific to the driver/HW).
 *	This flag must not be set for frames that don't request no-ack
 *	behaviour with IEEE80211_TX_CTL_NO_ACK.
 *
 * Note: If you have to add new flags to the enumeration, then don't
 *	 forget to update %IEEE80211_TX_TEMPORARY_FLAGS when necessary.
 */
enum mac80211_tx_info_flags {
	IEEE80211_TX_CTL_REQ_TX_STATUS		= BIT(0),
	IEEE80211_TX_CTL_ASSIGN_SEQ		= BIT(1),
	IEEE80211_TX_CTL_NO_ACK			= BIT(2),
	IEEE80211_TX_CTL_CLEAR_PS_FILT		= BIT(3),
	IEEE80211_TX_CTL_FIRST_FRAGMENT		= BIT(4),
	IEEE80211_TX_CTL_SEND_AFTER_DTIM	= BIT(5),
	IEEE80211_TX_CTL_AMPDU			= BIT(6),
	IEEE80211_TX_CTL_INJECTED		= BIT(7),
	IEEE80211_TX_STAT_TX_FILTERED		= BIT(8),
	IEEE80211_TX_STAT_ACK			= BIT(9),
	IEEE80211_TX_STAT_AMPDU			= BIT(10),
	IEEE80211_TX_STAT_AMPDU_NO_BACK		= BIT(11),
	IEEE80211_TX_CTL_RATE_CTRL_PROBE	= BIT(12),
	IEEE80211_TX_INTFL_OFFCHAN_TX_OK	= BIT(13),
	IEEE80211_TX_CTL_HW_80211_ENCAP		= BIT(14),
	IEEE80211_TX_INTFL_RETRIED		= BIT(15),
	IEEE80211_TX_INTFL_DONT_ENCRYPT		= BIT(16),
	IEEE80211_TX_CTL_NO_PS_BUFFER		= BIT(17),
	IEEE80211_TX_CTL_MORE_FRAMES		= BIT(18),
	IEEE80211_TX_INTFL_RETRANSMISSION	= BIT(19),
	IEEE80211_TX_INTFL_MLME_CONN_TX		= BIT(20),
	IEEE80211_TX_INTFL_NL80211_FRAME_TX	= BIT(21),
	IEEE80211_TX_CTL_LDPC			= BIT(22),
	IEEE80211_TX_CTL_STBC			= BIT(23) | BIT(24),
	IEEE80211_TX_CTL_TX_OFFCHAN		= BIT(25),
	IEEE80211_TX_INTFL_TKIP_MIC_FAILURE	= BIT(26),
	IEEE80211_TX_CTL_NO_CCK_RATE		= BIT(27),
	IEEE80211_TX_STATUS_EOSP		= BIT(28),
	IEEE80211_TX_CTL_USE_MINRATE		= BIT(29),
	IEEE80211_TX_CTL_DONTFRAG		= BIT(30),
	IEEE80211_TX_STAT_NOACK_TRANSMITTED	= BIT(31),
};

enum mac80211_tx_control_flags {
	IEEE80211_TX_CTRL_PORT_CTRL_PROTO	= BIT(0),
	IEEE80211_TX_CTRL_PS_RESPONSE		= BIT(1),
	IEEE80211_TX_CTRL_RATE_INJECT		= BIT(2),
	IEEE80211_TX_CTRL_AMSDU			= BIT(3),
	IEEE80211_TX_CTRL_FAST_XMIT		= BIT(4),
	IEEE80211_TX_CTRL_SKIP_MPATH_LOOKUP	= BIT(5),
	IEEE80211_TX_INTCFL_NEED_TXPROCESSING	= BIT(6),
	IEEE80211_TX_CTRL_NO_SEQNO		= BIT(7),
	IEEE80211_TX_CTRL_DONT_REORDER		= BIT(8),
	IEEE80211_TX_CTRL_MCAST_MLO_FIRST_TX	= BIT(9),
	IEEE80211_TX_CTRL_DONT_USE_RATE_MASK	= BIT(10),
	IEEE80211_TX_CTRL_MLO_LINK		= 0xf0000000,
};

enum ieee80211_conf_changed {
	IEEE80211_CONF_CHANGE_SMPS		= BIT(1),
	IEEE80211_CONF_CHANGE_LISTEN_INTERVAL	= BIT(2),
	IEEE80211_CONF_CHANGE_MONITOR		= BIT(3),
	IEEE80211_CONF_CHANGE_PS		= BIT(4),
	IEEE80211_CONF_CHANGE_POWER		= BIT(5),
	IEEE80211_CONF_CHANGE_CHANNEL		= BIT(6),
	IEEE80211_CONF_CHANGE_RETRY_LIMITS	= BIT(7),
	IEEE80211_CONF_CHANGE_IDLE		= BIT(8),
};

/*
 * enum ieee80211_channel_flags - channel flags
 *
 * Channel flags set by the regulatory control code.
 *
 * @IEEE80211_CHAN_DISABLED: This channel is disabled.
 * @IEEE80211_CHAN_NO_IR: do not initiate radiation, this includes
 *	sending probe requests or beaconing.
 * @IEEE80211_CHAN_PSD: Power spectral density (in dBm) is set for this
 *	channel.
 * @IEEE80211_CHAN_RADAR: Radar detection is required on this channel.
 * @IEEE80211_CHAN_NO_HT40PLUS: extension channel above this channel
 *	is not permitted.
 * @IEEE80211_CHAN_NO_HT40MINUS: extension channel below this channel
 *	is not permitted.
 * @IEEE80211_CHAN_NO_OFDM: OFDM is not allowed on this channel.
 * @IEEE80211_CHAN_NO_80MHZ: If the driver supports 80 MHz on the band,
 *	this flag indicates that an 80 MHz channel cannot use this
 *	channel as the control or any of the secondary channels.
 *	This may be due to the driver or due to regulatory bandwidth
 *	restrictions.
 * @IEEE80211_CHAN_NO_160MHZ: If the driver supports 160 MHz on the band,
 *	this flag indicates that an 160 MHz channel cannot use this
 *	channel as the control or any of the secondary channels.
 *	This may be due to the driver or due to regulatory bandwidth
 *	restrictions.
 * @IEEE80211_CHAN_INDOOR_ONLY: see %NL80211_FREQUENCY_ATTR_INDOOR_ONLY
 * @IEEE80211_CHAN_IR_CONCURRENT: see %NL80211_FREQUENCY_ATTR_IR_CONCURRENT
 * @IEEE80211_CHAN_NO_20MHZ: 20 MHz bandwidth is not permitted
 *	on this channel.
 * @IEEE80211_CHAN_NO_10MHZ: 10 MHz bandwidth is not permitted
 *	on this channel.
 * @IEEE80211_CHAN_NO_HE: HE operation is not permitted on this channel.
 * @IEEE80211_CHAN_NO_320MHZ: If the driver supports 320 MHz on the band,
 *	this flag indicates that a 320 MHz channel cannot use this
 *	channel as the control or any of the secondary channels.
 *	This may be due to the driver or due to regulatory bandwidth
 *	restrictions.
 * @IEEE80211_CHAN_NO_EHT: EHT operation is not permitted on this channel.
 * @IEEE80211_CHAN_DFS_CONCURRENT: See %NL80211_RRF_DFS_CONCURRENT
 * @IEEE80211_CHAN_NO_6GHZ_VLP_CLIENT: Client connection with VLP AP
 *	not permitted using this channel
 * @IEEE80211_CHAN_NO_6GHZ_AFC_CLIENT: Client connection with AFC AP
 *	not permitted using this channel
 * @IEEE80211_CHAN_CAN_MONITOR: This channel can be used for monitor
 *	mode even in the presence of other (regulatory) restrictions,
 *	even if it is otherwise disabled.
 * @IEEE80211_CHAN_ALLOW_6GHZ_VLP_AP: Allow using this channel for AP operation
 *	with very low power (VLP), even if otherwise set to NO_IR.
 * @IEEE80211_CHAN_ALLOW_20MHZ_ACTIVITY: Allow activity on a 20 MHz channel,
 *	even if otherwise set to NO_IR.
 * @IEEE80211_CHAN_S1G_NO_PRIMARY: Prevents the channel for use as an S1G
 *	primary channel. Does not prevent the wider operating channel
 *	described by the chandef from being used. In order for a 2MHz primary
 *	to be used, both 1MHz subchannels shall not contain this flag.
 * @IEEE80211_CHAN_NO_4MHZ: 4 MHz bandwidth is not permitted on this channel.
 * @IEEE80211_CHAN_NO_8MHZ: 8 MHz bandwidth is not permitted on this channel.
 * @IEEE80211_CHAN_NO_16MHZ: 16 MHz bandwidth is not permitted on this channel.
 */
enum ieee80211_channel_flags {
	IEEE80211_CHAN_DISABLED			= BIT(0),
	IEEE80211_CHAN_NO_IR			= BIT(1),
	IEEE80211_CHAN_PSD			= BIT(2),
	IEEE80211_CHAN_RADAR			= BIT(3),
	IEEE80211_CHAN_NO_HT40PLUS		= BIT(4),
	IEEE80211_CHAN_NO_HT40MINUS		= BIT(5),
	IEEE80211_CHAN_NO_OFDM			= BIT(6),
	IEEE80211_CHAN_NO_80MHZ			= BIT(7),
	IEEE80211_CHAN_NO_160MHZ		= BIT(8),
	IEEE80211_CHAN_INDOOR_ONLY		= BIT(9),
	IEEE80211_CHAN_IR_CONCURRENT		= BIT(10),
	IEEE80211_CHAN_NO_20MHZ			= BIT(11),
	IEEE80211_CHAN_NO_10MHZ			= BIT(12),
	IEEE80211_CHAN_NO_HE			= BIT(13),
	/* can use free bits here */
	IEEE80211_CHAN_NO_320MHZ		= BIT(19),
	IEEE80211_CHAN_NO_EHT			= BIT(20),
	IEEE80211_CHAN_DFS_CONCURRENT		= BIT(21),
	IEEE80211_CHAN_NO_6GHZ_VLP_CLIENT	= BIT(22),
	IEEE80211_CHAN_NO_6GHZ_AFC_CLIENT	= BIT(23),
	IEEE80211_CHAN_CAN_MONITOR		= BIT(24),
	IEEE80211_CHAN_ALLOW_6GHZ_VLP_AP	= BIT(25),
	IEEE80211_CHAN_ALLOW_20MHZ_ACTIVITY     = BIT(26),
	IEEE80211_CHAN_S1G_NO_PRIMARY		= BIT(27),
	IEEE80211_CHAN_NO_4MHZ			= BIT(28),
	IEEE80211_CHAN_NO_8MHZ			= BIT(29),
	IEEE80211_CHAN_NO_16MHZ			= BIT(30),
};

/*
 * enum cfg80211_signal_type - signal type
 *
 * @CFG80211_SIGNAL_TYPE_NONE: no signal strength information available
 * @CFG80211_SIGNAL_TYPE_MBM: signal strength in mBm (100*dBm)
 * @CFG80211_SIGNAL_TYPE_UNSPEC: signal strength, increasing from 0 through 100
 */
enum cfg80211_signal_type {
	CFG80211_SIGNAL_TYPE_NONE,
	CFG80211_SIGNAL_TYPE_MBM,
	CFG80211_SIGNAL_TYPE_UNSPEC,
};

/*
 * enum rfkill_hard_block_reasons - hard block reasons
 * @RFKILL_HARD_BLOCK_SIGNAL: the hardware rfkill signal is active
 * @RFKILL_HARD_BLOCK_NOT_OWNER: the NIC is not owned by the host
 */
enum rfkill_hard_block_reasons {
	RFKILL_HARD_BLOCK_SIGNAL	= 1 << 0,
	RFKILL_HARD_BLOCK_NOT_OWNER	= 1 << 1,
};

/* flags used in struct ieee80211_if_managed.flags */
enum ieee80211_sta_flags {
	IEEE80211_STA_CONNECTION_POLL	= BIT(1),
	IEEE80211_STA_CONTROL_PORT	= BIT(2),
	IEEE80211_STA_MFP_ENABLED	= BIT(6),
	IEEE80211_STA_UAPSD_ENABLED	= BIT(7),
	IEEE80211_STA_NULLFUNC_ACKED	= BIT(8),
	IEEE80211_STA_ENABLE_RRM	= BIT(15),
};

enum ieee80211_conn_mode {
	IEEE80211_CONN_MODE_S1G,
	IEEE80211_CONN_MODE_LEGACY,
	IEEE80211_CONN_MODE_HT,
	IEEE80211_CONN_MODE_VHT,
	IEEE80211_CONN_MODE_HE,
	IEEE80211_CONN_MODE_EHT,
};

/*
 * enum ieee80211_conf_flags - configuration flags
 *
 * Flags to define PHY configuration options
 *
 * @IEEE80211_CONF_MONITOR: there's a monitor interface present -- use this
 *	to determine for example whether to calculate timestamps for packets
 *	or not, do not use instead of filter flags!
 * @IEEE80211_CONF_PS: Enable 802.11 power save mode (managed mode only).
 *	This is the power save mode defined by IEEE 802.11-2007 section 11.2,
 *	meaning that the hardware still wakes up for beacons, is able to
 *	transmit frames and receive the possible acknowledgment frames.
 *	Not to be confused with hardware specific wakeup/sleep states,
 *	driver is responsible for that. See the section "Powersave support"
 *	for more.
 * @IEEE80211_CONF_IDLE: The device is running, but idle; if the flag is set
 *	the driver should be prepared to handle configuration requests but
 *	may turn the device off as much as possible. Typically, this flag will
 *	be set when an interface is set UP but not associated or scanning, but
 *	it can also be unset in that case when monitor interfaces are active.
 * @IEEE80211_CONF_OFFCHANNEL: The device is currently not on its main
 *	operating channel.
 */
enum ieee80211_conf_flags {
	IEEE80211_CONF_MONITOR		= (1<<0),
	IEEE80211_CONF_PS		= (1<<1),
	IEEE80211_CONF_IDLE		= (1<<2),
	IEEE80211_CONF_OFFCHANNEL	= (1<<3),
};

/*
 * enum ieee80211_bss_change - BSS change notification flags
 *
 * These flags are used with the bss_info_changed(), link_info_changed()
 * and vif_cfg_changed() callbacks to indicate which parameter(s) changed.
 *
 * @BSS_CHANGED_ASSOC: association status changed (associated/disassociated),
 *	also implies a change in the AID.
 * @BSS_CHANGED_ERP_CTS_PROT: CTS protection changed
 * @BSS_CHANGED_ERP_PREAMBLE: preamble changed
 * @BSS_CHANGED_ERP_SLOT: slot timing changed
 * @BSS_CHANGED_HT: 802.11n parameters changed
 * @BSS_CHANGED_BASIC_RATES: Basic rateset changed
 * @BSS_CHANGED_BEACON_INT: Beacon interval changed
 * @BSS_CHANGED_BSSID: BSSID changed, for whatever
 *	reason (IBSS and managed mode)
 * @BSS_CHANGED_BEACON: Beacon data changed, retrieve
 *	new beacon (beaconing modes)
 * @BSS_CHANGED_BEACON_ENABLED: Beaconing should be
 *	enabled/disabled (beaconing modes)
 * @BSS_CHANGED_CQM: Connection quality monitor config changed
 * @BSS_CHANGED_IBSS: IBSS join status changed
 * @BSS_CHANGED_ARP_FILTER: Hardware ARP filter address list or state changed.
 * @BSS_CHANGED_QOS: QoS for this association was enabled/disabled. Note
 *	that it is only ever disabled for station mode.
 * @BSS_CHANGED_IDLE: Idle changed for this BSS/interface.
 * @BSS_CHANGED_SSID: SSID changed for this BSS (AP and IBSS mode)
 * @BSS_CHANGED_AP_PROBE_RESP: Probe Response changed for this BSS (AP mode)
 * @BSS_CHANGED_PS: PS changed for this BSS (STA mode)
 * @BSS_CHANGED_TXPOWER: TX power setting changed for this interface
 * @BSS_CHANGED_P2P_PS: P2P powersave settings (CTWindow, opportunistic PS)
 *	changed
 * @BSS_CHANGED_BEACON_INFO: Data from the AP's beacon became available:
 *	currently dtim_period only is under consideration.
 * @BSS_CHANGED_BANDWIDTH: The bandwidth used by this interface changed,
 *	note that this is only called when it changes after the channel
 *	context had been assigned.
 * @BSS_CHANGED_OCB: OCB join status changed
 * @BSS_CHANGED_MU_GROUPS: VHT MU-MIMO group id or user position changed
 * @BSS_CHANGED_KEEP_ALIVE: keep alive options (idle period or protected
 *	keep alive) changed.
 * @BSS_CHANGED_MCAST_RATE: Multicast Rate setting changed for this interface
 * @BSS_CHANGED_FTM_RESPONDER: fine timing measurement request responder
 *	functionality changed for this BSS (AP mode).
 * @BSS_CHANGED_TWT: TWT status changed
 * @BSS_CHANGED_HE_OBSS_PD: OBSS Packet Detection status changed.
 * @BSS_CHANGED_HE_BSS_COLOR: BSS Color has changed
 * @BSS_CHANGED_FILS_DISCOVERY: FILS discovery status changed.
 * @BSS_CHANGED_UNSOL_BCAST_PROBE_RESP: Unsolicited broadcast probe response
 *	status changed.
 * @BSS_CHANGED_MLD_VALID_LINKS: MLD valid links status changed.
 * @BSS_CHANGED_MLD_TTLM: negotiated TID to link mapping was changed
 * @BSS_CHANGED_TPE: transmit power envelope changed
 */
enum ieee80211_bss_change {
	BSS_CHANGED_ASSOC		= 1<<0,
	BSS_CHANGED_ERP_CTS_PROT	= 1<<1,
	BSS_CHANGED_ERP_PREAMBLE	= 1<<2,
	BSS_CHANGED_ERP_SLOT		= 1<<3,
	BSS_CHANGED_HT			= 1<<4,
	BSS_CHANGED_BASIC_RATES		= 1<<5,
	BSS_CHANGED_BEACON_INT		= 1<<6,
	BSS_CHANGED_BSSID		= 1<<7,
	BSS_CHANGED_BEACON		= 1<<8,
	BSS_CHANGED_BEACON_ENABLED	= 1<<9,
	BSS_CHANGED_CQM			= 1<<10,
	BSS_CHANGED_IBSS		= 1<<11,
	BSS_CHANGED_ARP_FILTER		= 1<<12,
	BSS_CHANGED_QOS			= 1<<13,
	BSS_CHANGED_IDLE		= 1<<14,
	BSS_CHANGED_SSID		= 1<<15,
	BSS_CHANGED_AP_PROBE_RESP	= 1<<16,
	BSS_CHANGED_PS			= 1<<17,
	BSS_CHANGED_TXPOWER		= 1<<18,
	BSS_CHANGED_P2P_PS		= 1<<19,
	BSS_CHANGED_BEACON_INFO		= 1<<20,
	BSS_CHANGED_BANDWIDTH		= 1<<21,
	BSS_CHANGED_OCB                 = 1<<22,
	BSS_CHANGED_MU_GROUPS		= 1<<23,
	BSS_CHANGED_KEEP_ALIVE		= 1<<24,
	BSS_CHANGED_MCAST_RATE		= 1<<25,
	BSS_CHANGED_FTM_RESPONDER	= 1<<26,
	BSS_CHANGED_TWT			= 1<<27,
	BSS_CHANGED_HE_OBSS_PD		= 1<<28,
	BSS_CHANGED_HE_BSS_COLOR	= 1<<29,
	BSS_CHANGED_FILS_DISCOVERY      = 1<<30,
	BSS_CHANGED_UNSOL_BCAST_PROBE_RESP = BIT_ULL(31),
	BSS_CHANGED_MLD_VALID_LINKS	= BIT_ULL(33),
	BSS_CHANGED_MLD_TTLM		= BIT_ULL(34),
	BSS_CHANGED_TPE			= BIT_ULL(35),

	/* when adding here, make sure to change ieee80211_reconfig */
};

#define IEEE80211_CONN_MODE_HIGHEST	IEEE80211_CONN_MODE_EHT
enum ieee80211_conn_bw_limit {
	IEEE80211_CONN_BW_LIMIT_20,
	IEEE80211_CONN_BW_LIMIT_40,
	IEEE80211_CONN_BW_LIMIT_80,
	IEEE80211_CONN_BW_LIMIT_160, /* also 80+80 */
	IEEE80211_CONN_BW_LIMIT_320,
};

/* management frame protection */
enum ieee80211_mfp{
	IEEE80211_MFP_DISABLED = 0,
	IEEE80211_MFP_OPTIONAL = 1,
	IEEE80211_MFP_REQUIRED = 2
};

enum {
	IEEE80211_RX_MSG	= 1,
	IEEE80211_TX_STATUS_MSG	= 2,
};

/*
 * enum ieee80211_max_queues - maximum number of queues
 *
 * @IEEE80211_MAX_QUEUES: Maximum number of regular device queues.
 * @IEEE80211_MAX_QUEUE_MAP: bitmap with maximum queues set
 */
enum ieee80211_max_queues {
	IEEE80211_MAX_QUEUES =		16,
	IEEE80211_MAX_QUEUE_MAP =	BIT(IEEE80211_MAX_QUEUES) - 1,
};

/* Status codes */
enum ieee80211_statuscode {
	WLAN_STATUS_SUCCESS = 0,
	WLAN_STATUS_UNSPECIFIED_FAILURE = 1,
	WLAN_STATUS_CAPS_UNSUPPORTED = 10,
	WLAN_STATUS_REASSOC_NO_ASSOC = 11,
	WLAN_STATUS_ASSOC_DENIED_UNSPEC = 12,
	WLAN_STATUS_NOT_SUPPORTED_AUTH_ALG = 13,
	WLAN_STATUS_UNKNOWN_AUTH_TRANSACTION = 14,
	WLAN_STATUS_CHALLENGE_FAIL = 15,
	WLAN_STATUS_AUTH_TIMEOUT = 16,
	WLAN_STATUS_AP_UNABLE_TO_HANDLE_NEW_STA = 17,
	WLAN_STATUS_ASSOC_DENIED_RATES = 18,
	/* 802.11b */
	WLAN_STATUS_ASSOC_DENIED_NOSHORTPREAMBLE = 19,
	WLAN_STATUS_ASSOC_DENIED_NOPBCC = 20,
	WLAN_STATUS_ASSOC_DENIED_NOAGILITY = 21,
	/* 802.11h */
	WLAN_STATUS_ASSOC_DENIED_NOSPECTRUM = 22,
	WLAN_STATUS_ASSOC_REJECTED_BAD_POWER = 23,
	WLAN_STATUS_ASSOC_REJECTED_BAD_SUPP_CHAN = 24,
	/* 802.11g */
	WLAN_STATUS_ASSOC_DENIED_NOSHORTTIME = 25,
	WLAN_STATUS_ASSOC_DENIED_NODSSSOFDM = 26,
	/* 802.11w */
	WLAN_STATUS_ASSOC_REJECTED_TEMPORARILY = 30,
	WLAN_STATUS_ROBUST_MGMT_FRAME_POLICY_VIOLATION = 31,
	/* 802.11i */
	WLAN_STATUS_INVALID_IE = 40,
	WLAN_STATUS_INVALID_GROUP_CIPHER = 41,
	WLAN_STATUS_INVALID_PAIRWISE_CIPHER = 42,
	WLAN_STATUS_INVALID_AKMP = 43,
	WLAN_STATUS_UNSUPP_RSN_VERSION = 44,
	WLAN_STATUS_INVALID_RSN_IE_CAP = 45,
	WLAN_STATUS_CIPHER_SUITE_REJECTED = 46,
	/* 802.11e */
	WLAN_STATUS_UNSPECIFIED_QOS = 32,
	WLAN_STATUS_ASSOC_DENIED_NOBANDWIDTH = 33,
	WLAN_STATUS_ASSOC_DENIED_LOWACK = 34,
	WLAN_STATUS_ASSOC_DENIED_UNSUPP_QOS = 35,
	WLAN_STATUS_REQUEST_DECLINED = 37,
	WLAN_STATUS_INVALID_QOS_PARAM = 38,
	WLAN_STATUS_CHANGE_TSPEC = 39,
	WLAN_STATUS_WAIT_TS_DELAY = 47,
	WLAN_STATUS_NO_DIRECT_LINK = 48,
	WLAN_STATUS_STA_NOT_PRESENT = 49,
	WLAN_STATUS_STA_NOT_QSTA = 50,
	/* 802.11s */
	WLAN_STATUS_ANTI_CLOG_REQUIRED = 76,
	WLAN_STATUS_FCG_NOT_SUPP = 78,
	WLAN_STATUS_STA_NO_TBTT = 78,
	/* 802.11ad */
	WLAN_STATUS_REJECTED_WITH_SUGGESTED_CHANGES = 39,
	WLAN_STATUS_REJECTED_FOR_DELAY_PERIOD = 47,
	WLAN_STATUS_REJECT_WITH_SCHEDULE = 83,
	WLAN_STATUS_PENDING_ADMITTING_FST_SESSION = 86,
	WLAN_STATUS_PERFORMING_FST_NOW = 87,
	WLAN_STATUS_PENDING_GAP_IN_BA_WINDOW = 88,
	WLAN_STATUS_REJECT_U_PID_SETTING = 89,
	WLAN_STATUS_REJECT_DSE_BAND = 96,
	WLAN_STATUS_DENIED_WITH_SUGGESTED_BAND_AND_CHANNEL = 99,
	WLAN_STATUS_DENIED_DUE_TO_SPECTRUM_MANAGEMENT = 103,
	/* 802.11ai */
	WLAN_STATUS_FILS_AUTHENTICATION_FAILURE = 112,
	WLAN_STATUS_UNKNOWN_AUTHENTICATION_SERVER = 113,
	WLAN_STATUS_SAE_HASH_TO_ELEMENT = 126,
	WLAN_STATUS_SAE_PK = 127,
	WLAN_STATUS_DENIED_TID_TO_LINK_MAPPING = 133,
	WLAN_STATUS_PREF_TID_TO_LINK_MAPPING_SUGGESTED = 134,
};

/* Reason codes */
enum ieee80211_reasoncode {
	WLAN_REASON_UNSPECIFIED = 1,
	WLAN_REASON_PREV_AUTH_NOT_VALID = 2,
	WLAN_REASON_DEAUTH_LEAVING = 3,
	WLAN_REASON_DISASSOC_DUE_TO_INACTIVITY = 4,
	WLAN_REASON_DISASSOC_AP_BUSY = 5,
	WLAN_REASON_CLASS2_FRAME_FROM_NONAUTH_STA = 6,
	WLAN_REASON_CLASS3_FRAME_FROM_NONASSOC_STA = 7,
	WLAN_REASON_DISASSOC_STA_HAS_LEFT = 8,
	WLAN_REASON_STA_REQ_ASSOC_WITHOUT_AUTH = 9,
	/* 802.11h */
	WLAN_REASON_DISASSOC_BAD_POWER = 10,
	WLAN_REASON_DISASSOC_BAD_SUPP_CHAN = 11,
	/* 802.11i */
	WLAN_REASON_INVALID_IE = 13,
	WLAN_REASON_MIC_FAILURE = 14,
	WLAN_REASON_4WAY_HANDSHAKE_TIMEOUT = 15,
	WLAN_REASON_GROUP_KEY_HANDSHAKE_TIMEOUT = 16,
	WLAN_REASON_IE_DIFFERENT = 17,
	WLAN_REASON_INVALID_GROUP_CIPHER = 18,
	WLAN_REASON_INVALID_PAIRWISE_CIPHER = 19,
	WLAN_REASON_INVALID_AKMP = 20,
	WLAN_REASON_UNSUPP_RSN_VERSION = 21,
	WLAN_REASON_INVALID_RSN_IE_CAP = 22,
	WLAN_REASON_IEEE8021X_FAILED = 23,
	WLAN_REASON_CIPHER_SUITE_REJECTED = 24,
	/* TDLS (802.11z) */
	WLAN_REASON_TDLS_TEARDOWN_UNREACHABLE = 25,
	WLAN_REASON_TDLS_TEARDOWN_UNSPECIFIED = 26,
	/* 802.11e */
	WLAN_REASON_DISASSOC_UNSPECIFIED_QOS = 32,
	WLAN_REASON_DISASSOC_QAP_NO_BANDWIDTH = 33,
	WLAN_REASON_DISASSOC_LOW_ACK = 34,
	WLAN_REASON_DISASSOC_QAP_EXCEED_TXOP = 35,
	WLAN_REASON_QSTA_LEAVE_QBSS = 36,
	WLAN_REASON_QSTA_NOT_USE = 37,
	WLAN_REASON_QSTA_REQUIRE_SETUP = 38,
	WLAN_REASON_QSTA_TIMEOUT = 39,
	WLAN_REASON_QSTA_CIPHER_NOT_SUPP = 45,

	/* 802.11s */
	WLAN_REASON_MESH_PEER_CANCELED = 52,
	WLAN_REASON_MESH_MAX_PEERS = 53,
	WLAN_REASON_MESH_CONFIG = 54,
	WLAN_REASON_MESH_CLOSE = 55,
	WLAN_REASON_MESH_MAX_RETRIES = 56,
	WLAN_REASON_MESH_CONFIRM_TIMEOUT = 57,
	WLAN_REASON_MESH_INVALID_GTK = 58,
	WLAN_REASON_MESH_INCONSISTENT_PARAM = 59,
	WLAN_REASON_MESH_INVALID_SECURITY = 60,
	WLAN_REASON_MESH_PATH_ERROR = 61,
	WLAN_REASON_MESH_PATH_NOFORWARD = 62,
	WLAN_REASON_MESH_PATH_DEST_UNREACHABLE = 63,
	WLAN_REASON_MAC_EXISTS_IN_MBSS = 64,
	WLAN_REASON_MESH_CHAN_REGULATORY = 65,
	WLAN_REASON_MESH_CHAN = 66,
};

/* Information Element IDs */
enum ieee80211_eid {
	WLAN_EID_SSID = 0,
	WLAN_EID_SUPP_RATES = 1,
	WLAN_EID_FH_PARAMS = 2, /* reserved now */
	WLAN_EID_DS_PARAMS = 3,
	WLAN_EID_CF_PARAMS = 4,
	WLAN_EID_TIM = 5,
	WLAN_EID_IBSS_PARAMS = 6,
	WLAN_EID_COUNTRY = 7,
	/* 8, 9 reserved */
	WLAN_EID_REQUEST = 10,
	WLAN_EID_QBSS_LOAD = 11,
	WLAN_EID_EDCA_PARAM_SET = 12,
	WLAN_EID_TSPEC = 13,
	WLAN_EID_TCLAS = 14,
	WLAN_EID_SCHEDULE = 15,
	WLAN_EID_CHALLENGE = 16,
	/* 17-31 reserved for challenge text extension */
	WLAN_EID_PWR_CONSTRAINT = 32,
	WLAN_EID_PWR_CAPABILITY = 33,
	WLAN_EID_TPC_REQUEST = 34,
	WLAN_EID_TPC_REPORT = 35,
	WLAN_EID_SUPPORTED_CHANNELS = 36,
	WLAN_EID_CHANNEL_SWITCH = 37,
	WLAN_EID_MEASURE_REQUEST = 38,
	WLAN_EID_MEASURE_REPORT = 39,
	WLAN_EID_QUIET = 40,
	WLAN_EID_IBSS_DFS = 41,
	WLAN_EID_ERP_INFO = 42,
	WLAN_EID_TS_DELAY = 43,
	WLAN_EID_TCLAS_PROCESSING = 44,
	WLAN_EID_HT_CAPABILITY = 45,
	WLAN_EID_QOS_CAPA = 46,
	/* 47 reserved for Broadcom */
	WLAN_EID_RSN = 48,
	WLAN_EID_802_15_COEX = 49,
	WLAN_EID_EXT_SUPP_RATES = 50,
	WLAN_EID_AP_CHAN_REPORT = 51,
	WLAN_EID_NEIGHBOR_REPORT = 52,
	WLAN_EID_RCPI = 53,
	WLAN_EID_MOBILITY_DOMAIN = 54,
	WLAN_EID_FAST_BSS_TRANSITION = 55,
	WLAN_EID_TIMEOUT_INTERVAL = 56,
	WLAN_EID_RIC_DATA = 57,
	WLAN_EID_DSE_REGISTERED_LOCATION = 58,
	WLAN_EID_SUPPORTED_REGULATORY_CLASSES = 59,
	WLAN_EID_EXT_CHANSWITCH_ANN = 60,
	WLAN_EID_HT_OPERATION = 61,
	WLAN_EID_SECONDARY_CHANNEL_OFFSET = 62,
	WLAN_EID_BSS_AVG_ACCESS_DELAY = 63,
	WLAN_EID_ANTENNA_INFO = 64,
	WLAN_EID_RSNI = 65,
	WLAN_EID_MEASUREMENT_PILOT_TX_INFO = 66,
	WLAN_EID_BSS_AVAILABLE_CAPACITY = 67,
	WLAN_EID_BSS_AC_ACCESS_DELAY = 68,
	WLAN_EID_TIME_ADVERTISEMENT = 69,
	WLAN_EID_RRM_ENABLED_CAPABILITIES = 70,
	WLAN_EID_MULTIPLE_BSSID = 71,
	WLAN_EID_BSS_COEX_2040 = 72,
	WLAN_EID_BSS_INTOLERANT_CHL_REPORT = 73,
	WLAN_EID_OVERLAP_BSS_SCAN_PARAM = 74,
	WLAN_EID_RIC_DESCRIPTOR = 75,
	WLAN_EID_MMIE = 76,
	WLAN_EID_ASSOC_COMEBACK_TIME = 77,
	WLAN_EID_EVENT_REQUEST = 78,
	WLAN_EID_EVENT_REPORT = 79,
	WLAN_EID_DIAGNOSTIC_REQUEST = 80,
	WLAN_EID_DIAGNOSTIC_REPORT = 81,
	WLAN_EID_LOCATION_PARAMS = 82,
	WLAN_EID_NON_TX_BSSID_CAP =  83,
	WLAN_EID_SSID_LIST = 84,
	WLAN_EID_MULTI_BSSID_IDX = 85,
	WLAN_EID_FMS_DESCRIPTOR = 86,
	WLAN_EID_FMS_REQUEST = 87,
	WLAN_EID_FMS_RESPONSE = 88,
	WLAN_EID_QOS_TRAFFIC_CAPA = 89,
	WLAN_EID_BSS_MAX_IDLE_PERIOD = 90,
	WLAN_EID_TSF_REQUEST = 91,
	WLAN_EID_TSF_RESPOSNE = 92,
	WLAN_EID_WNM_SLEEP_MODE = 93,
	WLAN_EID_TIM_BCAST_REQ = 94,
	WLAN_EID_TIM_BCAST_RESP = 95,
	WLAN_EID_COLL_IF_REPORT = 96,
	WLAN_EID_CHANNEL_USAGE = 97,
	WLAN_EID_TIME_ZONE = 98,
	WLAN_EID_DMS_REQUEST = 99,
	WLAN_EID_DMS_RESPONSE = 100,
	WLAN_EID_LINK_ID = 101,
	WLAN_EID_WAKEUP_SCHEDUL = 102,
	/* 103 reserved */
	WLAN_EID_CHAN_SWITCH_TIMING = 104,
	WLAN_EID_PTI_CONTROL = 105,
	WLAN_EID_PU_BUFFER_STATUS = 106,
	WLAN_EID_INTERWORKING = 107,
	WLAN_EID_ADVERTISEMENT_PROTOCOL = 108,
	WLAN_EID_EXPEDITED_BW_REQ = 109,
	WLAN_EID_QOS_MAP_SET = 110,
	WLAN_EID_ROAMING_CONSORTIUM = 111,
	WLAN_EID_EMERGENCY_ALERT = 112,
	WLAN_EID_MESH_CONFIG = 113,
	WLAN_EID_MESH_ID = 114,
	WLAN_EID_LINK_METRIC_REPORT = 115,
	WLAN_EID_CONGESTION_NOTIFICATION = 116,
	WLAN_EID_PEER_MGMT = 117,
	WLAN_EID_CHAN_SWITCH_PARAM = 118,
	WLAN_EID_MESH_AWAKE_WINDOW = 119,
	WLAN_EID_BEACON_TIMING = 120,
	WLAN_EID_MCCAOP_SETUP_REQ = 121,
	WLAN_EID_MCCAOP_SETUP_RESP = 122,
	WLAN_EID_MCCAOP_ADVERT = 123,
	WLAN_EID_MCCAOP_TEARDOWN = 124,
	WLAN_EID_GANN = 125,
	WLAN_EID_RANN = 126,
	WLAN_EID_EXT_CAPABILITY = 127,
	/* 128, 129 reserved for Agere */
	WLAN_EID_PREQ = 130,
	WLAN_EID_PREP = 131,
	WLAN_EID_PERR = 132,
	/* 133-136 reserved for Cisco */
	WLAN_EID_PXU = 137,
	WLAN_EID_PXUC = 138,
	WLAN_EID_AUTH_MESH_PEER_EXCH = 139,
	WLAN_EID_MIC = 140,
	WLAN_EID_DESTINATION_URI = 141,
	WLAN_EID_UAPSD_COEX = 142,
	WLAN_EID_WAKEUP_SCHEDULE = 143,
	WLAN_EID_EXT_SCHEDULE = 144,
	WLAN_EID_STA_AVAILABILITY = 145,
	WLAN_EID_DMG_TSPEC = 146,
	WLAN_EID_DMG_AT = 147,
	WLAN_EID_DMG_CAP = 148,
	/* 149 reserved for Cisco */
	WLAN_EID_CISCO_VENDOR_SPECIFIC = 150,
	WLAN_EID_DMG_OPERATION = 151,
	WLAN_EID_DMG_BSS_PARAM_CHANGE = 152,
	WLAN_EID_DMG_BEAM_REFINEMENT = 153,
	WLAN_EID_CHANNEL_MEASURE_FEEDBACK = 154,
	/* 155-156 reserved for Cisco */
	WLAN_EID_AWAKE_WINDOW = 157,
	WLAN_EID_MULTI_BAND = 158,
	WLAN_EID_ADDBA_EXT = 159,
	WLAN_EID_NEXT_PCP_LIST = 160,
	WLAN_EID_PCP_HANDOVER = 161,
	WLAN_EID_DMG_LINK_MARGIN = 162,
	WLAN_EID_SWITCHING_STREAM = 163,
	WLAN_EID_SESSION_TRANSITION = 164,
	WLAN_EID_DYN_TONE_PAIRING_REPORT = 165,
	WLAN_EID_CLUSTER_REPORT = 166,
	WLAN_EID_RELAY_CAP = 167,
	WLAN_EID_RELAY_XFER_PARAM_SET = 168,
	WLAN_EID_BEAM_LINK_MAINT = 169,
	WLAN_EID_MULTIPLE_MAC_ADDR = 170,
	WLAN_EID_U_PID = 171,
	WLAN_EID_DMG_LINK_ADAPT_ACK = 172,
	/* 173 reserved for Symbol */
	WLAN_EID_MCCAOP_ADV_OVERVIEW = 174,
	WLAN_EID_QUIET_PERIOD_REQ = 175,
	/* 176 reserved for Symbol */
	WLAN_EID_QUIET_PERIOD_RESP = 177,
	/* 178-179 reserved for Symbol */
	/* 180 reserved for ISO/IEC 20011 */
	WLAN_EID_EPAC_POLICY = 182,
	WLAN_EID_CLISTER_TIME_OFF = 183,
	WLAN_EID_INTER_AC_PRIO = 184,
	WLAN_EID_SCS_DESCRIPTOR = 185,
	WLAN_EID_QLOAD_REPORT = 186,
	WLAN_EID_HCCA_TXOP_UPDATE_COUNT = 187,
	WLAN_EID_HL_STREAM_ID = 188,
	WLAN_EID_GCR_GROUP_ADDR = 189,
	WLAN_EID_ANTENNA_SECTOR_ID_PATTERN = 190,
	WLAN_EID_VHT_CAPABILITY = 191,
	WLAN_EID_VHT_OPERATION = 192,
	WLAN_EID_EXTENDED_BSS_LOAD = 193,
	WLAN_EID_WIDE_BW_CHANNEL_SWITCH = 194,
	WLAN_EID_TX_POWER_ENVELOPE = 195,
	WLAN_EID_CHANNEL_SWITCH_WRAPPER = 196,
	WLAN_EID_AID = 197,
	WLAN_EID_QUIET_CHANNEL = 198,
	WLAN_EID_OPMODE_NOTIF = 199,

	WLAN_EID_REDUCED_NEIGHBOR_REPORT = 201,

	WLAN_EID_AID_REQUEST = 210,
	WLAN_EID_AID_RESPONSE = 211,
	WLAN_EID_S1G_BCN_COMPAT = 213,
	WLAN_EID_S1G_SHORT_BCN_INTERVAL = 214,
	WLAN_EID_S1G_TWT = 216,
	WLAN_EID_S1G_CAPABILITIES = 217,
	WLAN_EID_VENDOR_SPECIFIC = 221,
	WLAN_EID_QOS_PARAMETER = 222,
	WLAN_EID_S1G_OPERATION = 232,
	WLAN_EID_CAG_NUMBER = 237,
	WLAN_EID_AP_CSN = 239,
	WLAN_EID_FILS_INDICATION = 240,
	WLAN_EID_DILS = 241,
	WLAN_EID_FRAGMENT = 242,
	WLAN_EID_RSNX = 244,
	WLAN_EID_EXTENSION = 255
};

/* Element ID Extensions for Element ID 255 */
enum ieee80211_eid_ext {
	WLAN_EID_EXT_ASSOC_DELAY_INFO = 1,
	WLAN_EID_EXT_FILS_REQ_PARAMS = 2,
	WLAN_EID_EXT_FILS_KEY_CONFIRM = 3,
	WLAN_EID_EXT_FILS_SESSION = 4,
	WLAN_EID_EXT_FILS_HLP_CONTAINER = 5,
	WLAN_EID_EXT_FILS_IP_ADDR_ASSIGN = 6,
	WLAN_EID_EXT_KEY_DELIVERY = 7,
	WLAN_EID_EXT_FILS_WRAPPED_DATA = 8,
	WLAN_EID_EXT_FILS_PUBLIC_KEY = 12,
	WLAN_EID_EXT_FILS_NONCE = 13,
	WLAN_EID_EXT_FUTURE_CHAN_GUIDANCE = 14,
	WLAN_EID_EXT_DH_PARAMETER = 32,
	WLAN_EID_EXT_HE_CAPABILITY = 35,
	WLAN_EID_EXT_HE_OPERATION = 36,
	WLAN_EID_EXT_UORA = 37,
	WLAN_EID_EXT_HE_MU_EDCA = 38,
	WLAN_EID_EXT_HE_SPR = 39,
	WLAN_EID_EXT_NDP_FEEDBACK_REPORT_PARAMSET = 41,
	WLAN_EID_EXT_BSS_COLOR_CHG_ANN = 42,
	WLAN_EID_EXT_QUIET_TIME_PERIOD_SETUP = 43,
	WLAN_EID_EXT_ESS_REPORT = 45,
	WLAN_EID_EXT_OPS = 46,
	WLAN_EID_EXT_HE_BSS_LOAD = 47,
	WLAN_EID_EXT_MAX_CHANNEL_SWITCH_TIME = 52,
	WLAN_EID_EXT_MULTIPLE_BSSID_CONFIGURATION = 55,
	WLAN_EID_EXT_NON_INHERITANCE = 56,
	WLAN_EID_EXT_KNOWN_BSSID = 57,
	WLAN_EID_EXT_SHORT_SSID_LIST = 58,
	WLAN_EID_EXT_HE_6GHZ_CAPA = 59,
	WLAN_EID_EXT_UL_MU_POWER_CAPA = 60,
	WLAN_EID_EXT_EHT_OPERATION = 106,
	WLAN_EID_EXT_EHT_MULTI_LINK = 107,
	WLAN_EID_EXT_EHT_CAPABILITY = 108,
	WLAN_EID_EXT_TID_TO_LINK_MAPPING = 109,
	WLAN_EID_EXT_BANDWIDTH_INDICATION = 135,
	WLAN_EID_EXT_KNOWN_STA_IDENTIFCATION = 136,
	WLAN_EID_EXT_NON_AP_STA_REG_CON = 137,
};

/* Action category code */
enum ieee80211_category {
	WLAN_CATEGORY_SPECTRUM_MGMT = 0,
	WLAN_CATEGORY_QOS = 1,
	WLAN_CATEGORY_DLS = 2,
	WLAN_CATEGORY_BACK = 3,
	WLAN_CATEGORY_PUBLIC = 4,
	WLAN_CATEGORY_RADIO_MEASUREMENT = 5,
	WLAN_CATEGORY_FAST_BBS_TRANSITION = 6,
	WLAN_CATEGORY_HT = 7,
	WLAN_CATEGORY_SA_QUERY = 8,
	WLAN_CATEGORY_PROTECTED_DUAL_OF_ACTION = 9,
	WLAN_CATEGORY_WNM = 10,
	WLAN_CATEGORY_WNM_UNPROTECTED = 11,
	WLAN_CATEGORY_TDLS = 12,
	WLAN_CATEGORY_MESH_ACTION = 13,
	WLAN_CATEGORY_MULTIHOP_ACTION = 14,
	WLAN_CATEGORY_SELF_PROTECTED = 15,
	WLAN_CATEGORY_DMG = 16,
	WLAN_CATEGORY_WMM = 17,
	WLAN_CATEGORY_FST = 18,
	WLAN_CATEGORY_UNPROT_DMG = 20,
	WLAN_CATEGORY_VHT = 21,
	WLAN_CATEGORY_S1G = 22,
	WLAN_CATEGORY_PROTECTED_EHT = 37,
	WLAN_CATEGORY_VENDOR_SPECIFIC_PROTECTED = 126,
	WLAN_CATEGORY_VENDOR_SPECIFIC = 127,
};

/* SPECTRUM_MGMT action code */
enum ieee80211_spectrum_mgmt_actioncode {
	WLAN_ACTION_SPCT_MSR_REQ = 0,
	WLAN_ACTION_SPCT_MSR_RPRT = 1,
	WLAN_ACTION_SPCT_TPC_REQ = 2,
	WLAN_ACTION_SPCT_TPC_RPRT = 3,
	WLAN_ACTION_SPCT_CHL_SWITCH = 4,
};

/* Self Protected Action codes */
enum ieee80211_self_protected_actioncode {
	WLAN_SP_RESERVED = 0,
	WLAN_SP_MESH_PEERING_OPEN = 1,
	WLAN_SP_MESH_PEERING_CONFIRM = 2,
	WLAN_SP_MESH_PEERING_CLOSE = 3,
	WLAN_SP_MGK_INFORM = 4,
	WLAN_SP_MGK_ACK = 5,
};

/* Unprotected WNM action codes */
enum ieee80211_unprotected_wnm_actioncode {
	WLAN_UNPROTECTED_WNM_ACTION_TIM = 0,
	WLAN_UNPROTECTED_WNM_ACTION_TIMING_MEASUREMENT_RESPONSE = 1,
};

/* Security key length */
enum ieee80211_key_len {
	WLAN_KEY_LEN_WEP40 = 5,
	WLAN_KEY_LEN_WEP104 = 13,
	WLAN_KEY_LEN_CCMP = 16,
	WLAN_KEY_LEN_CCMP_256 = 32,
	WLAN_KEY_LEN_TKIP = 32,
	WLAN_KEY_LEN_AES_CMAC = 16,
	WLAN_KEY_LEN_SMS4 = 32,
	WLAN_KEY_LEN_GCMP = 16,
	WLAN_KEY_LEN_GCMP_256 = 32,
	WLAN_KEY_LEN_BIP_CMAC_256 = 32,
	WLAN_KEY_LEN_BIP_GMAC_128 = 16,
	WLAN_KEY_LEN_BIP_GMAC_256 = 32,
};

/* Radio measurement action codes as defined in IEEE 802.11-2024 - Table 9-470 */
enum ieee80211_radio_measurement_actioncode {
	WLAN_RM_ACTION_RADIO_MEASUREMENT_REQUEST = 0,
	WLAN_RM_ACTION_RADIO_MEASUREMENT_REPORT  = 1,
	WLAN_RM_ACTION_LINK_MEASUREMENT_REQUEST  = 2,
	WLAN_RM_ACTION_LINK_MEASUREMENT_REPORT   = 3,
	WLAN_RM_ACTION_NEIGHBOR_REPORT_REQUEST   = 4,
	WLAN_RM_ACTION_NEIGHBOR_REPORT_RESPONSE  = 5,
};

/* see the radiotap website for the descriptions */
enum ieee80211_radiotap_presence {
	IEEE80211_RADIOTAP_TSFT = 0,
	IEEE80211_RADIOTAP_FLAGS = 1,
	IEEE80211_RADIOTAP_RATE = 2,
	IEEE80211_RADIOTAP_CHANNEL = 3,
	IEEE80211_RADIOTAP_FHSS = 4,
	IEEE80211_RADIOTAP_DBM_ANTSIGNAL = 5,
	IEEE80211_RADIOTAP_DBM_ANTNOISE = 6,
	IEEE80211_RADIOTAP_LOCK_QUALITY = 7,
	IEEE80211_RADIOTAP_TX_ATTENUATION = 8,
	IEEE80211_RADIOTAP_DB_TX_ATTENUATION = 9,
	IEEE80211_RADIOTAP_DBM_TX_POWER = 10,
	IEEE80211_RADIOTAP_ANTENNA = 11,
	IEEE80211_RADIOTAP_DB_ANTSIGNAL = 12,
	IEEE80211_RADIOTAP_DB_ANTNOISE = 13,
	IEEE80211_RADIOTAP_RX_FLAGS = 14,
	IEEE80211_RADIOTAP_TX_FLAGS = 15,
	IEEE80211_RADIOTAP_RTS_RETRIES = 16,
	IEEE80211_RADIOTAP_DATA_RETRIES = 17,
	/* 18 is XChannel, but it's not defined yet */
	IEEE80211_RADIOTAP_MCS = 19,
	IEEE80211_RADIOTAP_AMPDU_STATUS = 20,
	IEEE80211_RADIOTAP_VHT = 21,
	IEEE80211_RADIOTAP_TIMESTAMP = 22,
	IEEE80211_RADIOTAP_HE = 23,
	IEEE80211_RADIOTAP_HE_MU = 24,
	IEEE80211_RADIOTAP_ZERO_LEN_PSDU = 26,
	IEEE80211_RADIOTAP_LSIG = 27,
	IEEE80211_RADIOTAP_TLV = 28,

	/* valid in every it_present bitmap, even vendor namespaces */
	IEEE80211_RADIOTAP_RADIOTAP_NAMESPACE = 29,
	IEEE80211_RADIOTAP_VENDOR_NAMESPACE = 30,
	IEEE80211_RADIOTAP_EXT = 31,
	IEEE80211_RADIOTAP_EHT_USIG = 33,
	IEEE80211_RADIOTAP_EHT = 34,
};

/* for IEEE80211_RADIOTAP_FLAGS */
enum ieee80211_radiotap_flags {
	IEEE80211_RADIOTAP_F_CFP = 0x01,
	IEEE80211_RADIOTAP_F_SHORTPRE = 0x02,
	IEEE80211_RADIOTAP_F_WEP = 0x04,
	IEEE80211_RADIOTAP_F_FRAG = 0x08,
	IEEE80211_RADIOTAP_F_FCS = 0x10,
	IEEE80211_RADIOTAP_F_DATAPAD = 0x20,
	IEEE80211_RADIOTAP_F_BADFCS = 0x40,
};

/* for IEEE80211_RADIOTAP_CHANNEL */
enum ieee80211_radiotap_channel_flags {
	IEEE80211_CHAN_CCK = 0x0020,
	IEEE80211_CHAN_OFDM = 0x0040,
	IEEE80211_CHAN_2GHZ = 0x0080,
	IEEE80211_CHAN_5GHZ = 0x0100,
	IEEE80211_CHAN_DYN = 0x0400,
	IEEE80211_CHAN_HALF = 0x4000,
	IEEE80211_CHAN_QUARTER = 0x8000,
};

/* for IEEE80211_RADIOTAP_RX_FLAGS */
enum ieee80211_radiotap_rx_flags {
	IEEE80211_RADIOTAP_F_RX_BADPLCP = 0x0002,
};

/* for IEEE80211_RADIOTAP_TX_FLAGS */
enum ieee80211_radiotap_tx_flags {
	IEEE80211_RADIOTAP_F_TX_FAIL = 0x0001,
	IEEE80211_RADIOTAP_F_TX_CTS = 0x0002,
	IEEE80211_RADIOTAP_F_TX_RTS = 0x0004,
	IEEE80211_RADIOTAP_F_TX_NOACK = 0x0008,
	IEEE80211_RADIOTAP_F_TX_NOSEQNO = 0x0010,
	IEEE80211_RADIOTAP_F_TX_ORDER = 0x0020,
};

/* for IEEE80211_RADIOTAP_MCS "have" flags */
enum ieee80211_radiotap_mcs_have {
	IEEE80211_RADIOTAP_MCS_HAVE_BW = 0x01,
	IEEE80211_RADIOTAP_MCS_HAVE_MCS = 0x02,
	IEEE80211_RADIOTAP_MCS_HAVE_GI = 0x04,
	IEEE80211_RADIOTAP_MCS_HAVE_FMT = 0x08,
	IEEE80211_RADIOTAP_MCS_HAVE_FEC = 0x10,
	IEEE80211_RADIOTAP_MCS_HAVE_STBC = 0x20,
};

enum ieee80211_radiotap_mcs_flags {
	IEEE80211_RADIOTAP_MCS_BW_MASK = 0x03,
	IEEE80211_RADIOTAP_MCS_BW_20 = 0,
	IEEE80211_RADIOTAP_MCS_BW_40 = 1,
	IEEE80211_RADIOTAP_MCS_BW_20L = 2,
	IEEE80211_RADIOTAP_MCS_BW_20U = 3,

	IEEE80211_RADIOTAP_MCS_SGI = 0x04,
	IEEE80211_RADIOTAP_MCS_FMT_GF = 0x08,
	IEEE80211_RADIOTAP_MCS_FEC_LDPC = 0x10,
	IEEE80211_RADIOTAP_MCS_STBC_MASK = 0x60,
	IEEE80211_RADIOTAP_MCS_STBC_1 = 1,
	IEEE80211_RADIOTAP_MCS_STBC_2 = 2,
	IEEE80211_RADIOTAP_MCS_STBC_3 = 3,
	IEEE80211_RADIOTAP_MCS_STBC_SHIFT = 5,
};

/* for IEEE80211_RADIOTAP_AMPDU_STATUS */
enum ieee80211_radiotap_ampdu_flags {
	IEEE80211_RADIOTAP_AMPDU_REPORT_ZEROLEN = 0x0001,
	IEEE80211_RADIOTAP_AMPDU_IS_ZEROLEN = 0x0002,
	IEEE80211_RADIOTAP_AMPDU_LAST_KNOWN = 0x0004,
	IEEE80211_RADIOTAP_AMPDU_IS_LAST = 0x0008,
	IEEE80211_RADIOTAP_AMPDU_DELIM_CRC_ERR = 0x0010,
	IEEE80211_RADIOTAP_AMPDU_DELIM_CRC_KNOWN = 0x0020,
	IEEE80211_RADIOTAP_AMPDU_EOF = 0x0040,
	IEEE80211_RADIOTAP_AMPDU_EOF_KNOWN = 0x0080,
};

/* for IEEE80211_RADIOTAP_VHT */
enum ieee80211_radiotap_vht_known {
	IEEE80211_RADIOTAP_VHT_KNOWN_STBC = 0x0001,
	IEEE80211_RADIOTAP_VHT_KNOWN_TXOP_PS_NA = 0x0002,
	IEEE80211_RADIOTAP_VHT_KNOWN_GI = 0x0004,
	IEEE80211_RADIOTAP_VHT_KNOWN_SGI_NSYM_DIS = 0x0008,
	IEEE80211_RADIOTAP_VHT_KNOWN_LDPC_EXTRA_OFDM_SYM = 0x0010,
	IEEE80211_RADIOTAP_VHT_KNOWN_BEAMFORMED = 0x0020,
	IEEE80211_RADIOTAP_VHT_KNOWN_BANDWIDTH = 0x0040,
	IEEE80211_RADIOTAP_VHT_KNOWN_GROUP_ID = 0x0080,
	IEEE80211_RADIOTAP_VHT_KNOWN_PARTIAL_AID = 0x0100,
};

enum ieee80211_radiotap_vht_flags {
	IEEE80211_RADIOTAP_VHT_FLAG_STBC = 0x01,
	IEEE80211_RADIOTAP_VHT_FLAG_TXOP_PS_NA = 0x02,
	IEEE80211_RADIOTAP_VHT_FLAG_SGI = 0x04,
	IEEE80211_RADIOTAP_VHT_FLAG_SGI_NSYM_M10_9 = 0x08,
	IEEE80211_RADIOTAP_VHT_FLAG_LDPC_EXTRA_OFDM_SYM = 0x10,
	IEEE80211_RADIOTAP_VHT_FLAG_BEAMFORMED = 0x20,
};

enum ieee80211_radiotap_vht_coding {
	IEEE80211_RADIOTAP_CODING_LDPC_USER0 = 0x01,
	IEEE80211_RADIOTAP_CODING_LDPC_USER1 = 0x02,
	IEEE80211_RADIOTAP_CODING_LDPC_USER2 = 0x04,
	IEEE80211_RADIOTAP_CODING_LDPC_USER3 = 0x08,
};

enum ieee80211_radiotap_vht_bandwidth {
	/* Note: more values are defined but can't really be used */
	IEEE80211_RADIOTAP_VHT_BW_20		= 0,
	IEEE80211_RADIOTAP_VHT_BW_40		= 1,
	IEEE80211_RADIOTAP_VHT_BW_80		= 4,
	IEEE80211_RADIOTAP_VHT_BW_160		= 11,
};

/*
 * enum rate_info_flags - bitrate info flags
 *
 * Used by the driver to indicate the specific rate transmission
 * type for 802.11n transmissions.
 *
 * @RATE_INFO_FLAGS_MCS: mcs field filled with HT MCS
 * @RATE_INFO_FLAGS_VHT_MCS: mcs field filled with VHT MCS
 * @RATE_INFO_FLAGS_SHORT_GI: 400ns guard interval
 * @RATE_INFO_FLAGS_DMG: 60GHz MCS
 * @RATE_INFO_FLAGS_HE_MCS: HE MCS information
 * @RATE_INFO_FLAGS_EDMG: 60GHz MCS in EDMG mode
 * @RATE_INFO_FLAGS_EXTENDED_SC_DMG: 60GHz extended SC MCS
 * @RATE_INFO_FLAGS_EHT_MCS: EHT MCS information
 * @RATE_INFO_FLAGS_S1G_MCS: MCS field filled with S1G MCS
 */
enum rate_info_flags {
	RATE_INFO_FLAGS_MCS			= BIT(0),
	RATE_INFO_FLAGS_VHT_MCS			= BIT(1),
	RATE_INFO_FLAGS_SHORT_GI		= BIT(2),
	RATE_INFO_FLAGS_DMG			= BIT(3),
	RATE_INFO_FLAGS_HE_MCS			= BIT(4),
	RATE_INFO_FLAGS_EDMG			= BIT(5),
	RATE_INFO_FLAGS_EXTENDED_SC_DMG		= BIT(6),
	RATE_INFO_FLAGS_EHT_MCS			= BIT(7),
	RATE_INFO_FLAGS_S1G_MCS			= BIT(8),
};

/*
 * enum ieee80211_smps_mode - spatial multiplexing power save mode
 *
 * @IEEE80211_SMPS_AUTOMATIC: automatic
 * @IEEE80211_SMPS_OFF: off
 * @IEEE80211_SMPS_STATIC: static
 * @IEEE80211_SMPS_DYNAMIC: dynamic
 * @IEEE80211_SMPS_NUM_MODES: internal, don't use
 */
enum ieee80211_smps_mode {
	IEEE80211_SMPS_AUTOMATIC,
	IEEE80211_SMPS_OFF,
	IEEE80211_SMPS_STATIC,
	IEEE80211_SMPS_DYNAMIC,

	/* keep last */
	IEEE80211_SMPS_NUM_MODES,
};

/*
 * enum ieee80211_hw_flags - hardware flags
 *
 * These flags are used to indicate hardware capabilities to
 * the stack. Generally, flags here should have their meaning
 * done in a way that the simplest hardware doesn't need setting
 * any particular flags. There are some exceptions to this rule,
 * however, so you are advised to review these flags carefully.
 *
 * @IEEE80211_HW_HAS_RATE_CONTROL:
 *	The hardware or firmware includes rate control, and cannot be
 *	controlled by the stack. As such, no rate control algorithm
 *	should be instantiated, and the TX rate reported to userspace
 *	will be taken from the TX status instead of the rate control
 *	algorithm.
 *	Note that this requires that the driver implement a number of
 *	callbacks so it has the correct information, it needs to have
 *	the @set_rts_threshold callback and must look at the BSS config
 *	@use_cts_prot for G/N protection, @use_short_slot for slot
 *	timing in 2.4 GHz and @use_short_preamble for preambles for
 *	CCK frames.
 *
 * @IEEE80211_HW_RX_INCLUDES_FCS:
 *	Indicates that received frames passed to the stack include
 *	the FCS at the end.
 *
 * @IEEE80211_HW_HOST_BROADCAST_PS_BUFFERING:
 *	Some wireless LAN chipsets buffer broadcast/multicast frames
 *	for power saving stations in the hardware/firmware and others
 *	rely on the host system for such buffering. This option is used
 *	to configure the IEEE 802.11 upper layer to buffer broadcast and
 *	multicast frames when there are power saving stations so that
 *	the driver can fetch them with ieee80211_get_buffered_bc().
 *
 * @IEEE80211_HW_SIGNAL_UNSPEC:
 *	Hardware can provide signal values but we don't know its units. We
 *	expect values between 0 and @max_signal.
 *	If possible please provide dB or dBm instead.
 *
 * @IEEE80211_HW_SIGNAL_DBM:
 *	Hardware gives signal values in dBm, decibel difference from
 *	one milliwatt. This is the preferred method since it is standardized
 *	between different devices. @max_signal does not need to be set.
 *
 * @IEEE80211_HW_SPECTRUM_MGMT:
 * 	Hardware supports spectrum management defined in 802.11h
 * 	Measurement, Channel Switch, Quieting, TPC
 *
 * @IEEE80211_HW_AMPDU_AGGREGATION:
 *	Hardware supports 11n A-MPDU aggregation.
 *
 * @IEEE80211_HW_SUPPORTS_PS:
 *	Hardware has power save support (i.e. can go to sleep).
 *
 * @IEEE80211_HW_PS_NULLFUNC_STACK:
 *	Hardware requires nullfunc frame handling in stack, implies
 *	stack support for dynamic PS.
 *
 * @IEEE80211_HW_SUPPORTS_DYNAMIC_PS:
 *	Hardware has support for dynamic PS.
 *
 * @IEEE80211_HW_MFP_CAPABLE:
 *	Hardware supports management frame protection (MFP, IEEE 802.11w).
 *
 * @IEEE80211_HW_REPORTS_TX_ACK_STATUS:
 *	Hardware can provide ack status reports of Tx frames to
 *	the stack.
 *
 * @IEEE80211_HW_CONNECTION_MONITOR:
 *	The hardware performs its own connection monitoring, including
 *	periodic keep-alives to the AP and probing the AP on beacon loss.
 *
 * @IEEE80211_HW_NEED_DTIM_BEFORE_ASSOC:
 *	This device needs to get data from beacon before association (i.e.
 *	dtim_period).
 *
 * @IEEE80211_HW_SUPPORTS_PER_STA_GTK: The device's crypto engine supports
 *	per-station GTKs as used by IBSS RSN or during fast transition. If
 *	the device doesn't support per-station GTKs, but can be asked not
 *	to decrypt group addressed frames, then IBSS RSN support is still
 *	possible but software crypto will be used. Advertise the wiphy flag
 *	only in that case.
 *
 * @IEEE80211_HW_AP_LINK_PS: When operating in AP mode the device
 *	autonomously manages the PS status of connected stations. When
 *	this flag is set mac80211 will not trigger PS mode for connected
 *	stations based on the PM bit of incoming frames.
 *	Use ieee80211_start_ps()/ieee8021_end_ps() to manually configure
 *	the PS mode of connected stations.
 *
 * @IEEE80211_HW_TX_AMPDU_SETUP_IN_HW: The device handles TX A-MPDU session
 *	setup strictly in HW. mac80211 should not attempt to do this in
 *	software.
 *
 * @IEEE80211_HW_WANT_MONITOR_VIF: The driver would like to be informed of
 *	a virtual monitor interface when monitor interfaces are the only
 *	active interfaces.
 *
 * @IEEE80211_HW_NO_VIRTUAL_MONITOR: The driver would like to be informed
 *	of any monitor interface, as well as their configured channel.
 *	This is useful for supporting multiple monitor interfaces on different
 *	channels.
 *
 * @IEEE80211_HW_NO_AUTO_VIF: The driver would like for no wlanX to
 *	be created.  It is expected user-space will create vifs as
 *	desired (and thus have them named as desired).
 *
 * @IEEE80211_HW_SW_CRYPTO_CONTROL: The driver wants to control which of the
 *	crypto algorithms can be done in software - so don't automatically
 *	try to fall back to it if hardware crypto fails, but do so only if
 *	the driver returns 1. This also forces the driver to advertise its
 *	supported cipher suites.
 *
 * @IEEE80211_HW_SUPPORT_FAST_XMIT: The driver/hardware supports fast-xmit,
 *	this currently requires only the ability to calculate the duration
 *	for frames.
 *
 * @IEEE80211_HW_QUEUE_CONTROL: The driver wants to control per-interface
 *	queue mapping in order to use different queues (not just one per AC)
 *	for different virtual interfaces. See the doc section on HW queue
 *	control for more details.
 *
 * @IEEE80211_HW_SUPPORTS_RC_TABLE: The driver supports using a rate
 *	selection table provided by the rate control algorithm.
 *
 * @IEEE80211_HW_P2P_DEV_ADDR_FOR_INTF: Use the P2P Device address for any
 *	P2P Interface. This will be honoured even if more than one interface
 *	is supported.
 *
 * @IEEE80211_HW_TIMING_BEACON_ONLY: Use sync timing from beacon frames
 *	only, to allow getting TBTT of a DTIM beacon.
 *
 * @IEEE80211_HW_SUPPORTS_HT_CCK_RATES: Hardware supports mixing HT/CCK rates
 *	and can cope with CCK rates in an aggregation session (e.g. by not
 *	using aggregation for such frames.)
 *
 * @IEEE80211_HW_CHANCTX_STA_CSA: Support 802.11h based channel-switch (CSA)
 *	for a single active channel while using channel contexts. When support
 *	is not enabled the default action is to disconnect when getting the
 *	CSA frame.
 *
 * @IEEE80211_HW_SUPPORTS_CLONED_SKBS: The driver will never modify the payload
 *	or tailroom of TX skbs without copying them first.
 *
 * @IEEE80211_HW_SINGLE_SCAN_ON_ALL_BANDS: The HW supports scanning on all bands
 *	in one command, mac80211 doesn't have to run separate scans per band.
 *
 * @IEEE80211_HW_TDLS_WIDER_BW: The device/driver supports wider bandwidth
 *	than then BSS bandwidth for a TDLS link on the base channel.
 *
 * @IEEE80211_HW_SUPPORTS_AMSDU_IN_AMPDU: The driver supports receiving A-MSDUs
 *	within A-MPDU.
 *
 * @IEEE80211_HW_BEACON_TX_STATUS: The device/driver provides TX status
 *	for sent beacons.
 *
 * @IEEE80211_HW_NEEDS_UNIQUE_STA_ADDR: Hardware (or driver) requires that each
 *	station has a unique address, i.e. each station entry can be identified
 *	by just its MAC address; this prevents, for example, the same station
 *	from connecting to two virtual AP interfaces at the same time.
 *
 * @IEEE80211_HW_SUPPORTS_REORDERING_BUFFER: Hardware (or driver) manages the
 *	reordering buffer internally, guaranteeing mac80211 receives frames in
 *	order and does not need to manage its own reorder buffer or BA session
 *	timeout.
 *
 * @IEEE80211_HW_USES_RSS: The device uses RSS and thus requires parallel RX,
 *	which implies using per-CPU station statistics.
 *
 * @IEEE80211_HW_TX_AMSDU: Hardware (or driver) supports software aggregated
 *	A-MSDU frames. Requires software tx queueing and fast-xmit support.
 *	When not using minstrel/minstrel_ht rate control, the driver must
 *	limit the maximum A-MSDU size based on the current tx rate by setting
 *	max_rc_amsdu_len in struct ieee80211_sta.
 *
 * @IEEE80211_HW_TX_FRAG_LIST: Hardware (or driver) supports sending frag_list
 *	skbs, needed for zero-copy software A-MSDU.
 *
 * @IEEE80211_HW_REPORTS_LOW_ACK: The driver (or firmware) reports low ack event
 *	by ieee80211_report_low_ack() based on its own algorithm. For such
 *	drivers, mac80211 packet loss mechanism will not be triggered and driver
 *	is completely depending on firmware event for station kickout.
 *
 * @IEEE80211_HW_SUPPORTS_TX_FRAG: Hardware does fragmentation by itself.
 *	The stack will not do fragmentation.
 *	The callback for @set_frag_threshold should be set as well.
 *
 * @IEEE80211_HW_SUPPORTS_TDLS_BUFFER_STA: Hardware supports buffer STA on
 *	TDLS links.
 *
 * @IEEE80211_HW_DOESNT_SUPPORT_QOS_NDP: The driver (or firmware) doesn't
 *	support QoS NDP for AP probing - that's most likely a driver bug.
 *
 * @IEEE80211_HW_BUFF_MMPDU_TXQ: use the TXQ for bufferable MMPDUs, this of
 *	course requires the driver to use TXQs to start with.
 *
 * @IEEE80211_HW_SUPPORTS_VHT_EXT_NSS_BW: (Hardware) rate control supports VHT
 *	extended NSS BW (dot11VHTExtendedNSSBWCapable). This flag will be set if
 *	the selected rate control algorithm sets %RATE_CTRL_CAPA_VHT_EXT_NSS_BW
 *	but if the rate control is built-in then it must be set by the driver.
 *	See also the documentation for that flag.
 *
 * @IEEE80211_HW_STA_MMPDU_TXQ: use the extra non-TID per-station TXQ for all
 *	MMPDUs on station interfaces. This of course requires the driver to use
 *	TXQs to start with.
 *
 * @IEEE80211_HW_TX_STATUS_NO_AMPDU_LEN: Driver does not report accurate A-MPDU
 *	length in tx status information
 *
 * @IEEE80211_HW_SUPPORTS_MULTI_BSSID: Hardware supports multi BSSID
 *
 * @IEEE80211_HW_SUPPORTS_ONLY_HE_MULTI_BSSID: Hardware supports multi BSSID
 *	only for HE APs. Applies if @IEEE80211_HW_SUPPORTS_MULTI_BSSID is set.
 *
 * @IEEE80211_HW_AMPDU_KEYBORDER_SUPPORT: The card and driver is only
 *	aggregating MPDUs with the same keyid, allowing mac80211 to keep Tx
 *	A-MPDU sessions active while rekeying with Extended Key ID.
 *
 * @IEEE80211_HW_SUPPORTS_TX_ENCAP_OFFLOAD: Hardware supports tx encapsulation
 *	offload
 *
 * @IEEE80211_HW_SUPPORTS_RX_DECAP_OFFLOAD: Hardware supports rx decapsulation
 *	offload
 *
 * @IEEE80211_HW_SUPPORTS_CONC_MON_RX_DECAP: Hardware supports concurrent rx
 *	decapsulation offload and passing raw 802.11 frames for monitor iface.
 *	If this is supported, the driver must pass both 802.3 frames for real
 *	usage and 802.11 frames with %RX_FLAG_ONLY_MONITOR set for monitor to
 *	the stack.
 *
 * @IEEE80211_HW_DETECTS_COLOR_COLLISION: HW/driver has support for BSS color
 *	collision detection and doesn't need it in software.
 *
 * @IEEE80211_HW_MLO_MCAST_MULTI_LINK_TX: Hardware/driver handles transmitting
 *	multicast frames on all links, mac80211 should not do that.
 *
 * @IEEE80211_HW_DISALLOW_PUNCTURING: HW requires disabling puncturing in EHT
 *	and connecting with a lower bandwidth instead
 *
 * @IEEE80211_HW_HANDLES_QUIET_CSA: HW/driver handles quieting for CSA, so
 *	no need to stop queues. This really should be set by a driver that
 *	implements MLO, so operation can continue on other links when one
 *	link is switching.
 *
 * @IEEE80211_HW_STRICT: strictly enforce certain things mandated by the spec
 *	but otherwise ignored/worked around for interoperability. This is a
 *	HW flag so drivers can opt in according to their own control, e.g. in
 *	testing.
 *
 * @NUM_IEEE80211_HW_FLAGS: number of hardware flags, used for sizing arrays
 */
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

/*
 * enum wiphy_flags - wiphy capability flags
 *
 * @WIPHY_FLAG_SPLIT_SCAN_6GHZ: if set to true, the scan request will be split
 *	 into two, first for legacy bands and second for 6 GHz.
 * @WIPHY_FLAG_NETNS_OK: if not set, do not allow changing the netns of this
 *	wiphy at all
 * @WIPHY_FLAG_PS_ON_BY_DEFAULT: if set to true, powersave will be enabled
 *	by default -- this flag will be set depending on the kernel's default
 *	on wiphy_new(), but can be changed by the driver if it has a good
 *	reason to override the default
 * @WIPHY_FLAG_4ADDR_AP: supports 4addr mode even on AP (with a single station
 *	on a VLAN interface). This flag also serves an extra purpose of
 *	supporting 4ADDR AP mode on devices which do not support AP/VLAN iftype.
 * @WIPHY_FLAG_4ADDR_STATION: supports 4addr mode even as a station
 * @WIPHY_FLAG_CONTROL_PORT_PROTOCOL: This device supports setting the
 *	control port protocol ethertype. The device also honours the
 *	control_port_no_encrypt flag.
 * @WIPHY_FLAG_IBSS_RSN: The device supports IBSS RSN.
 * @WIPHY_FLAG_MESH_AUTH: The device supports mesh authentication by routing
 *	auth frames to userspace. See @NL80211_MESH_SETUP_USERSPACE_AUTH.
 * @WIPHY_FLAG_SUPPORTS_FW_ROAM: The device supports roaming feature in the
 *	firmware.
 * @WIPHY_FLAG_AP_UAPSD: The device supports uapsd on AP.
 * @WIPHY_FLAG_SUPPORTS_TDLS: The device supports TDLS (802.11z) operation.
 * @WIPHY_FLAG_TDLS_EXTERNAL_SETUP: The device does not handle TDLS (802.11z)
 *	link setup/discovery operations internally. Setup, discovery and
 *	teardown packets should be sent through the @NL80211_CMD_TDLS_MGMT
 *	command. When this flag is not set, @NL80211_CMD_TDLS_OPER should be
 *	used for asking the driver/firmware to perform a TDLS operation.
 * @WIPHY_FLAG_HAVE_AP_SME: device integrates AP SME
 * @WIPHY_FLAG_REPORTS_OBSS: the device will report beacons from other BSSes
 *	when there are virtual interfaces in AP mode by calling
 *	cfg80211_report_obss_beacon().
 * @WIPHY_FLAG_AP_PROBE_RESP_OFFLOAD: When operating as an AP, the device
 *	responds to probe-requests in hardware.
 * @WIPHY_FLAG_OFFCHAN_TX: Device supports direct off-channel TX.
 * @WIPHY_FLAG_HAS_REMAIN_ON_CHANNEL: Device supports remain-on-channel call.
 * @WIPHY_FLAG_SUPPORTS_5_10_MHZ: Device supports 5 MHz and 10 MHz channels.
 * @WIPHY_FLAG_HAS_CHANNEL_SWITCH: Device supports channel switch in
 *	beaconing mode (AP, IBSS, Mesh, ...).
 * @WIPHY_FLAG_SUPPORTS_EXT_KEK_KCK: The device supports bigger kek and kck keys
 * @WIPHY_FLAG_SUPPORTS_MLO: This is a temporary flag gating the MLO APIs,
 *	in order to not have them reachable in normal drivers, until we have
 *	complete feature/interface combinations/etc. advertisement. No driver
 *	should set this flag for now.
 * @WIPHY_FLAG_SUPPORTS_EXT_KCK_32: The device supports 32-byte KCK keys.
 * @WIPHY_FLAG_NOTIFY_REGDOM_BY_DRIVER: The device could handle reg notify for
 *	NL80211_REGDOM_SET_BY_DRIVER.
 * @WIPHY_FLAG_CHANNEL_CHANGE_ON_BEACON: reg_call_notifier() is called if driver
 *	set this flag to update channels on beacon hints.
 * @WIPHY_FLAG_SUPPORTS_NSTR_NONPRIMARY: support connection to non-primary link
 *	of an NSTR mobile AP MLD.
 * @WIPHY_FLAG_DISABLE_WEXT: disable wireless extensions for this device
 */
enum wiphy_flags {
	WIPHY_FLAG_SUPPORTS_EXT_KEK_KCK		= BIT(0),
	WIPHY_FLAG_SUPPORTS_MLO			= BIT(1),
	WIPHY_FLAG_SPLIT_SCAN_6GHZ		= BIT(2),
	WIPHY_FLAG_NETNS_OK			= BIT(3),
	WIPHY_FLAG_PS_ON_BY_DEFAULT		= BIT(4),
	WIPHY_FLAG_4ADDR_AP			= BIT(5),
	WIPHY_FLAG_4ADDR_STATION		= BIT(6),
	WIPHY_FLAG_CONTROL_PORT_PROTOCOL	= BIT(7),
	WIPHY_FLAG_IBSS_RSN			= BIT(8),
	WIPHY_FLAG_DISABLE_WEXT			= BIT(9),
	WIPHY_FLAG_MESH_AUTH			= BIT(10),
	WIPHY_FLAG_SUPPORTS_EXT_KCK_32          = BIT(11),
	WIPHY_FLAG_SUPPORTS_NSTR_NONPRIMARY	= BIT(12),
	WIPHY_FLAG_SUPPORTS_FW_ROAM		= BIT(13),
	WIPHY_FLAG_AP_UAPSD			= BIT(14),
	WIPHY_FLAG_SUPPORTS_TDLS		= BIT(15),
	WIPHY_FLAG_TDLS_EXTERNAL_SETUP		= BIT(16),
	WIPHY_FLAG_HAVE_AP_SME			= BIT(17),
	WIPHY_FLAG_REPORTS_OBSS			= BIT(18),
	WIPHY_FLAG_AP_PROBE_RESP_OFFLOAD	= BIT(19),
	WIPHY_FLAG_OFFCHAN_TX			= BIT(20),
	WIPHY_FLAG_HAS_REMAIN_ON_CHANNEL	= BIT(21),
	WIPHY_FLAG_SUPPORTS_5_10_MHZ		= BIT(22),
	WIPHY_FLAG_HAS_CHANNEL_SWITCH		= BIT(23),
	WIPHY_FLAG_NOTIFY_REGDOM_BY_DRIVER	= BIT(24),
	WIPHY_FLAG_CHANNEL_CHANGE_ON_BEACON     = BIT(25),
};

/*
 * enum wiphy_bss_param_flags - bit positions for supported bss parameters.
 *
 * @WIPHY_BSS_PARAM_CTS_PROT: support changing CTS protection.
 * @WIPHY_BSS_PARAM_SHORT_PREAMBLE: support changing short preamble usage.
 * @WIPHY_BSS_PARAM_SHORT_SLOT_TIME: support changing short slot time usage.
 * @WIPHY_BSS_PARAM_BASIC_RATES: support reconfiguring basic rates.
 * @WIPHY_BSS_PARAM_AP_ISOLATE: support changing AP isolation.
 * @WIPHY_BSS_PARAM_HT_OPMODE: support changing HT operating mode.
 * @WIPHY_BSS_PARAM_P2P_CTWINDOW: support reconfiguring ctwindow.
 * @WIPHY_BSS_PARAM_P2P_OPPPS: support changing P2P opportunistic power-save.
 */
enum wiphy_bss_param_flags {
	WIPHY_BSS_PARAM_CTS_PROT = BIT(0),
	WIPHY_BSS_PARAM_SHORT_PREAMBLE = BIT(1),
	WIPHY_BSS_PARAM_SHORT_SLOT_TIME = BIT(2),
	WIPHY_BSS_PARAM_BASIC_RATES = BIT(3),
	WIPHY_BSS_PARAM_AP_ISOLATE = BIT(4),
	WIPHY_BSS_PARAM_HT_OPMODE = BIT(5),
	WIPHY_BSS_PARAM_P2P_CTWINDOW = BIT(6),
	WIPHY_BSS_PARAM_P2P_OPPPS = BIT(7),
};

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

/*
 * enum ieee80211_sta_state - station state
 *
 * @IEEE80211_STA_NOTEXIST: station doesn't exist at all,
 *	this is a special state for add/remove transitions
 * @IEEE80211_STA_NONE: station exists without special state
 * @IEEE80211_STA_AUTH: station is authenticated
 * @IEEE80211_STA_ASSOC: station is associated
 * @IEEE80211_STA_AUTHORIZED: station is authorized (802.1X)
 */
enum ieee80211_sta_state {
	/* NOTE: These need to be ordered correctly! */
	IEEE80211_STA_NOTEXIST,
	IEEE80211_STA_NONE,
	IEEE80211_STA_AUTH,
	IEEE80211_STA_ASSOC,
	IEEE80211_STA_AUTHORIZED,
};

/*
 * enum mac80211_scan_flags - currently active scan mode
 *
 * @SCAN_SW_SCANNING: We're currently in the process of scanning but may as
 *	well be on the operating channel
 * @SCAN_HW_SCANNING: The hardware is scanning for us, we have no way to
 *	determine if we are on the operating channel or not
 * @SCAN_ONCHANNEL_SCANNING:  Do a software scan on only the current operating
 *	channel. This should not interrupt normal traffic.
 * @SCAN_COMPLETED: Set for our scan work function when the driver reported
 *	that the scan completed.
 * @SCAN_ABORTED: Set for our scan work function when the driver reported
 *	a scan complete for an aborted scan.
 * @SCAN_HW_CANCELLED: Set for our scan work function when the scan is being
 *	cancelled.
 * @SCAN_BEACON_WAIT: Set whenever we're passive scanning because of radar/no-IR
 *	and could send a probe request after receiving a beacon.
 * @SCAN_BEACON_DONE: Beacon received, we can now send a probe request
 */
enum mac80211_scan_flags {
	SCAN_SW_SCANNING,
	SCAN_HW_SCANNING,
	SCAN_ONCHANNEL_SCANNING,
	SCAN_COMPLETED,
	SCAN_ABORTED,
	SCAN_HW_CANCELLED,
	SCAN_BEACON_WAIT,
	SCAN_BEACON_DONE,
};

/*
 * enum mac80211_scan_state - scan state machine states
 *
 * @SCAN_DECISION: Main entry point to the scan state machine, this state
 *	determines if we should keep on scanning or switch back to the
 *	operating channel
 * @SCAN_SET_CHANNEL: Set the next channel to be scanned
 * @SCAN_SEND_PROBE: Send probe requests and wait for probe responses
 * @SCAN_SUSPEND: Suspend the scan and go back to operating channel to
 *	send out data
 * @SCAN_RESUME: Resume the scan and scan the next channel
 * @SCAN_ABORT: Abort the scan and go back to operating channel
 */
enum mac80211_scan_state {
	SCAN_DECISION,
	SCAN_SET_CHANNEL,
	SCAN_SEND_PROBE,
	SCAN_SUSPEND,
	SCAN_RESUME,
	SCAN_ABORT,
};

enum bss_source_type {
	BSS_SOURCE_DIRECT,
	BSS_SOURCE_MBSSID,
	BSS_SOURCE_STA_PROFILE,
};

/*
 * enum ieee80211_key_flags - key flags
 *
 * These flags are used for communication about keys between the driver
 * and mac80211, with the @flags parameter of &struct ieee80211_key_conf.
 *
 * @IEEE80211_KEY_FLAG_GENERATE_IV: This flag should be set by the
 *	driver to indicate that it requires IV generation for this
 *	particular key. Setting this flag does not necessarily mean that SKBs
 *	will have sufficient tailroom for ICV or MIC.
 * @IEEE80211_KEY_FLAG_GENERATE_MMIC: This flag should be set by
 *	the driver for a TKIP key if it requires Michael MIC
 *	generation in software.
 * @IEEE80211_KEY_FLAG_PAIRWISE: Set by mac80211, this flag indicates
 *	that the key is pairwise rather then a shared key.
 * @IEEE80211_KEY_FLAG_SW_MGMT_TX: This flag should be set by the driver for a
 *	CCMP/GCMP key if it requires CCMP/GCMP encryption of management frames
 *	(MFP) to be done in software.
 * @IEEE80211_KEY_FLAG_PUT_IV_SPACE: This flag should be set by the driver
 *	if space should be prepared for the IV, but the IV
 *	itself should not be generated. Do not set together with
 *	@IEEE80211_KEY_FLAG_GENERATE_IV on the same key. Setting this flag does
 *	not necessarily mean that SKBs will have sufficient tailroom for ICV or
 *	MIC.
 * @IEEE80211_KEY_FLAG_RX_MGMT: This key will be used to decrypt received
 *	management frames. The flag can help drivers that have a hardware
 *	crypto implementation that doesn't deal with management frames
 *	properly by allowing them to not upload the keys to hardware and
 *	fall back to software crypto. Note that this flag deals only with
 *	RX, if your crypto engine can't deal with TX you can also set the
 *	%IEEE80211_KEY_FLAG_SW_MGMT_TX flag to encrypt such frames in SW.
 * @IEEE80211_KEY_FLAG_GENERATE_IV_MGMT: This flag should be set by the
 *	driver for a CCMP/GCMP key to indicate that is requires IV generation
 *	only for management frames (MFP).
 * @IEEE80211_KEY_FLAG_RESERVE_TAILROOM: This flag should be set by the
 *	driver for a key to indicate that sufficient tailroom must always
 *	be reserved for ICV or MIC, even when HW encryption is enabled.
 * @IEEE80211_KEY_FLAG_PUT_MIC_SPACE: This flag should be set by the driver for
 *	a TKIP key if it only requires MIC space. Do not set together with
 *	@IEEE80211_KEY_FLAG_GENERATE_MMIC on the same key.
 * @IEEE80211_KEY_FLAG_NO_AUTO_TX: Key needs explicit Tx activation.
 * @IEEE80211_KEY_FLAG_GENERATE_MMIE: This flag should be set by the driver
 *	for a AES_CMAC or a AES_GMAC key to indicate that it requires sequence
 *	number generation only
 * @IEEE80211_KEY_FLAG_SPP_AMSDU: SPP A-MSDUs can be used with this key
 *	(set by mac80211 from the sta->spp_amsdu flag)
 */
enum ieee80211_key_flags {
	IEEE80211_KEY_FLAG_GENERATE_IV_MGMT	= BIT(0),
	IEEE80211_KEY_FLAG_GENERATE_IV		= BIT(1),
	IEEE80211_KEY_FLAG_GENERATE_MMIC	= BIT(2),
	IEEE80211_KEY_FLAG_PAIRWISE		= BIT(3),
	IEEE80211_KEY_FLAG_SW_MGMT_TX		= BIT(4),
	IEEE80211_KEY_FLAG_PUT_IV_SPACE		= BIT(5),
	IEEE80211_KEY_FLAG_RX_MGMT		= BIT(6),
	IEEE80211_KEY_FLAG_RESERVE_TAILROOM	= BIT(7),
	IEEE80211_KEY_FLAG_PUT_MIC_SPACE	= BIT(8),
	IEEE80211_KEY_FLAG_NO_AUTO_TX		= BIT(9),
	IEEE80211_KEY_FLAG_GENERATE_MMIE	= BIT(10),
	IEEE80211_KEY_FLAG_SPP_AMSDU		= BIT(11),
};

enum set_key_cmd {
	SET_KEY,
	DISABLE_KEY,
};

/*
 * enum rate_info_bw - rate bandwidth information
 *
 * Used by the driver to indicate the rate bandwidth.
 *
 * @RATE_INFO_BW_5: 5 MHz bandwidth
 * @RATE_INFO_BW_10: 10 MHz bandwidth
 * @RATE_INFO_BW_20: 20 MHz bandwidth
 * @RATE_INFO_BW_40: 40 MHz bandwidth
 * @RATE_INFO_BW_80: 80 MHz bandwidth
 * @RATE_INFO_BW_160: 160 MHz bandwidth
 * @RATE_INFO_BW_HE_RU: bandwidth determined by HE RU allocation
 * @RATE_INFO_BW_320: 320 MHz bandwidth
 * @RATE_INFO_BW_EHT_RU: bandwidth determined by EHT RU allocation
 * @RATE_INFO_BW_1: 1 MHz bandwidth
 * @RATE_INFO_BW_2: 2 MHz bandwidth
 * @RATE_INFO_BW_4: 4 MHz bandwidth
 * @RATE_INFO_BW_8: 8 MHz bandwidth
 * @RATE_INFO_BW_16: 16 MHz bandwidth
 */
enum rate_info_bw {
	RATE_INFO_BW_20 = 0,
	RATE_INFO_BW_5,
	RATE_INFO_BW_10,
	RATE_INFO_BW_40,
	RATE_INFO_BW_80,
	RATE_INFO_BW_160,
	RATE_INFO_BW_HE_RU,
	RATE_INFO_BW_320,
	RATE_INFO_BW_EHT_RU,
	RATE_INFO_BW_1,
	RATE_INFO_BW_2,
	RATE_INFO_BW_4,
	RATE_INFO_BW_8,
	RATE_INFO_BW_16,
};

/*
 * enum mac80211_rate_control_flags - per-rate flags set by the
 *	Rate Control algorithm.
 *
 * These flags are set by the Rate control algorithm for each rate during tx,
 * in the @flags member of struct ieee80211_tx_rate.
 *
 * @IEEE80211_TX_RC_USE_RTS_CTS: Use RTS/CTS exchange for this rate.
 * @IEEE80211_TX_RC_USE_CTS_PROTECT: CTS-to-self protection is required.
 *	This is set if the current BSS requires ERP protection.
 * @IEEE80211_TX_RC_USE_SHORT_PREAMBLE: Use short preamble.
 * @IEEE80211_TX_RC_MCS: HT rate.
 * @IEEE80211_TX_RC_VHT_MCS: VHT MCS rate, in this case the idx field is split
 *	into a higher 4 bits (Nss) and lower 4 bits (MCS number)
 * @IEEE80211_TX_RC_GREEN_FIELD: Indicates whether this rate should be used in
 *	Greenfield mode.
 * @IEEE80211_TX_RC_40_MHZ_WIDTH: Indicates if the Channel Width should be 40 MHz.
 * @IEEE80211_TX_RC_80_MHZ_WIDTH: Indicates 80 MHz transmission
 * @IEEE80211_TX_RC_160_MHZ_WIDTH: Indicates 160 MHz transmission
 *	(80+80 isn't supported yet)
 * @IEEE80211_TX_RC_DUP_DATA: The frame should be transmitted on both of the
 *	adjacent 20 MHz channels, if the current channel type is
 *	NL80211_CHAN_HT40MINUS or NL80211_CHAN_HT40PLUS.
 * @IEEE80211_TX_RC_SHORT_GI: Short Guard interval should be used for this rate.
 */
enum mac80211_rate_control_flags {
	IEEE80211_TX_RC_USE_RTS_CTS		= BIT(0),
	IEEE80211_TX_RC_USE_CTS_PROTECT		= BIT(1),
	IEEE80211_TX_RC_USE_SHORT_PREAMBLE	= BIT(2),

	/* rate index is an HT/VHT MCS instead of an index */
	IEEE80211_TX_RC_MCS			= BIT(3),
	IEEE80211_TX_RC_GREEN_FIELD		= BIT(4),
	IEEE80211_TX_RC_40_MHZ_WIDTH		= BIT(5),
	IEEE80211_TX_RC_DUP_DATA		= BIT(6),
	IEEE80211_TX_RC_SHORT_GI		= BIT(7),
	IEEE80211_TX_RC_VHT_MCS			= BIT(8),
	IEEE80211_TX_RC_80_MHZ_WIDTH		= BIT(9),
	IEEE80211_TX_RC_160_MHZ_WIDTH		= BIT(10),
};

/*
 * enum ieee80211_rate_flags - rate flags
 *
 * Hardware/specification flags for rates. These are structured
 * in a way that allows using the same bitrate structure for
 * different bands/PHY modes.
 *
 * @IEEE80211_RATE_SHORT_PREAMBLE: Hardware can send with short
 *	preamble on this bitrate; only relevant in 2.4GHz band and
 *	with CCK rates.
 * @IEEE80211_RATE_MANDATORY_A: This bitrate is a mandatory rate
 *	when used with 802.11a (on the 5 GHz band); filled by the
 *	core code when registering the wiphy.
 * @IEEE80211_RATE_MANDATORY_B: This bitrate is a mandatory rate
 *	when used with 802.11b (on the 2.4 GHz band); filled by the
 *	core code when registering the wiphy.
 * @IEEE80211_RATE_MANDATORY_G: This bitrate is a mandatory rate
 *	when used with 802.11g (on the 2.4 GHz band); filled by the
 *	core code when registering the wiphy.
 * @IEEE80211_RATE_ERP_G: This is an ERP rate in 802.11g mode.
 * @IEEE80211_RATE_SUPPORTS_5MHZ: Rate can be used in 5 MHz mode
 * @IEEE80211_RATE_SUPPORTS_10MHZ: Rate can be used in 10 MHz mode
 */
enum ieee80211_rate_flags {
	IEEE80211_RATE_SHORT_PREAMBLE	= BIT(0),
	IEEE80211_RATE_MANDATORY_A	= BIT(1),
	IEEE80211_RATE_MANDATORY_B	= BIT(2),
	IEEE80211_RATE_MANDATORY_G	= BIT(3),
	IEEE80211_RATE_ERP_G		= BIT(4),
	IEEE80211_RATE_SUPPORTS_5MHZ	= BIT(5),
	IEEE80211_RATE_SUPPORTS_10MHZ	= BIT(6),
};

/*
 * enum ieee80211_regulatory_flags - device regulatory flags
 *
 * @REGULATORY_CUSTOM_REG: tells us the driver for this device
 *	has its own custom regulatory domain and cannot identify the
 *	ISO / IEC 3166 alpha2 it belongs to. When this is enabled
 *	we will disregard the first regulatory hint (when the
 *	initiator is %REGDOM_SET_BY_CORE). Drivers that use
 *	wiphy_apply_custom_regulatory() should have this flag set
 *	or the regulatory core will set it for the wiphy.
 *	If you use regulatory_hint() *after* using
 *	wiphy_apply_custom_regulatory() the wireless core will
 *	clear the REGULATORY_CUSTOM_REG for your wiphy as it would be
 *	implied that the device somehow gained knowledge of its region.
 * @REGULATORY_STRICT_REG: tells us that the wiphy for this device
 *	has regulatory domain that it wishes to be considered as the
 *	superset for regulatory rules. After this device gets its regulatory
 *	domain programmed further regulatory hints shall only be considered
 *	for this device to enhance regulatory compliance, forcing the
 *	device to only possibly use subsets of the original regulatory
 *	rules. For example if channel 13 and 14 are disabled by this
 *	device's regulatory domain no user specified regulatory hint which
 *	has these channels enabled would enable them for this wiphy,
 *	the device's original regulatory domain will be trusted as the
 *	base. You can program the superset of regulatory rules for this
 *	wiphy with regulatory_hint() for cards programmed with an
 *	ISO3166-alpha2 country code. wiphys that use regulatory_hint()
 *	will have their wiphy->regd programmed once the regulatory
 *	domain is set, and all other regulatory hints will be ignored
 *	until their own regulatory domain gets programmed.
 * @REGULATORY_DISABLE_BEACON_HINTS: enable this if your driver needs to
 *	ensure that passive scan flags and beaconing flags may not be lifted by
 *	cfg80211 due to regulatory beacon hints. For more information on beacon
 *	hints read the documentation for regulatory_hint_found_beacon()
 * @REGULATORY_COUNTRY_IE_FOLLOW_POWER:  for devices that have a preference
 *	that even though they may have programmed their own custom power
 *	setting prior to wiphy registration, they want to ensure their channel
 *	power settings are updated for this connection with the power settings
 *	derived from the regulatory domain. The regulatory domain used will be
 *	based on the ISO3166-alpha2 from country IE provided through
 *	regulatory_hint_country_ie()
 * @REGULATORY_COUNTRY_IE_IGNORE: for devices that have a preference to ignore
 * 	all country IE information processed by the regulatory core. This will
 * 	override %REGULATORY_COUNTRY_IE_FOLLOW_POWER as all country IEs will
 * 	be ignored.
 * @REGULATORY_ENABLE_RELAX_NO_IR: for devices that wish to allow the
 *      NO_IR relaxation, which enables transmissions on channels on which
 *      otherwise initiating radiation is not allowed. This will enable the
 *      relaxations enabled under the CFG80211_REG_RELAX_NO_IR configuration
 *      option
 * @REGULATORY_WIPHY_SELF_MANAGED: for devices that employ wiphy-specific
 *	regdom management. These devices will ignore all regdom changes not
 *	originating from their own wiphy.
 *	A self-managed wiphys only employs regulatory information obtained from
 *	the FW and driver and does not use other cfg80211 sources like
 *	beacon-hints, country-code IEs and hints from other devices on the same
 *	system. Conversely, a self-managed wiphy does not share its regulatory
 *	hints with other devices in the system. If a system contains several
 *	devices, one or more of which are self-managed, there might be
 *	contradictory regulatory settings between them. Usage of flag is
 *	generally discouraged. Only use it if the FW/driver is incompatible
 *	with non-locally originated hints.
 *	This flag is incompatible with the flags: %REGULATORY_CUSTOM_REG,
 *	%REGULATORY_STRICT_REG, %REGULATORY_COUNTRY_IE_FOLLOW_POWER,
 *	%REGULATORY_COUNTRY_IE_IGNORE and %REGULATORY_DISABLE_BEACON_HINTS.
 *	Mixing any of the above flags with this flag will result in a failure
 *	to register the wiphy. This flag implies
 *	%REGULATORY_DISABLE_BEACON_HINTS and %REGULATORY_COUNTRY_IE_IGNORE.
 */
enum ieee80211_regulatory_flags {
	REGULATORY_CUSTOM_REG			= BIT(0),
	REGULATORY_STRICT_REG			= BIT(1),
	REGULATORY_DISABLE_BEACON_HINTS		= BIT(2),
	REGULATORY_COUNTRY_IE_FOLLOW_POWER	= BIT(3),
	REGULATORY_COUNTRY_IE_IGNORE		= BIT(4),
	REGULATORY_ENABLE_RELAX_NO_IR           = BIT(5),
	/* reuse bit 6 next time */
	REGULATORY_WIPHY_SELF_MANAGED		= BIT(7),
};

/*
 * enum mac80211_rx_encoding_flags - MCS & bandwidth flags
 *
 * @RX_ENC_FLAG_SHORTPRE: Short preamble was used for this frame
 * @RX_ENC_FLAG_SHORT_GI: Short guard interval was used
 * @RX_ENC_FLAG_HT_GF: This frame was received in a HT-greenfield transmission,
 *	if the driver fills this value it should add
 *	%IEEE80211_RADIOTAP_MCS_HAVE_FMT
 *	to @hw.radiotap_mcs_details to advertise that fact.
 * @RX_ENC_FLAG_LDPC: LDPC was used
 * @RX_ENC_FLAG_STBC_MASK: STBC 2 bit bitmask. 1 - Nss=1, 2 - Nss=2, 3 - Nss=3
 * @RX_ENC_FLAG_BF: packet was beamformed
 */
enum mac80211_rx_encoding_flags {
	RX_ENC_FLAG_SHORTPRE		= BIT(0),
	RX_ENC_FLAG_SHORT_GI		= BIT(2),
	RX_ENC_FLAG_HT_GF		= BIT(3),
	RX_ENC_FLAG_STBC_MASK		= BIT(4) | BIT(5),
	RX_ENC_FLAG_LDPC		= BIT(6),
	RX_ENC_FLAG_BF			= BIT(7),
};

/*
 * enum ieee80211_internal_key_flags - internal key flags
 *
 * @KEY_FLAG_UPLOADED_TO_HARDWARE: Indicates that this key is present
 *	in the hardware for TX crypto hardware acceleration.
 * @KEY_FLAG_TAINTED: Key is tainted and packets should be dropped.
 */
enum ieee80211_internal_key_flags {
	KEY_FLAG_UPLOADED_TO_HARDWARE	= BIT(0),
	KEY_FLAG_TAINTED		= BIT(1),
};

enum ieee80211_internal_tkip_state {
	TKIP_STATE_NOT_INIT,
	TKIP_STATE_PHASE1_DONE,
	TKIP_STATE_PHASE1_HW_UPLOADED,
};

enum mac80211_rx_encoding {
	RX_ENC_LEGACY = 0,
	RX_ENC_HT,
	RX_ENC_VHT,
	RX_ENC_HE,
	RX_ENC_EHT,
};

/*
 * enum ieee80211_edmg_bw_config - allowed channel bandwidth configurations
 *
 * @IEEE80211_EDMG_BW_CONFIG_4: 2.16GHz
 * @IEEE80211_EDMG_BW_CONFIG_5: 2.16GHz and 4.32GHz
 * @IEEE80211_EDMG_BW_CONFIG_6: 2.16GHz, 4.32GHz and 6.48GHz
 * @IEEE80211_EDMG_BW_CONFIG_7: 2.16GHz, 4.32GHz, 6.48GHz and 8.64GHz
 * @IEEE80211_EDMG_BW_CONFIG_8: 2.16GHz and 2.16GHz + 2.16GHz
 * @IEEE80211_EDMG_BW_CONFIG_9: 2.16GHz, 4.32GHz and 2.16GHz + 2.16GHz
 * @IEEE80211_EDMG_BW_CONFIG_10: 2.16GHz, 4.32GHz, 6.48GHz and 2.16GHz+2.16GHz
 * @IEEE80211_EDMG_BW_CONFIG_11: 2.16GHz, 4.32GHz, 6.48GHz, 8.64GHz and
 *	2.16GHz+2.16GHz
 * @IEEE80211_EDMG_BW_CONFIG_12: 2.16GHz, 2.16GHz + 2.16GHz and
 *	4.32GHz + 4.32GHz
 * @IEEE80211_EDMG_BW_CONFIG_13: 2.16GHz, 4.32GHz, 2.16GHz + 2.16GHz and
 *	4.32GHz + 4.32GHz
 * @IEEE80211_EDMG_BW_CONFIG_14: 2.16GHz, 4.32GHz, 6.48GHz, 2.16GHz + 2.16GHz
 *	and 4.32GHz + 4.32GHz
 * @IEEE80211_EDMG_BW_CONFIG_15: 2.16GHz, 4.32GHz, 6.48GHz, 8.64GHz,
 *	2.16GHz + 2.16GHz and 4.32GHz + 4.32GHz
 */
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

/*
 * enum environment_cap - Environment parsed from country IE
 * @ENVIRON_ANY: indicates country IE applies to both indoor and
 *	outdoor operation.
 * @ENVIRON_INDOOR: indicates country IE applies only to indoor operation
 * @ENVIRON_OUTDOOR: indicates country IE applies only to outdoor operation
 */
enum environment_cap {
	ENVIRON_ANY,
	ENVIRON_INDOOR,
	ENVIRON_OUTDOOR,
};

/*
 * enum rfkill_type - type of rfkill switch.
 *
 * @RFKILL_TYPE_ALL: toggles all switches (requests only - not a switch type)
 * @RFKILL_TYPE_WLAN: switch is on a 802.11 wireless network device.
 * @RFKILL_TYPE_BLUETOOTH: switch is on a bluetooth device.
 * @RFKILL_TYPE_UWB: switch is on a ultra wideband device.
 * @RFKILL_TYPE_WIMAX: switch is on a WiMAX device.
 * @RFKILL_TYPE_WWAN: switch is on a wireless WAN device.
 * @RFKILL_TYPE_GPS: switch is on a GPS device.
 * @RFKILL_TYPE_FM: switch is on a FM radio device.
 * @RFKILL_TYPE_NFC: switch is on an NFC device.
 * @NUM_RFKILL_TYPES: number of defined rfkill types
 */
enum rfkill_type {
	RFKILL_TYPE_ALL = 0,
	RFKILL_TYPE_WLAN,
	RFKILL_TYPE_BLUETOOTH,
	RFKILL_TYPE_UWB,
	RFKILL_TYPE_WIMAX,
	RFKILL_TYPE_WWAN,
	RFKILL_TYPE_GPS,
	RFKILL_TYPE_FM,
	RFKILL_TYPE_NFC,
	NUM_RFKILL_TYPES,
};

/*
 * enum ieee80211_chanctx_switch_mode - channel context switch mode
 * @CHANCTX_SWMODE_REASSIGN_VIF: Both old and new contexts already
 *	exist (and will continue to exist), but the virtual interface
 *	needs to be switched from one to the other.
 * @CHANCTX_SWMODE_SWAP_CONTEXTS: The old context exists but will stop
 *      to exist with this call, the new context doesn't exist but
 *      will be active after this call, the virtual interface switches
 *      from the old to the new (note that the driver may of course
 *      implement this as an on-the-fly chandef switch of the existing
 *      hardware context, but the mac80211 pointer for the old context
 *      will cease to exist and only the new one will later be used
 *      for changes/removal.)
 */
enum ieee80211_chanctx_switch_mode {
	CHANCTX_SWMODE_REASSIGN_VIF,
	CHANCTX_SWMODE_SWAP_CONTEXTS,
};

enum cfg80211_event_type {
	EVENT_CONNECT_RESULT,
	EVENT_ROAMED,
	EVENT_DISCONNECTED,
	EVENT_IBSS_JOINED,
	EVENT_STOPPED,
	EVENT_PORT_AUTHORIZED,
};

enum ieee80211_chanctx_change {
	IEEE80211_CHANCTX_CHANGE_WIDTH        = BIT(0),
	IEEE80211_CHANCTX_CHANGE_RX_CHAINS    = BIT(1),
	IEEE80211_CHANCTX_CHANGE_RADAR        = BIT(2),
	IEEE80211_CHANCTX_CHANGE_CHANNEL      = BIT(3),
	IEEE80211_CHANCTX_CHANGE_MIN_DEF      = BIT(4),
	IEEE80211_CHANCTX_CHANGE_AP           = BIT(5),
	IEEE80211_CHANCTX_CHANGE_PUNCTURING   = BIT(6),
};

struct wiphy_work;
typedef void (*wiphy_work_func_t)(struct wiphy *, struct wiphy_work *);

struct wiphy_work {
	struct list_head entry;
	wiphy_work_func_t func;
};

struct wiphy_delayed_work {
	struct wiphy_work work;
	struct wiphy *wiphy;
	struct timer_list timer;
};

/*
 * struct wiphy_iftype_akm_suites - This structure encapsulates supported akm
 * suites for interface types defined in @iftypes_mask. Each type in the
 * @iftypes_mask must be unique across all instances of iftype_akm_suites.
 *
 * @iftypes_mask: bitmask of interfaces types
 * @akm_suites: points to an array of supported akm suites
 * @n_akm_suites: number of supported AKM suites
 */
struct wiphy_iftype_akm_suites {
	u16 iftypes_mask;
	const u32 *akm_suites;
	int n_akm_suites;
};

struct rfkill {
	enum rfkill_type	type;

	unsigned long		state;
	unsigned long		hard_block_reasons;

	u32			idx;

	bool			registered;
	bool			persistent;
	bool			polling_paused;
	bool			suspended;
	bool			need_sync;

	const struct rfkill_ops	*ops;
	void			*data;
};

struct element {
        u8 id;
        u8 datalen;
        u8 data[];
} __packed;

struct ieee80211_radiotap_vht {
	__le16 known;
	u8 flags;
	u8 bandwidth;
	u8 mcs_nss[4];
	u8 coding;
	u8 group_id;
	__le16 partial_aid;
} __packed;

struct ieee80211_radiotap_he {
	__le16 data1;
	__le16 data2;
	__le16 data3;
	__le16 data4;
	__le16 data5;
	__le16 data6;
};

struct ieee80211_radiotap_he_mu {
	__le16 flags1;
	__le16 flags2;
	u8 ru_ch1[4];
	u8 ru_ch2[4];
};

struct ieee80211_radiotap_lsig {
	__le16 data1;
	__le16 data2;
};

/* Management MIC information element (IEEE 802.11w) for CMAC */
struct ieee80211_mmie {
	u8 element_id;
	u8 length;
	__le16 key_id;
	u8 sequence_number[6];
	u8 mic[8];
} __packed;

/* Management MIC information element (IEEE 802.11w) for GMAC and CMAC-256 */
struct ieee80211_mmie_16 {
	u8 element_id;
	u8 length;
	__le16 key_id;
	u8 sequence_number[6];
	u8 mic[16];
} __packed;

/* Management MIC information element (IEEE 802.11w) for all variants */
struct ieee80211_mmie_var {
	u8 element_id;
	u8 length;
	__le16 key_id;
	u8 sequence_number[6];
	u8 mic[]; /* 8 or 16 bytes */
} __packed;

struct ieee80211_vendor_ie {
	u8 element_id;
	u8 len;
	u8 oui[3];
	u8 oui_type;
} __packed;


struct ieee80211_addba_ext_ie {
	u8 data;
} __packed;

struct ieee80211_ext {
	__le16 frame_control;
	__le16 duration;
	union {
		struct {
			u8 sa[ETH_ALEN];
			__le32 timestamp;
			u8 change_seq;
			u8 variable[];
		} __packed s1g_beacon;
	} u;
} __packed __aligned(2);

/*
 * struct ieee80211_bss_load_elem - BSS Load elemen
 *
 * Defined in section 9.4.2.26 in IEEE 802.11-REVme D4.1
 *
 * @sta_count: total number of STAs currently associated with the AP.
 * @channel_util: Percentage of time that the access point sensed the channel
 *	was busy. This value is in range [0, 255], the highest value means
 *	100% busy.
 * @avail_admission_capa: remaining amount of medium time used for admission
 *	control.
 */
struct ieee80211_bss_load_elem {
	__le16 sta_count;
	u8 channel_util;
	__le16 avail_admission_capa;
} __packed;

/*
 * struct ieee80211_tpc_report_ie - TPC Report element
 * @tx_power: Transmit Power
 * @link_margin: Link Margin
 *
 * This structure represents the payload of the "TPC Report element" as
 * described in IEEE Std 802.11-2020 section 9.4.2.16.
 */
struct ieee80211_tpc_report_ie {
	u8 tx_power;
	u8 link_margin;
} __packed;

/*
 * struct ieee80211_quiet_ie - Quiet element
 * @count: Quiet Count
 * @period: Quiet Period
 * @duration: Quiet Duration
 * @offset: Quiet Offset
 *
 * This structure represents the payload of the "Quiet element" as
 * described in IEEE Std 802.11-2020 section 9.4.2.22.
 */
struct ieee80211_quiet_ie {
	u8 count;
	u8 period;
	__le16 duration;
	__le16 offset;
} __packed;

/*
 * struct ieee80211_msrment_ie - Measurement element
 * @token: Measurement Token
 * @mode: Measurement Report Mode
 * @type: Measurement Type
 * @request: Measurement Request or Measurement Report
 *
 * This structure represents the payload of both the "Measurement
 * Request element" and the "Measurement Report element" as described
 * in IEEE Std 802.11-2020 sections 9.4.2.20 and 9.4.2.21.
 */
struct ieee80211_msrment_ie {
	u8 token;
	u8 mode;
	u8 type;
	u8 request[];
} __packed;

/*
 * struct ieee80211_channel_sw_ie - Channel Switch Announcement element
 * @mode: Channel Switch Mode
 * @new_ch_num: New Channel Number
 * @count: Channel Switch Count
 *
 * This structure represents the payload of the "Channel Switch
 * Announcement element" as described in IEEE Std 802.11-2020 section
 * 9.4.2.18.
 */
struct ieee80211_channel_sw_ie {
	u8 mode;
	u8 new_ch_num;
	u8 count;
} __packed;

/*
 * struct ieee80211_ext_chansw_ie - Extended Channel Switch Announcement element
 * @mode: Channel Switch Mode
 * @new_operating_class: New Operating Class
 * @new_ch_num: New Channel Number
 * @count: Channel Switch Count
 *
 * This structure represents the "Extended Channel Switch Announcement
 * element" as described in IEEE Std 802.11-2020 section 9.4.2.52.
 */
struct ieee80211_ext_chansw_ie {
	u8 mode;
	u8 new_operating_class;
	u8 new_ch_num;
	u8 count;
} __packed;

/*
 * struct ieee80211_sec_chan_offs_ie - secondary channel offset IE
 * @sec_chan_offs: secondary channel offset, uses IEEE80211_HT_PARAM_CHA_SEC_*
 *	values here
 * This structure represents the "Secondary Channel Offset element"
 */
struct ieee80211_sec_chan_offs_ie {
	u8 sec_chan_offs;
} __packed;

/*
 * struct ieee80211_wide_bw_chansw_ie - wide bandwidth channel switch IE
 * @new_channel_width: New Channel Width
 * @new_center_freq_seg0: New Channel Center Frequency Segment 0
 * @new_center_freq_seg1: New Channel Center Frequency Segment 1
 *
 * This structure represents the payload of the "Wide Bandwidth
 * Channel Switch element" as described in IEEE Std 802.11-2020
 * section 9.4.2.160.
 */
struct ieee80211_wide_bw_chansw_ie {
	u8 new_channel_width;
	u8 new_center_freq_seg0, new_center_freq_seg1;
} __packed;

/* represent an ieee80211 management frame */
struct ieee80211_mgmt {
	__le16 frame_control;
	__le16 duration;
	u8 da[ETH_ALEN];
	u8 sa[ETH_ALEN];
	u8 bssid[ETH_ALEN];
	__le16 seq_ctrl;
	union {
		struct {
			__le16 auth_alg;
			__le16 auth_transaction;
			__le16 status_code;
			/* possibly followed by Challenge text */
			u8 variable[];
		} __packed auth;
		struct {
			__le16 reason_code;
		} __packed deauth;
		struct {
			__le16 capab_info;
			__le16 listen_interval;
			/* followed by SSID and Supported rates */
			u8 variable[];
		} __packed assoc_req;
		struct {
			__le16 capab_info;
			__le16 status_code;
			__le16 aid;
			/* followed by Supported rates */
			u8 variable[];
		} __packed assoc_resp, reassoc_resp;
		struct {
			__le16 capab_info;
			__le16 status_code;
			u8 variable[];
		} __packed s1g_assoc_resp, s1g_reassoc_resp;
		struct {
			__le16 capab_info;
			__le16 listen_interval;
			u8 current_ap[ETH_ALEN];
			/* followed by SSID and Supported rates */
			u8 variable[];
		} __packed reassoc_req;
		struct {
			__le16 reason_code;
		} __packed disassoc;
		struct {
			__le64 timestamp;
			__le16 beacon_int;
			__le16 capab_info;
			/* followed by some of SSID, Supported rates,
			 * FH Params, DS Params, CF Params, IBSS Params, TIM */
			u8 variable[];
		} __packed beacon;
		struct {
			/* only variable items: SSID, Supported rates */
			DECLARE_FLEX_ARRAY(u8, variable);
		} __packed probe_req;
		struct {
			__le64 timestamp;
			__le16 beacon_int;
			__le16 capab_info;
			/* followed by some of SSID, Supported rates,
			 * FH Params, DS Params, CF Params, IBSS Params */
			u8 variable[];
		} __packed probe_resp;
		struct {
			u8 category;
			union {
				struct {
					u8 action_code;
					u8 dialog_token;
					u8 status_code;
					u8 variable[];
				} __packed wme_action;
				struct{
					u8 action_code;
					u8 variable[];
				} __packed chan_switch;
				struct{
					u8 action_code;
					struct ieee80211_ext_chansw_ie data;
					u8 variable[];
				} __packed ext_chan_switch;
				struct{
					u8 action_code;
					u8 dialog_token;
					u8 element_id;
					u8 length;
					struct ieee80211_msrment_ie msr_elem;
				} __packed measurement;
				struct{
					u8 action_code;
					u8 dialog_token;
					__le16 capab;
					__le16 timeout;
					__le16 start_seq_num;
					/* followed by BA Extension */
					u8 variable[];
				} __packed addba_req;
				struct{
					u8 action_code;
					u8 dialog_token;
					__le16 status;
					__le16 capab;
					__le16 timeout;
					/* followed by BA Extension */
					u8 variable[];
				} __packed addba_resp;
				struct{
					u8 action_code;
					__le16 params;
					__le16 reason_code;
				} __packed delba;
				struct {
					u8 action_code;
					u8 variable[];
				} __packed self_prot;
				struct{
					u8 action_code;
					u8 variable[];
				} __packed mesh_action;
				struct {
					u8 action;
					u8 trans_id[WLAN_SA_QUERY_TR_ID_LEN];
				} __packed sa_query;
				struct {
					u8 action;
					u8 smps_control;
				} __packed ht_smps;
				struct {
					u8 action_code;
					u8 chanwidth;
				} __packed ht_notify_cw;
				struct {
					u8 action_code;
					u8 dialog_token;
					__le16 capability;
					u8 variable[];
				} __packed tdls_discover_resp;
				struct {
					u8 action_code;
					u8 operating_mode;
				} __packed vht_opmode_notif;
				struct {
					u8 action_code;
					u8 membership[WLAN_MEMBERSHIP_LEN];
					u8 position[WLAN_USER_POSITION_LEN];
				} __packed vht_group_notif;
				struct {
					u8 action_code;
					u8 dialog_token;
					u8 tpc_elem_id;
					u8 tpc_elem_length;
					struct ieee80211_tpc_report_ie tpc;
				} __packed tpc_report;
				struct {
					u8 action_code;
					u8 dialog_token;
					u8 follow_up;
					u8 tod[6];
					u8 toa[6];
					__le16 tod_error;
					__le16 toa_error;
					u8 variable[];
				} __packed ftm;
				struct {
					u8 action_code;
					u8 variable[];
				} __packed s1g;
				struct {
					u8 action_code;
					u8 dialog_token;
					u8 follow_up;
					u32 tod;
					u32 toa;
					u8 max_tod_error;
					u8 max_toa_error;
				} __packed wnm_timing_msr;
				struct {
					u8 action_code;
					u8 dialog_token;
					u8 variable[];
				} __packed ttlm_req;
				struct {
					u8 action_code;
					u8 dialog_token;
					__le16 status_code;
					u8 variable[];
				} __packed ttlm_res;
				struct {
					u8 action_code;
				} __packed ttlm_tear_down;
				struct {
					u8 action_code;
					u8 dialog_token;
					u8 variable[];
				} __packed ml_reconf_req;
				struct {
					u8 action_code;
					u8 dialog_token;
					u8 count;
					u8 variable[];
				} __packed ml_reconf_resp;
				struct {
					u8 action_code;
					u8 variable[];
				} __packed epcs;
				struct {
					u8 action_code;
					u8 dialog_token;
					u8 control;
					u8 variable[];
				} __packed eml_omn;
			} u;
		} __packed action;
		DECLARE_FLEX_ARRAY(u8, body); /* Generic frame body */
	} u;
} __packed __aligned(2);

/*
 * struct ieee80211_mcs_info - Supported MCS Set field
 * @rx_mask: RX mask
 * @rx_highest: highest supported RX rate. If set represents
 *	the highest supported RX data rate in units of 1 Mbps.
 *	If this field is 0 this value should not be used to
 *	consider the highest RX data rate supported.
 * @tx_params: TX parameters
 * @reserved: Reserved bits
 *
 * This structure represents the "Supported MCS Set field" as
 * described in IEEE Std 802.11-2020 section 9.4.2.55.4.
 */
struct ieee80211_mcs_info {
	u8 rx_mask[IEEE80211_HT_MCS_MASK_LEN];
	__le16 rx_highest;
	u8 tx_params;
	u8 reserved[3];
} __packed;

/*
 * struct ieee80211_sta_ht_cap - STA's HT capabilities
 *
 * This structure describes most essential parameters needed
 * to describe 802.11n HT capabilities for an STA.
 *
 * @ht_supported: is HT supported by the STA
 * @cap: HT capabilities map as described in 802.11n spec
 * @ampdu_factor: Maximum A-MPDU length factor
 * @ampdu_density: Minimum A-MPDU spacing
 * @mcs: Supported MCS rates
 */
struct ieee80211_sta_ht_cap {
	u16 cap; /* use IEEE80211_HT_CAP_ */
	bool ht_supported;
	u8 ampdu_factor;
	u8 ampdu_density;
	struct ieee80211_mcs_info mcs;
};

/*
 * struct ieee80211_vht_mcs_info - VHT MCS information
 * @rx_mcs_map: RX MCS map 2 bits for each stream, total 8 streams
 * @rx_highest: Indicates highest long GI VHT PPDU data rate
 *	STA can receive. Rate expressed in units of 1 Mbps.
 *	If this field is 0 this value should not be used to
 *	consider the highest RX data rate supported.
 *	The top 3 bits of this field indicate the Maximum NSTS,total
 *	(a beamformee capability.)
 * @tx_mcs_map: TX MCS map 2 bits for each stream, total 8 streams
 * @tx_highest: Indicates highest long GI VHT PPDU data rate
 *	STA can transmit. Rate expressed in units of 1 Mbps.
 *	If this field is 0 this value should not be used to
 *	consider the highest TX data rate supported.
 *	The top 2 bits of this field are reserved, the
 *	3rd bit from the top indiciates VHT Extended NSS BW
 *	Capability.
 */
struct ieee80211_vht_mcs_info {
	__le16 rx_mcs_map;
	__le16 rx_highest;
	__le16 tx_mcs_map;
	__le16 tx_highest;
} __packed;

/*
 * struct ieee80211_vht_cap - VHT capabilities
 *
 * This structure is the "VHT capabilities element" as
 * described in 802.11ac D3.0 8.4.2.160
 * @vht_cap_info: VHT capability info
 * @supp_mcs: VHT MCS supported rates
 */
struct ieee80211_vht_cap {
	__le32 vht_cap_info;
	struct ieee80211_vht_mcs_info supp_mcs;
} __packed;

/*
 * struct ieee80211_sta_vht_cap - STA's VHT capabilities
 *
 * This structure describes most essential parameters needed
 * to describe 802.11ac VHT capabilities for an STA.
 *
 * @vht_supported: is VHT supported by the STA
 * @cap: VHT capabilities map as described in 802.11ac spec
 * @vht_mcs: Supported VHT MCS rates
 */
struct ieee80211_sta_vht_cap {
	bool vht_supported;
	u32 cap; /* use IEEE80211_VHT_CAP_ */
	struct ieee80211_vht_mcs_info vht_mcs;
};

/*
 * struct ieee80211_sta_s1g_cap - STA's S1G capabilities
 *
 * This structure describes most essential parameters needed
 * to describe 802.11ah S1G capabilities for a STA.
 *
 * @s1g: is STA an S1G STA
 * @cap: S1G capabilities information
 * @nss_mcs: Supported NSS MCS set
 */
struct ieee80211_sta_s1g_cap {
	bool s1g;
	u8 cap[10]; /* use S1G_CAPAB_ */
	u8 nss_mcs[5];
};

/*
 * struct ieee80211_he_mcs_nss_supp - HE Tx/Rx HE MCS NSS Support Field
 *
 * This structure holds the data required for the Tx/Rx HE MCS NSS Support Field
 * described in P802.11ax_D2.0 section 9.4.2.237.4
 *
 * @rx_mcs_80: Rx MCS map 2 bits for each stream, total 8 streams, for channel
 *     widths less than 80MHz.
 * @tx_mcs_80: Tx MCS map 2 bits for each stream, total 8 streams, for channel
 *     widths less than 80MHz.
 * @rx_mcs_160: Rx MCS map 2 bits for each stream, total 8 streams, for channel
 *     width 160MHz.
 * @tx_mcs_160: Tx MCS map 2 bits for each stream, total 8 streams, for channel
 *     width 160MHz.
 * @rx_mcs_80p80: Rx MCS map 2 bits for each stream, total 8 streams, for
 *     channel width 80p80MHz.
 * @tx_mcs_80p80: Tx MCS map 2 bits for each stream, total 8 streams, for
 *     channel width 80p80MHz.
 */
struct ieee80211_he_mcs_nss_supp {
	__le16 rx_mcs_80;
	__le16 tx_mcs_80;
	__le16 rx_mcs_160;
	__le16 tx_mcs_160;
	__le16 rx_mcs_80p80;
	__le16 tx_mcs_80p80;
} __packed;

/*
 * struct ieee80211_he_cap_elem - HE capabilities element
 * @mac_cap_info: HE MAC Capabilities Information
 * @phy_cap_info: HE PHY Capabilities Information
 *
 * This structure represents the fixed fields of the payload of the
 * "HE capabilities element" as described in IEEE Std 802.11ax-2021
 * sections 9.4.2.248.2 and 9.4.2.248.3.
 */
struct ieee80211_he_cap_elem {
	u8 mac_cap_info[6];
	u8 phy_cap_info[11];
} __packed;

/*
 * struct cfg80211_bss_ies - BSS entry IE data
 * @tsf: TSF contained in the frame that carried these IEs
 * @rcu_head: internal use, for freeing
 * @len: length of the IEs
 * @from_beacon: these IEs are known to come from a beacon
 * @data: IE data
 */
struct cfg80211_bss_ies {
	u64 tsf;
	struct rcu_head rcu_head;
	int len;
	bool from_beacon;
	u8 data[];
};

/*
 * struct ieee80211_ht_cap - HT capabilities element
 * @cap_info: HT Capability Information
 * @ampdu_params_info: A-MPDU Parameters
 * @mcs: Supported MCS Set
 * @extended_ht_cap_info: HT Extended Capabilities
 * @tx_BF_cap_info: Transmit Beamforming Capabilities
 * @antenna_selection_info: ASEL Capability
 *
 * This structure represents the payload of the "HT Capabilities
 * element" as described in IEEE Std 802.11-2020 section 9.4.2.55.
 */
struct ieee80211_ht_cap {
	__le16 cap_info;
	u8 ampdu_params_info;

	/* 16 bytes MCS information */
	struct ieee80211_mcs_info mcs;

	__le16 extended_ht_cap_info;
	__le32 tx_BF_cap_info;
	u8 antenna_selection_info;
} __packed;

struct ieee80211_s1g_cap {
	u8 capab_info[10];
	u8 supp_mcs_nss[5];
} __packed;

/*
 * struct ieee80211_eht_cap_elem_fixed - EHT capabilities fixed data
 *
 * This structure is the "EHT Capabilities element" fixed fields as
 * described in P802.11be_D2.0 section 9.4.2.313.
 *
 * @mac_cap_info: MAC capabilities, see IEEE80211_EHT_MAC_CAP*
 * @phy_cap_info: PHY capabilities, see IEEE80211_EHT_PHY_CAP*
 */
struct ieee80211_eht_cap_elem_fixed {
	u8 mac_cap_info[2];
	u8 phy_cap_info[9];
} __packed;

/*
 * struct ieee80211_eht_cap_elem - EHT capabilities element
 * @fixed: fixed parts, see &ieee80211_eht_cap_elem_fixed
 * @optional: optional parts
 */
struct ieee80211_eht_cap_elem {
	struct ieee80211_eht_cap_elem_fixed fixed;

	/*
	 * Followed by:
	 * Supported EHT-MCS And NSS Set field: 4, 3, 6 or 9 octets.
	 * EHT PPE Thresholds field: variable length.
	 */
	u8 optional[];
} __packed;

/*
 * struct ieee80211_ampdu_params - AMPDU action parameters
 *
 * @action: the ampdu action, value from %ieee80211_ampdu_mlme_action.
 * @sta: peer of this AMPDU session
 * @tid: tid of the BA session
 * @ssn: start sequence number of the session. TX/RX_STOP can pass 0. When
 *	action is set to %IEEE80211_AMPDU_RX_START the driver passes back the
 *	actual ssn value used to start the session and writes the value here.
 * @buf_size: reorder buffer size  (number of subframes). Valid only when the
 *	action is set to %IEEE80211_AMPDU_RX_START or
 *	%IEEE80211_AMPDU_TX_OPERATIONAL
 * @amsdu: indicates the peer's ability to receive A-MSDU within A-MPDU.
 *	valid when the action is set to %IEEE80211_AMPDU_TX_OPERATIONAL
 * @timeout: BA session timeout. Valid only when the action is set to
 *	%IEEE80211_AMPDU_RX_START
 */
struct ieee80211_ampdu_params {
	enum ieee80211_ampdu_mlme_action action;
	struct ieee80211_sta *sta;
	u16 tid;
	u16 ssn;
	u16 buf_size;
	bool amsdu;
	u16 timeout;
};

/*
 * struct ieee80211_he_mu_edca_param_ac_rec - MU AC Parameter Record field
 * @aifsn: ACI/AIFSN
 * @ecw_min_max: ECWmin/ECWmax
 * @mu_edca_timer: MU EDCA Timer
 *
 * This structure represents the "MU AC Parameter Record" as described
 * in IEEE Std 802.11ax-2021 section 9.4.2.251, Figure 9-788p.
 */
struct ieee80211_he_mu_edca_param_ac_rec {
	u8 aifsn;
	u8 ecw_min_max;
	u8 mu_edca_timer;
} __packed;

/*
 * struct ieee80211_tx_queue_params - transmit queue configuration
 *
 * The information provided in this structure is required for QoS
 * transmit queue configuration. Cf. IEEE 802.11 7.3.2.29.
 *
 * @aifs: arbitration interframe space [0..255]
 * @cw_min: minimum contention window [a value of the form
 *	2^n-1 in the range 1..32767]
 * @cw_max: maximum contention window [like @cw_min]
 * @txop: maximum burst time in units of 32 usecs, 0 meaning disabled
 * @acm: is mandatory admission control required for the access category
 * @uapsd: is U-APSD mode enabled for the queue
 * @mu_edca: is the MU EDCA configured
 * @mu_edca_param_rec: MU EDCA Parameter Record for HE
 */
struct ieee80211_tx_queue_params {
	u16 txop;
	u16 cw_min;
	u16 cw_max;
	u8 aifs;
	bool acm;
	bool uapsd;
	bool mu_edca;
	struct ieee80211_he_mu_edca_param_ac_rec mu_edca_param_rec;
};

struct ieee80211_bss {
	u32 device_ts_beacon;
	u32 device_ts_presp;

	bool wmm_used;
	bool uapsd_supported;

	#define IEEE80211_MAX_SUPP_RATES 32
	u8 supp_rates[IEEE80211_MAX_SUPP_RATES];
	size_t supp_rates_len;
	struct ieee80211_rate *beacon_rate;

	u32 vht_cap_info;

	/*
	 * During association, we save an ERP value from a probe response so
	 * that we can feed ERP info to the driver when handling the
	 * association completes. these fields probably won't be up-to-date
	 * otherwise, you probably don't want to use them.
	 */
	bool has_erp_value;
	u8 erp_value;

	/* Keep track of the corruption of the last beacon/probe response. */
	u8 corrupt_data;

	/* Keep track of what bits of information we have valid info for. */
	u8 valid_data;
};

struct chan_def {
	int freq;
	int hw_value;
	enum nl80211_band band;
	u32 flags;
};

/*
 * struct cfg80211_bss - BSS description
 *
 * This structure describes a BSS (which may also be a mesh network)
 * for use in scan results and similar.
 *
 * @channel: channel this BSS is on
 * @bssid: BSSID of the BSS
 * @beacon_interval: the beacon interval as from the frame
 * @capability: the capability field in host byte order
 * @ies: the information elements (Note that there is no guarantee that these
 *	are well-formed!); this is a pointer to either the beacon_ies or
 *	proberesp_ies depending on whether Probe Response frame has been
 *	received. It is always non-%NULL.
 * @beacon_ies: the information elements from the last Beacon frame
 *	(implementation note: if @hidden_beacon_bss is set this struct doesn't
 *	own the beacon_ies, but they're just pointers to the ones from the
 *	@hidden_beacon_bss struct)
 * @proberesp_ies: the information elements from the last Probe Response frame
 * @proberesp_ecsa_stuck: ECSA element is stuck in the Probe Response frame,
 *	cannot rely on it having valid data
 * @hidden_beacon_bss: in case this BSS struct represents a probe response from
 *	a BSS that hides the SSID in its beacon, this points to the BSS struct
 *	that holds the beacon data. @beacon_ies is still valid, of course, and
 *	points to the same data as hidden_beacon_bss->beacon_ies in that case.
 * @transmitted_bss: pointer to the transmitted BSS, if this is a
 *	non-transmitted one (multi-BSSID support)
 * @nontrans_list: list of non-transmitted BSS, if this is a transmitted one
 *	(multi-BSSID support)
 * @signal: signal strength value (type depends on the wiphy's signal_type)
 * @chains: bitmask for filled values in @chain_signal.
 * @chain_signal: per-chain signal strength of last received BSS in dBm.
 * @bssid_index: index in the multiple BSS set
 * @max_bssid_indicator: max number of members in the BSS set
 * @use_for: bitmap of possible usage for this BSS, see
 *	&enum nl80211_bss_use_for
 * @cannot_use_reasons: the reasons (bitmap) for not being able to connect,
 *	if @restrict_use is set and @use_for is zero (empty); may be 0 for
 *	unspecified reasons; see &enum nl80211_bss_cannot_use_reasons
 * @priv: private area for driver use, has at least wiphy->bss_priv_size bytes
 */
struct cfg80211_bss {
	struct ieee80211_channel *channel;

	const struct cfg80211_bss_ies __rcu *ies;
	const struct cfg80211_bss_ies __rcu *beacon_ies;
	const struct cfg80211_bss_ies __rcu *proberesp_ies;

	struct cfg80211_bss *hidden_beacon_bss;
	struct cfg80211_bss *transmitted_bss;
	struct list_head nontrans_list;

	s32 signal;

	u16 beacon_interval;
	u16 capability;

	u8 bssid[ETH_ALEN];
	u8 chains;
	s8 chain_signal[IEEE80211_MAX_CHAINS];

	u8 proberesp_ecsa_stuck:1;

	u8 bssid_index;
	u8 max_bssid_indicator;

	u8 use_for;
	u8 cannot_use_reasons;

	u8 priv[] __aligned(sizeof(void *));
};

struct mcs_group {
	u8 shift;
	u16 duration[MCS_GROUP_RATES];
};

struct cfg80211_internal_bss {
	struct list_head list;
	struct list_head hidden_list;
	struct rb_node rbn;
	unsigned long ts;
	unsigned long refcount;
	atomic_uint hold;

	/* time at the start of the reception of the first octet of the
	 * timestamp field of the last beacon/probe received for this BSS.
	 * The time is the TSF of the BSS specified by %parent_bssid.
	 */
	u64 parent_tsf;

	/* the BSS according to which %parent_tsf is set. This is set to
	 * the BSS that the interface that requested the scan was connected to
	 * when the beacon/probe was received.
	 */
	u8 parent_bssid[ETH_ALEN] __aligned(2);

	enum bss_source_type bss_source;

	/* must be last because of priv member */
	struct cfg80211_bss pub;
};

struct ieee80211_hdr {
	__le16 frame_control;
	__le16 duration_id;
	struct_group(addrs,
		u8 addr1[ETH_ALEN];
		u8 addr2[ETH_ALEN];
		u8 addr3[ETH_ALEN];
	);
	__le16 seq_ctrl;
	u8 addr4[ETH_ALEN];
} __packed __aligned(2);

struct ieee80211_hdr_3addr {
	__le16 frame_control;
	__le16 duration_id;
	u8 addr1[ETH_ALEN];
	u8 addr2[ETH_ALEN];
	u8 addr3[ETH_ALEN];
	__le16 seq_ctrl;
} __packed __aligned(2);

struct ieee80211_qos_hdr {
	__le16 frame_control;
	__le16 duration_id;
	u8 addr1[ETH_ALEN];
	u8 addr2[ETH_ALEN];
	u8 addr3[ETH_ALEN];
	__le16 seq_ctrl;
	__le16 qos_ctrl;
} __packed __aligned(2);

struct ieee80211_qos_hdr_4addr {
	__le16 frame_control;
	__le16 duration_id;
	u8 addr1[ETH_ALEN];
	u8 addr2[ETH_ALEN];
	u8 addr3[ETH_ALEN];
	__le16 seq_ctrl;
	u8 addr4[ETH_ALEN];
	__le16 qos_ctrl;
} __packed __aligned(2);

struct ieee80211_trigger {
	__le16 frame_control;
	__le16 duration;
	u8 ra[ETH_ALEN];
	u8 ta[ETH_ALEN];
	__le64 common_info;
	u8 variable[];
} __packed __aligned(2);

struct ieee80211_pspoll {
	__le16 frame_control;
	__le16 aid;
	u8 bssid[ETH_ALEN];
	u8 ta[ETH_ALEN];
} __packed __aligned(2);

/*
 * struct ieee80211_multiple_bssid_configuration - multiple BSSID configuration
 *	element structure
 *
 * This structure refers to "Multiple BSSID Configuration element"
 *
 * @bssid_count: total number of active BSSIDs in the set
 * @profile_periodicity: the least number of beacon frames need to be received
 *	in order to discover all the nontransmitted BSSIDs in the set.
 */
struct ieee80211_multiple_bssid_configuration {
	u8 bssid_count;
	u8 profile_periodicity;
};

/*
 * struct ieee80211_s1g_oper_ie - S1G Operation element
 * @ch_width: S1G Operation Information Channel Width
 * @oper_class: S1G Operation Information Operating Class
 * @primary_ch: S1G Operation Information Primary Channel Number
 * @oper_ch: S1G Operation Information  Channel Center Frequency
 * @basic_mcs_nss: Basic S1G-MCS and NSS Set
 *
 * This structure represents the payload of the "S1G Operation
 * element" as described in IEEE Std 802.11-2020 section 9.4.2.212.
 */
struct ieee80211_s1g_oper_ie {
	u8 ch_width;
	u8 oper_class;
	u8 primary_ch;
	u8 oper_ch;
	__le16 basic_mcs_nss;
} __packed;

/*
 * ieee80211_he_6ghz_oper - HE 6 GHz operation Information field
 * @primary: primary channel
 * @control: control flags
 * @ccfs0: channel center frequency segment 0
 * @ccfs1: channel center frequency segment 1
 * @minrate: minimum rate (in 1 Mbps units)
 */
struct ieee80211_he_6ghz_oper {
	u8 primary;
	#define IEEE80211_HE_6GHZ_OPER_CTRL_CHANWIDTH		0x3
	#define	IEEE80211_HE_6GHZ_OPER_CTRL_CHANWIDTH_20MHZ	0
	#define	IEEE80211_HE_6GHZ_OPER_CTRL_CHANWIDTH_40MHZ	1
	#define	IEEE80211_HE_6GHZ_OPER_CTRL_CHANWIDTH_80MHZ	2
	#define	IEEE80211_HE_6GHZ_OPER_CTRL_CHANWIDTH_160MHZ	3
	#define IEEE80211_HE_6GHZ_OPER_CTRL_DUP_BEACON		0x4
	#define IEEE80211_HE_6GHZ_OPER_CTRL_REG_INFO		0x38
	u8 control;
	u8 ccfs0;
	u8 ccfs1;
	u8 minrate;
} __packed;

/*
 * struct ieee80211_rate - bitrate definition
 *
 * This structure describes a bitrate that an 802.11 PHY can
 * operate with. The two values @hw_value and @hw_value_short
 * are only for driver use when pointers to this structure are
 * passed around.
 *
 * @flags: rate-specific flags from &enum ieee80211_rate_flags
 * @bitrate: bitrate in units of 100 Kbps
 * @hw_value: driver/hardware value for this rate
 * @hw_value_short: driver/hardware value for this rate when
 *	short preamble is used
 */
struct ieee80211_rate {
	u32 flags;
	u16 bitrate;
	u16 hw_value;
	u16 hw_value_short;
};

/*
 * struct ieee80211_tx_rate - rate selection/status
 *
 * @idx: rate index to attempt to send with
 * @flags: rate control flags (&enum mac80211_rate_control_flags)
 * @count: number of tries in this rate before going to the next rate
 *
 * A value of -1 for @idx indicates an invalid rate and, if used
 * in an array of retry rates, that no more rates should be tried.
 *
 * When used for transmit status reporting, the driver should
 * always report the rate along with the flags it used.
 *
 * &struct ieee80211_tx_info contains an array of these structs
 * in the control information, and it will be filled by the rate
 * control algorithm according to what should be sent. For example,
 * if this array contains, in the format { <idx>, <count> } the
 * information::
 *
 *    { 3, 2 }, { 2, 2 }, { 1, 4 }, { -1, 0 }, { -1, 0 }
 *
 * then this means that the frame should be transmitted
 * up to twice at rate 3, up to twice at rate 2, and up to four
 * times at rate 1 if it doesn't get acknowledged. Say it gets
 * acknowledged by the peer after the fifth attempt, the status
 * information should then contain::
 *
 *   { 3, 2 }, { 2, 2 }, { 1, 1 }, { -1, 0 } ...
 *
 * since it was transmitted twice at rate 3, twice at rate 2
 * and once at rate 1 after which we received an acknowledgement.
 */
struct ieee80211_tx_rate {
	s8 idx;
	u16 count:5,
	    flags:11;
} __packed;

/*
 * struct cfg80211_scan_info - information about completed scan
 * @scan_start_tsf: scan start time in terms of the TSF of the BSS that the
 *	wireless device that requested the scan is connected to. If this
 *	information is not available, this field is left zero.
 * @tsf_bssid: the BSSID according to which %scan_start_tsf is set.
 * @aborted: set to true if the scan was aborted for any reason,
 *	userspace will be notified of that
 */
struct cfg80211_scan_info {
	u64 scan_start_tsf;
	u8 tsf_bssid[ETH_ALEN] __aligned(2);
	bool aborted;
};

/*
 * struct ieee80211_he_operation - HE Operation element
 * @he_oper_params: HE Operation Parameters + BSS Color Information
 * @he_mcs_nss_set: Basic HE-MCS And NSS Set
 * @optional: Optional fields VHT Operation Information, Max Co-Hosted
 *            BSSID Indicator, and 6 GHz Operation Information
 *
 * This structure represents the payload of the "HE Operation
 * element" as described in IEEE Std 802.11ax-2021 section 9.4.2.249.
 */
struct ieee80211_he_operation {
	__le32 he_oper_params;
	__le16 he_mcs_nss_set;
	u8 optional[];
} __packed;

/*
 * struct ieee80211_ht_operation - HT operation IE
 *
 * This structure is the "HT operation element" as
 * described in 802.11n-2009 7.3.2.57
 */
struct ieee80211_ht_operation {
	u8 primary_chan;
	u8 ht_param;
	__le16 operation_mode;
	__le16 stbc_param;
	u8 basic_set[16];
} __packed;

struct ps_data {
	/* yes, this looks ugly, but guarantees that we can later use
	 * bitmap_empty :)
	 * NB: don't touch this bitmap, use sta_info_{set,clear}_tim_bit */
	u8 tim[sizeof(unsigned long) * BITS_TO_LONGS(IEEE80211_MAX_AID + 1)]__aligned(__alignof__(unsigned long));
	struct sk_buff_head bc_buf;
	atomic_uint num_sta_ps; /* number of stations in PS mode */
	int dtim_count;
	bool dtim_bc_mc;
	int sb_count; /* num short beacons til next long beacon */
};

struct ieee80211_conn_settings {
	enum ieee80211_conn_mode mode;
	enum ieee80211_conn_bw_limit bw_limit;
};

struct ieee80211_mgd_auth_data {
	struct cfg80211_bss *bss;
	unsigned long timeout;
	int tries;
	u16 algorithm;
	u16 expected_transaction;

	unsigned long userspace_selectors[BITS_TO_LONGS(128)];

	u8 key[WLAN_KEY_LEN_WEP104];
	u8 key_len;
	u8 key_idx;
	bool done;
	bool waiting;
	bool peer_confirmed;
	bool timeout_started;
	int link_id;

	u8 ap_addr[ETH_ALEN] __aligned(2);

	u16 sae_trans;
	u16 sae_status;
	size_t data_len;
	u8 data[];
};

struct ieee80211_mgd_assoc_data {
	struct {
		struct cfg80211_bss *bss;

		u8 addr[ETH_ALEN] __aligned(2);

		u8 ap_ht_param;

		struct ieee80211_vht_cap ap_vht_cap;

		size_t elems_len;
		u8 *elems; /* pointing to inside ie[] below */

		struct ieee80211_conn_settings conn;

		u16 status;
	} link[IEEE80211_MLD_MAX_NUM_LINKS];

	u8 ap_addr[ETH_ALEN] __aligned(2);

	/* this is for a workaround, so we use it only for non-MLO */
	const u8 *supp_rates;
	u8 supp_rates_len;

	unsigned long timeout;
	int tries;

	u8 prev_ap_addr[ETH_ALEN];
	u8 ssid[IEEE80211_MAX_SSID_LEN];
	u8 ssid_len;
	bool wmm, uapsd;
	bool need_beacon;
	bool synced;
	bool timeout_started;
	bool comeback; /* whether the AP has requested association comeback */
	bool s1g;
	bool spp_amsdu;

	s8 assoc_link_id;

	__le16 ext_mld_capa_ops;

	u8 fils_nonces[2 * FILS_NONCE_LEN];
	u8 fils_kek[FILS_MAX_KEK_LEN];
	size_t fils_kek_len;

	size_t ie_len;
	u8 *ie_pos; /* used to fill ie[] with link[].elems */
	u8 ie[];
};

struct ieee80211_if_ap {
	struct list_head vlans; /* write-protected with RTNL and local->mtx */

	struct ps_data ps;
	atomic_uint num_mcast_sta; /* number of stations receiving multicast */

	bool multicast_to_unicast;
	bool active;
};

struct ieee80211_if_vlan {
	struct list_head list; /* write-protected with RTNL and local->mtx */

	/* used for all tx if the VLAN is configured to 4-addr mode */
	struct sta_info __rcu *sta;
	atomic_uint num_mcast_sta; /* number of stations receiving multicast */
};

struct ieee80211_if_managed {
	unsigned long beacon_timeout;
	unsigned long probe_timeout;
	int probe_send_count;
	bool nullfunc_failed;
	u8 connection_loss:1;
	u8 driver_disconnect:1;
	u8 reconnect:1;
	u8 associated:1;

	struct ieee80211_mgd_auth_data  auth_data;
	struct ieee80211_mgd_assoc_data assoc_data;

	unsigned long userspace_selectors[BITS_TO_LONGS(128)];

	bool powersave; /* powersave requested for this iface */
	bool broken_ap; /* AP is broken -- turn off powersave */

	unsigned int flags;

	u16 mcast_seq_last;

	bool status_acked;
	bool status_received;
	__le16 status_fc;

	enum ieee80211_mfp mfp;

	/*
	 * Bitmask of enabled u-apsd queues,
	 * IEEE80211_WMM_IE_STA_QOSINFO_AC_BE & co. Needs a new association
	 * to take effect.
	 */
	unsigned int uapsd_queues;

	/*
	 * Maximum number of buffered frames AP can deliver during a
	 * service period, IEEE80211_WMM_IE_STA_QOSINFO_SP_ALL or similar.
	 * Needs a new association to take effect.
	 */
	unsigned int uapsd_max_sp_len;

	u8 use_4addr;

	/*
	 * State variables for keeping track of RSSI of the AP currently
	 * connected to and informing driver when RSSI has gone
	 * below/above a certain threshold.
	 */
	int rssi_min_thold;
	int rssi_max_thold;

	/* Information elements from the last transmitted (Re)Association
	 * Request frame.
	 */
	u8 *assoc_req_ies;
	size_t assoc_req_ies_len;

	u16 removed_links;
	u8 dialog_token_alloc;
};

struct ieee80211_link_data_managed {
	u8 bssid[ETH_ALEN] __aligned(2);

	u8 dtim_period;
};

/*
 * struct ieee80211_meshconf_ie - Mesh Configuration element
 * @meshconf_psel: Active Path Selection Protocol Identifier
 * @meshconf_pmetric: Active Path Selection Metric Identifier
 * @meshconf_congest: Congestion Control Mode Identifier
 * @meshconf_synch: Synchronization Method Identifier
 * @meshconf_auth: Authentication Protocol Identifier
 * @meshconf_form: Mesh Formation Info
 * @meshconf_cap: Mesh Capability (see &enum mesh_config_capab_flags)
 *
 * This structure represents the payload of the "Mesh Configuration
 * element" as described in IEEE Std 802.11-2020 section 9.4.2.97.
 */
struct ieee80211_meshconf_ie {
	u8 meshconf_psel;
	u8 meshconf_pmetric;
	u8 meshconf_congest;
	u8 meshconf_synch;
	u8 meshconf_auth;
	u8 meshconf_form;
	u8 meshconf_cap;
} __packed;

/*
 * struct cfg80211_rnr_elems - Reduced neighbor report (RNR) elements
 *
 * @cnt: Number of elements in array %elems.
 *
 * @elem: Array of RNR element(s) to be added into Beacon frames.
 * @elem.data: Data for RNR elements.
 * @elem.len: Length of data.
 */
struct cfg80211_rnr_elems {
	u8 cnt;
	struct {
		const u8 *data;
		size_t len;
	} elem[];
};

/*
 * struct cfg80211_mbssid_elems - Multiple BSSID elements
 *
 * @cnt: Number of elements in array %elems.
 *
 * @elem: Array of multiple BSSID element(s) to be added into Beacon frames.
 * @elem.data: Data for multiple BSSID elements.
 * @elem.len: Length of data.
 */
struct cfg80211_mbssid_elems {
	u8 cnt;
	struct {
		const u8 *data;
		size_t len;
	} elem[];
};

struct beacon_data {
	u8 *head;
	u8 *tail;
	int head_len, tail_len;
	struct ieee80211_meshconf_ie *meshconf;
	u16 cntdwn_counter_offsets[2]; /* 'IEEE80211_MAX_CNTDWN_COUNTERS_NUM' = 2 */
	u8 cntdwn_current_counter;
	struct cfg80211_mbssid_elems *mbssid_ies;
	struct cfg80211_rnr_elems *rnr_ies;
	struct rcu_head rcu_head;
};

struct ieee80211_link_data_ap {
	struct beacon_data __rcu *beacon;
	struct probe_resp __rcu *probe_resp;
};

/*
 * struct ieee80211_channel - channel definition
 *
 * This structure describes a single channel for use
 * with cfg80211.
 *
 * @center_freq: center frequency in MHz
 * @freq_offset: offset from @center_freq, in KHz
 * @hw_value: hardware-specific value for the channel
 * @flags: channel flags from &enum ieee80211_channel_flags.
 * @orig_flags: channel flags at registration time, used by regulatory
 *	code to support devices with additional restrictions
 * @band: band this channel belongs to.
 * @max_antenna_gain: maximum antenna gain in dBi
 * @max_power: maximum transmission power (in dBm)
 * @max_reg_power: maximum regulatory transmission power (in dBm)
 * @beacon_found: helper to regulatory code to indicate when a beacon
 *	has been found on this channel. Use regulatory_hint_found_beacon()
 *	to enable this, this is useful only on 5 GHz band.
 * @orig_mag: internal use
 * @orig_mpwr: internal use
 * @dfs_state: current state of this channel. Only relevant if radar is required
 *	on this channel.
 * @dfs_state_entered: timestamp (jiffies) when the dfs state was entered.
 * @dfs_cac_ms: DFS CAC time in milliseconds, this is valid for DFS channels.
 * @psd: power spectral density (in dBm)
 */
struct ieee80211_channel {
	enum nl80211_band band;
	u32 center_freq;
	u16 freq_offset;
	u16 hw_value;
	u32 flags;
	int max_antenna_gain;
	int max_power;
	int max_reg_power;
	bool beacon_found;
	u32 orig_flags;
	int orig_mag, orig_mpwr;
	enum nl80211_dfs_state dfs_state;
	unsigned long dfs_state_entered;
	unsigned int dfs_cac_ms;
	s8 psd;
};

/*
 * struct ieee80211_edmg - EDMG configuration
 *
 * This structure describes most essential parameters needed
 * to describe 802.11ay EDMG configuration
 *
 * @channels: bitmap that indicates the 2.16 GHz channel(s)
 *	that are allowed to be used for transmissions.
 *	Bit 0 indicates channel 1, bit 1 indicates channel 2, etc.
 *	Set to 0 indicate EDMG not supported.
 * @bw_config: Channel BW Configuration subfield encodes
 *	the allowed channel bandwidth configurations
 */
struct ieee80211_edmg {
	u8 channels;
	enum ieee80211_edmg_bw_config bw_config;
};

/*
 * struct cfg80211_chan_def - channel definition
 * @chan: the (control) channel
 * @width: channel width
 * @center_freq1: center frequency of first segment
 * @center_freq2: center frequency of second segment
 *	(only with 80+80 MHz)
 * @edmg: define the EDMG channels configuration.
 *	If edmg is requested (i.e. the .channels member is non-zero),
 *	chan will define the primary channel and all other
 *	parameters are ignored.
 * @freq1_offset: offset from @center_freq1, in KHz
 * @punctured: mask of the punctured 20 MHz subchannels, with
 *	bits turned on being disabled (punctured); numbered
 *	from lower to higher frequency (like in the spec)
 * @s1g_primary_2mhz: Indicates if the control channel pointed to
 *	by 'chan' exists as a 1MHz primary subchannel within an
 *	S1G 2MHz primary channel.
 */
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

/*
 * struct ieee80211_chan_req - A channel "request"
 * @oper: channel definition to use for operation
 * @ap: the channel definition of the AP, if any
 *	(otherwise the chan member is %NULL)
 */
struct ieee80211_chan_req {
	struct cfg80211_chan_def oper;
	struct cfg80211_chan_def ap;
};

struct ieee80211_chanctx_conf {
	struct cfg80211_chan_def def;
	struct cfg80211_chan_def min_def;
	struct cfg80211_chan_def ap;
	int radio_idx;
	u8 rx_chains_static;
	u8 rx_chains_dynamic;
	bool radar_enabled;
	u8 drv_priv[] __aligned(sizeof(void *));
};

struct ieee80211_link_data {
	struct ieee80211_sub_if_data *sdata;
	unsigned int link_id;

	/* multicast keys only */
	struct ieee80211_key __rcu *gtk[NUM_DEFAULT_KEYS +
					NUM_DEFAULT_MGMT_KEYS +
					NUM_DEFAULT_BEACON_KEYS];
	struct ieee80211_key __rcu *default_multicast_key;
	struct ieee80211_key __rcu *default_mgmt_key;
	struct ieee80211_key __rcu *default_beacon_key;

	struct {
		struct wiphy_work finalize_work;
		struct ieee80211_chan_req chanreq;
	} csa;

	bool operating_11g_mode;

	int user_power_level; /* in dBm */
	int ap_power_level; /* in dBm */

	struct ieee80211_bss_conf *conf;

	union {
		struct ieee80211_link_data_managed mgd;
		struct ieee80211_link_data_ap ap;
	} u;
};

struct ieee80211_sub_if_data {
	struct list_head list;

	/* keys */
	struct list_head key_list;

	/* count for keys needing tailroom space allocation */
	int crypto_tx_tailroom_needed_cnt;
	int crypto_tx_tailroom_pending_dec;

	struct net_device *dev;
	struct ieee80211_local *local;
	unsigned int flags;
	unsigned long state;

	char name[IFNAMSIZ];

	/* TID bitmap for NoAck policy */
	u16 noack_map;

	/* bit field of ACM bits (BIT(802.1D tag)) */
	u8 wmm_acm;

	u16 sequence_number;
	u16 mld_mcast_seq;
	__be16 control_port_protocol;
	bool control_port_no_encrypt;
	bool control_port_no_preauth;
	bool control_port_over_nl80211;

	struct sk_buff_head skb_queue;
	struct sk_buff_head status_queue;

	/*
	 * AP this belongs to: self in AP mode and
	 * corresponding AP in VLAN mode, NULL for
	 * all others (might be needed later in IBSS)
	 */
	struct ieee80211_if_ap *bss;

	/* bitmap of allowed (non-MCS) rate indexes for rate control */
	u32 rc_rateidx_mask[NUM_NL80211_BANDS];

	bool rc_has_mcs_mask[NUM_NL80211_BANDS];
	u8  rc_rateidx_mcs_mask[NUM_NL80211_BANDS][IEEE80211_HT_MCS_MASK_LEN];

	bool rc_has_vht_mcs_mask[NUM_NL80211_BANDS];
	u16 rc_rateidx_vht_mcs_mask[NUM_NL80211_BANDS][NL80211_VHT_NSS_MAX];

	/* Beacon frame (non-MCS) rate (as a bitmap) */
	u32 beacon_rateidx_mask[NUM_NL80211_BANDS];
	bool beacon_rate_set;

	union {
		struct ieee80211_if_ap ap;
		struct ieee80211_if_vlan vlan;
		struct ieee80211_if_managed mgd;
	} u;

	struct ieee80211_key __rcu *keys[NUM_DEFAULT_KEYS];
	struct ieee80211_key __rcu *default_unicast_key;

	struct ieee80211_link_data deflink;
	struct ieee80211_link_data __rcu *link[IEEE80211_MLD_MAX_NUM_LINKS];

	u32 tx_handlers_drop;
	/* must be last, dynamically sized area in this! */
	struct ieee80211_vif *vif;
};

struct ieee80211_rx_data {
	struct list_head *list;
	struct sk_buff *skb;
	struct ieee80211_local *local;
	struct ieee80211_sub_if_data *sdata;
	struct ieee80211_link_data *link;
	struct sta_info *sta;
	struct link_sta_info *link_sta;
	struct ieee80211_key *key;

	unsigned int flags;

	/*
	 * Index into sequence numbers array, 0..16
	 * since the last (16) is used for non-QoS,
	 * will be 16 on non-QoS frames.
	 */
	int seqno_idx;

	/*
	 * Index into the security IV/PN arrays, 0..16
	 * since the last (16) is used for CCMP-encrypted
	 * management frames, will be set to 16 on mgmt
	 * frames and 0 on non-QoS frames.
	 */
	int security_idx;

	int link_id;

	union {
		struct {
			u32 iv32;
			u16 iv16;
		} tkip;
		struct {
			u8 pn[IEEE80211_CCMP_PN_LEN];
		} ccm_gcm;
	};
};

/*
 * struct ieee80211_fast_rx - RX fastpath information
 * @dev: netdevice for reporting the SKB
 * @vif_type: (P2P-less) interface type of the original sdata (sdata->vif.type)
 * @vif_addr: interface address
 * @rfc1042_hdr: copy of the RFC 1042 SNAP header (to have in cache)
 * @control_port_protocol: control port protocol copied from sdata
 * @expected_ds_bits: from/to DS bits expected
 * @icv_len: length of the MIC if present
 * @key: bool indicating encryption is expected (key is set)
 * @internal_forward: forward froms internally on AP/VLAN type interfaces
 * @uses_rss: copy of USES_RSS hw flag
 * @da_offs: offset of the DA in the header (for header conversion)
 * @sa_offs: offset of the SA in the header (for header conversion)
 * @rcu_head: RCU head for freeing this structure
 */
struct ieee80211_fast_rx {
	struct net_device *dev;
	enum nl80211_iftype vif_type;
	u8 vif_addr[ETH_ALEN] __aligned(2);
	u8 rfc1042_hdr[6] __aligned(2);
	__be16 control_port_protocol;
	__le16 expected_ds_bits;
	u8 icv_len;
	u8 key:1;
	u8 internal_forward:1;
	u8 uses_rss:1;
	u8 da_offs;
	u8 sa_offs;
	struct rcu_head rcu_head;
};

/*
 * struct ieee80211_vif_cfg - interface configuration
 * @assoc: association status
 * @ibss_joined: indicates whether this station is part of an IBSS or not
 * @ibss_creator: indicates if a new IBSS network is being created
 * @ps: power-save mode (STA only). This flag is NOT affected by
 *	offchannel/dynamic_ps operations.
 * @aid: association ID number, valid only when @assoc is true
 * @eml_cap: EML capabilities as described in P802.11be_D4.1 Figure 9-1001j.
 * @eml_med_sync_delay: Medium Synchronization delay as described in
 *	P802.11be_D4.1 Figure 9-1001i.
 * @mld_capa_op: MLD Capabilities and Operations per P802.11be_D4.1
 *	Figure 9-1001k
 * @arp_addr_list: List of IPv4 addresses for hardware ARP filtering. The
 *	may filter ARP queries targeted for other addresses than listed here.
 *	The driver must allow ARP queries targeted for all address listed here
 *	to pass through. An empty list implies no ARP queries need to pass.
 * @arp_addr_cnt: Number of addresses currently on the list. Note that this
 *	may be larger than %IEEE80211_BSS_ARP_ADDR_LIST_LEN (the arp_addr_list
 *	array size), it's up to the driver what to do in that case.
 * @ssid: The SSID of the current vif. Valid in AP and IBSS mode.
 * @ssid_len: Length of SSID given in @ssid.
 * @s1g: BSS is S1G BSS (affects Association Request format).
 * @idle: This interface is idle. There's also a global idle flag in the
 *	hardware config which may be more appropriate depending on what
 *	your driver/device needs to do.
 * @ap_addr: AP MLD address, or BSSID for non-MLO connections
 *	(station mode only)
 */
struct ieee80211_vif_cfg {
	/* association related data */
	bool assoc, ibss_joined;
	bool ibss_creator;
	bool ps;
	u16 aid;
	u16 eml_cap;
	u16 eml_med_sync_delay;
	u16 mld_capa_op;

	__be32 arp_addr_list[IEEE80211_BSS_ARP_ADDR_LIST_LEN];
	int arp_addr_cnt;
	u8 ssid[IEEE80211_MAX_SSID_LEN];
	size_t ssid_len;
	bool s1g;
	bool idle;
	u8 ap_addr[ETH_ALEN] __aligned(2);
};

/*
 * struct vif_params - describes virtual interface parameters
 * @flags: monitor interface flags, unchanged if 0, otherwise
 *	%MONITOR_FLAG_CHANGED will be set
 * @use_4addr: use 4-address frames
 * @macaddr: address to use for this virtual interface.
 *	If this parameter is set to zero address the driver may
 *	determine the address as needed.
 *	This feature is only fully supported by drivers that enable the
 *	%NL80211_FEATURE_MAC_ON_CREATE flag.  Others may support creating
 **	only p2p devices with specified MAC.
 * @vht_mumimo_groups: MU-MIMO groupID, used for monitoring MU-MIMO packets
 *	belonging to that MU-MIMO groupID; %NULL if not changed
 * @vht_mumimo_follow_addr: MU-MIMO follow address, used for monitoring
 *	MU-MIMO packets going to the specified station; %NULL if not changed
 */
struct vif_params {
	u32 flags;
	int use_4addr;
	u8 macaddr[ETH_ALEN];
	const u8 *vht_mumimo_groups;
	const u8 *vht_mumimo_follow_addr;
};

/*
 * struct ieee80211_txq - Software intermediate tx queue
 *
 * @vif: &struct ieee80211_vif pointer from the add_interface callback.
 * @sta: station table entry, %NULL for per-vif queue
 * @tid: the TID for this queue (unused for per-vif queue),
 *	%IEEE80211_NUM_TIDS for non-data (if enabled)
 * @ac: the AC for this queue
 * @drv_priv: driver private area, sized by hw->txq_data_size
 *
 * The driver can obtain packets from this queue by calling
 * ieee80211_tx_dequeue().
 */
struct ieee80211_txq {
	struct ieee80211_vif *vif;
	struct ieee80211_sta *sta;
	u8 tid;
	u8 ac;

	/* must be last */
	u8 drv_priv[] __aligned(sizeof(void *));
};

/*
 * struct sta_info - STA information
 *
 * This structure collects information about a station that
 * mac80211 is communicating with.
 * The only difference here is that it had been reimplemenented for userspace :)
 */
struct sta_info {
	/* General information, mostly static */
	struct list_head list;
	struct list_head free_list;
	u8 addr[ETH_ALEN];
	u16 listen_interval;
	bool dead;
	bool removed;
	bool uploaded;
	enum ieee80211_sta_state sta_state;
	/* use the accessors defined below */
	unsigned long _flags;

	struct rhlist_head hash_node;

	struct sk_buff_head ps_tx_buf[IEEE80211_NUM_ACS];
	struct sk_buff_head tx_filtered[IEEE80211_NUM_ACS];
	unsigned long driver_buffered_tids;
	unsigned long txq_buffered_tids;

	u64 assoc_at;
	long last_connected;

	/* Plus 1 for non-QoS frames */
	__le16 last_seq_ctrl[IEEE80211_NUM_TIDS + 1];
	u16 tid_seq[IEEE80211_QOS_CTL_TID_MASK + 1];
	u16 airtime_weight;

	struct ieee80211_sub_if_data sdata;
	struct ieee80211_sta *sta;
	struct ieee80211_vif *vif;
};

struct link_sta_info {
	u8 addr[ETH_ALEN];
	u8 link_id;

	u8 op_mode_nss;
	u8 capa_nss;

	struct rhlist_head link_hash_node;

	struct sta_info *sta;
};

struct regulatory_request {
	struct rcu_head rcu_head;
	int wiphy_idx;
	enum nl80211_reg_initiator initiator;
	enum nl80211_user_reg_hint_type user_reg_hint_type;
	char alpha2[3];
	enum nl80211_dfs_regions dfs_region;
	bool intersect;
	bool processed;
	enum environment_cap country_ie_env;
	struct list_head list;
};

/*
 * struct ieee80211_key_seq - key sequence counter
 *
 * @tkip: TKIP data, containing IV32 and IV16 in host byte order
 * @ccmp: PN data, most significant byte first (big endian,
 *	reverse order than in packet)
 * @aes_cmac: PN data, most significant byte first (big endian,
 *	reverse order than in packet)
 * @aes_gmac: PN data, most significant byte first (big endian,
 *	reverse order than in packet)
 * @gcmp: PN data, most significant byte first (big endian,
 *	reverse order than in packet)
 * @hw: data for HW-only (e.g. cipher scheme) keys
 */
struct ieee80211_key_seq {
	union {
		struct {
			u32 iv32;
			u16 iv16;
		} tkip;
		struct {
			u8 pn[6];
		} ccmp;
		struct {
			u8 pn[6];
		} aes_cmac;
		struct {
			u8 pn[6];
		} aes_gmac;
		struct {
			u8 pn[6];
		} gcmp;
		struct {
			u8 seq[IEEE80211_MAX_PN_LEN];
			u8 seq_len;
		} hw;
	};
};

/*
 * struct ieee80211_key_conf - key information
 *
 * This key information is given by mac80211 to the driver by
 * the set_key() callback in &struct ieee80211_ops.
 *
 * @hw_key_idx: To be set by the driver, this is the key index the driver
 *	wants to be given when a frame is transmitted and needs to be
 *	encrypted in hardware.
 * @cipher: The key's cipher suite selector.
 * @tx_pn: PN used for TX keys, may be used by the driver as well if it
 *	needs to do software PN assignment by itself (e.g. due to TSO)
 * @flags: key flags, see &enum ieee80211_key_flags.
 * @keyidx: the key index (0-7)
 * @keylen: key material length
 * @key: key material. For ALG_TKIP the key is encoded as a 256-bit (32 byte)
 * 	data block:
 * 	- Temporal Encryption Key (128 bits)
 * 	- Temporal Authenticator Tx MIC Key (64 bits)
 * 	- Temporal Authenticator Rx MIC Key (64 bits)
 * @icv_len: The ICV length for this key type
 * @iv_len: The IV length for this key type
 * @link_id: the link ID, 0 for non-MLO, or -1 for pairwise keys
 */
struct ieee80211_key_conf {
	atomic_long tx_pn;
	u32 cipher;
	u8 icv_len;
	u8 iv_len;
	u8 hw_key_idx;
	s8 keyidx;
	u16 flags;
	s8 link_id;
	u8 keylen;
	u8 key[];
};

struct tkip_ctx {
	u16 p1k[5];	/* p1k cache */
	u32 p1k_iv32;	/* iv32 for which p1k computed */
	enum ieee80211_internal_tkip_state state;
};

struct tkip_ctx_rx {
	struct tkip_ctx ctx;
	u32 iv32;	/* current iv32 */
	u16 iv16;	/* current iv16 */
};

struct ieee80211_key {
	struct sta_info *sta;
	/* for sdata list */
	struct list_head list;

	/* protected by key mutex */
	unsigned int flags;

	union {
		struct {
			/* protects tx context */
			pthread_mutex_t txlock;

			/* last used TSC */
			struct tkip_ctx tx;

			/* last received RSC */
			struct tkip_ctx_rx rx[IEEE80211_NUM_TIDS];

			/* number of mic failures */
			u32 mic_failures;
		} tkip;
		struct {
			/*
			 * Last received packet number. The first
			 * IEEE80211_NUM_TIDS counters are used with Data
			 * frames and the last counter is used with Robust
			 * Management frames.
			 */
			u8 rx_pn[IEEE80211_NUM_TIDS + 1][IEEE80211_CCMP_PN_LEN];
			struct crypto_aead *tfm;
			u32 replays; /* dot11RSNAStatsCCMPReplays */
		} ccmp;
		struct {
			u8 rx_pn[IEEE80211_CMAC_PN_LEN];
			struct crypto_shash *tfm;
			u32 replays; /* dot11RSNAStatsCMACReplays */
			u32 icverrors; /* dot11RSNAStatsCMACICVErrors */
		} aes_cmac;
		struct {
			u8 rx_pn[IEEE80211_GMAC_PN_LEN];
			struct crypto_aead *tfm;
			u32 replays; /* dot11RSNAStatsCMACReplays */
			u32 icverrors; /* dot11RSNAStatsCMACICVErrors */
		} aes_gmac;
		struct {
			/* Last received packet number. The first
			 * IEEE80211_NUM_TIDS counters are used with Data
			 * frames and the last counter is used with Robust
			 * Management frames.
			 */
			u8 rx_pn[IEEE80211_NUM_TIDS + 1][IEEE80211_GCMP_PN_LEN];
			struct crypto_aead *tfm;
			u32 replays; /* dot11RSNAStatsGCMPReplays */
		} gcmp;
		struct {
			/* generic cipher scheme */
			u8 rx_pn[IEEE80211_NUM_TIDS + 1][IEEE80211_MAX_PN_LEN];
		} gen;
	} u;

	unsigned int color;

	/*
	 * key config, must be last because it contains key
	 * material as variable length member
	 */
	struct ieee80211_key_conf conf;
};

/*
 * struct rfkill_ops - rfkill driver methods
 *
 * @poll: poll the rfkill block state(s) -- only assign this method
 *	when you need polling. When called, simply call one of the
 *	rfkill_set{,_hw,_sw}_state family of functions. If the hw
 *	is getting unblocked you need to take into account the return
 *	value of those functions to make sure the software block is
 *	properly used.
 * @query: query the rfkill block state(s) and call exactly one of the
 *	rfkill_set{,_hw,_sw}_state family of functions. Assign this
 *	method if input events can cause hardware state changes to make
 *	the rfkill core query your driver before setting a requested
 *	block.
 * @set_block: turn the transmitter on (blocked == false) or off
 *	(blocked == true) -- ignore and return 0 when hard blocked.
 *	This callback must be assigned.
 */
struct rfkill_ops {
	void	(*poll)(struct rfkill *rfkill, void *data);
	void	(*query)(struct rfkill *rfkill, void *data);
	int	(*set_block)(void *data, bool blocked);
};

/* first field of the struct associated with the rfkill and cfg80211_ops callbacks */
struct cfg80211_registered_device {
	const struct cfg80211_ops *ops;
	struct list_head list;

	/* rfkill support */
	struct rfkill_ops rfkill_ops;

	/* wiphy index, internal only */
	int wiphy_idx;

	/* protected by RTNL */
	int devlist_generation;
	int wdev_id;
	int opencount;
	struct list_head bss_list;
	u32 bss_generation;
	u32 bss_entries;
	struct work_struct wiphy_work;
	struct list_head wiphy_work_list;
	struct list_head beacon_registrations;
	struct list_head sched_scan_req_list;
	struct wiphy *wiphy;
};

/*
 * struct ieee80211_rx_status - receive status
 *
 * The low-level driver should provide this information (the subset
 * supported by hardware) to the 802.11 code with each received
 * frame, in the skb's control buffer (cb).
 *
 * @mactime: value in microseconds of the 64-bit Time Synchronization Function
 * 	(TSF) timer when the first data symbol (MPDU) arrived at the hardware.
 * @boottime_ns: CLOCK_BOOTTIME timestamp the frame was received at, this is
 *	needed only for beacons and probe responses that update the scan cache.
 * @ack_tx_hwtstamp: Hardware timestamp for the ack TX in nanoseconds. Only
 *	needed for Timing measurement and Fine timing measurement action frames.
 *	Only reported by devices that have timestamping enabled.
 * @device_timestamp: arbitrary timestamp for the device, mac80211 doesn't use
 *	it but can store it and pass it back to the driver for synchronisation
 * @band: the active band when this frame was received
 * @freq: frequency the radio was tuned to when receiving this frame, in MHz
 *	This field must be set for management frames, but isn't strictly needed
 *	for data (other) frames - for those it only affects radiotap reporting.
 * @freq_offset: @freq has a positive offset of 500Khz.
 * @signal: signal strength when receiving this frame, either in dBm, in dB or
 *	unspecified depending on the hardware capabilities flags
 *	@IEEE80211_HW_SIGNAL_*
 * @chains: bitmask of receive chains for which separate signal strength
 *	values were filled.
 * @chain_signal: per-chain signal strength, in dBm (unlike @signal, doesn't
 *	support dB or unspecified units)
 * @antenna: antenna used
 * @rate_idx: index of data rate into band's supported rates or MCS index if
 *	HT or VHT is used (%RX_FLAG_HT/%RX_FLAG_VHT)
 * @nss: number of streams (VHT, HE and EHT only)
 * @flag: %RX_FLAG_\*
 * @encoding: &enum mac80211_rx_encoding
 * @bw: &enum rate_info_bw
 * @enc_flags: uses bits from &enum mac80211_rx_encoding_flags
 * @he_ru: HE RU, from &enum nl80211_he_ru_alloc
 * @he_gi: HE GI, from &enum nl80211_he_gi
 * @he_dcm: HE DCM value
 * @eht: EHT specific rate information
 * @eht.ru: EHT RU, from &enum nl80211_eht_ru_alloc
 * @eht.gi: EHT GI, from &enum nl80211_eht_gi
 * @rx_flags: internal RX flags for mac80211
 * @ampdu_reference: A-MPDU reference number, must be a different value for
 *	each A-MPDU but the same for each subframe within one A-MPDU
 * @zero_length_psdu_type: radiotap type of the 0-length PSDU
 * @link_valid: if the link which is identified by @link_id is valid. This flag
 *	is set only when connection is MLO.
 * @link_id: id of the link used to receive the packet. This is used along with
 *	@link_valid.
 */
struct ieee80211_rx_status {
	u64 mactime;
	union {
		u64 boottime_ns;
		u64 ack_tx_hwtstamp;
	};
	u32 device_timestamp;
	u32 ampdu_reference;
	u32 flag;
	u16 freq: 13, freq_offset: 1;
	u8 enc_flags;
	u8 encoding:3, bw:4;
	union {
		struct {
			u8 he_ru:3;
			u8 he_gi:2;
			u8 he_dcm:1;
		};
		struct {
			u8 ru:4;
			u8 gi:2;
		} eht;
	};
	u8 rate_idx;
	u8 nss;
	u8 rx_flags;
	u8 band;
	u8 antenna;
	s8 signal;
	u8 chains;
	s8 chain_signal[IEEE80211_MAX_CHAINS];
	u8 zero_length_psdu_type;
	u8 link_valid:1, link_id:4;
};

/*
 * struct cfg80211_fils_resp_params - FILS connection response params
 * @kek: KEK derived from a successful FILS connection (may be %NULL)
 * @kek_len: Length of @fils_kek in octets
 * @update_erp_next_seq_num: Boolean value to specify whether the value in
 *	@erp_next_seq_num is valid.
 * @erp_next_seq_num: The next sequence number to use in ERP message in
 *	FILS Authentication. This value should be specified irrespective of the
 *	status for a FILS connection.
 * @pmk: A new PMK if derived from a successful FILS connection (may be %NULL).
 * @pmk_len: Length of @pmk in octets
 * @pmkid: A new PMKID if derived from a successful FILS connection or the PMKID
 *	used for this FILS connection (may be %NULL).
 */
struct cfg80211_fils_resp_params {
	const u8 *kek;
	size_t kek_len;
	bool update_erp_next_seq_num;
	u16 erp_next_seq_num;
	const u8 *pmk;
	size_t pmk_len;
	const u8 *pmkid;
};

/*
 * struct cfg80211_connect_resp_params - Connection response params
 * @status: Status code, %WLAN_STATUS_SUCCESS for successful connection, use
 *	%WLAN_STATUS_UNSPECIFIED_FAILURE if your device cannot give you
 *	the real status code for failures. If this call is used to report a
 *	failure due to a timeout (e.g., not receiving an Authentication frame
 *	from the AP) instead of an explicit rejection by the AP, -1 is used to
 *	indicate that this is a failure, but without a status code.
 *	@timeout_reason is used to report the reason for the timeout in that
 *	case.
 * @req_ie: Association request IEs (may be %NULL)
 * @req_ie_len: Association request IEs length
 * @resp_ie: Association response IEs (may be %NULL)
 * @resp_ie_len: Association response IEs length
 * @fils: FILS connection response parameters.
 * @timeout_reason: Reason for connection timeout. This is used when the
 *	connection fails due to a timeout instead of an explicit rejection from
 *	the AP. %NL80211_TIMEOUT_UNSPECIFIED is used when the timeout reason is
 *	not known. This value is used only if @status < 0 to indicate that the
 *	failure is due to a timeout and not due to explicit rejection by the AP.
 *	This value is ignored in other cases (@status >= 0).
 * @valid_links: For MLO connection, BIT mask of the valid link ids. Otherwise
 *	zero.
 * @ap_mld_addr: For MLO connection, MLD address of the AP. Otherwise %NULL.
 * @links : For MLO connection, contains link info for the valid links indicated
 *	using @valid_links. For non-MLO connection, links[0] contains the
 *	connected AP info.
 * @links.addr: For MLO connection, MAC address of the STA link. Otherwise
 *	%NULL.
 * @links.bssid: For MLO connection, MAC address of the AP link. For non-MLO
 *	connection, links[0].bssid points to the BSSID of the AP (may be %NULL).
 * @links.bss: For MLO connection, entry of bss to which STA link is connected.
 *	For non-MLO connection, links[0].bss points to entry of bss to which STA
 *	is connected. It can be obtained through cfg80211_get_bss() (may be
 *	%NULL). It is recommended to store the bss from the connect_request and
 *	hold a reference to it and return through this param to avoid a warning
 *	if the bss is expired during the connection, esp. for those drivers
 *	implementing connect op. Only one parameter among @bssid and @bss needs
 *	to be specified.
 * @links.status: per-link status code, to report a status code that's not
 *	%WLAN_STATUS_SUCCESS for a given link, it must also be in the
 *	@valid_links bitmap and may have a BSS pointer (which is then released)
 */
struct cfg80211_connect_resp_params {
	int status;
	const u8 *req_ie;
	size_t req_ie_len;
	const u8 *resp_ie;
	size_t resp_ie_len;
	struct cfg80211_fils_resp_params fils;
	enum nl80211_timeout_reason timeout_reason;

	const u8 *ap_mld_addr;
	u16 valid_links;
	struct {
		const u8 *addr;
		const u8 *bssid;
		struct cfg80211_bss *bss;
		u16 status;
	} links[IEEE80211_MLD_MAX_NUM_LINKS];
};

/*
 * struct cfg80211_roam_info - driver initiated roaming information
 *
 * @req_ie: association request IEs (maybe be %NULL)
 * @req_ie_len: association request IEs length
 * @resp_ie: association response IEs (may be %NULL)
 * @resp_ie_len: assoc response IEs length
 * @fils: FILS related roaming information.
 * @valid_links: For MLO roaming, BIT mask of the new valid links is set.
 *	Otherwise zero.
 * @ap_mld_addr: For MLO roaming, MLD address of the new AP. Otherwise %NULL.
 * @links : For MLO roaming, contains new link info for the valid links set in
 *	@valid_links. For non-MLO roaming, links[0] contains the new AP info.
 * @links.addr: For MLO roaming, MAC address of the STA link. Otherwise %NULL.
 * @links.bssid: For MLO roaming, MAC address of the new AP link. For non-MLO
 *	roaming, links[0].bssid points to the BSSID of the new AP. May be
 *	%NULL if %links.bss is set.
 * @links.channel: the channel of the new AP.
 * @links.bss: For MLO roaming, entry of new bss to which STA link got
 *	roamed. For non-MLO roaming, links[0].bss points to entry of bss to
 *	which STA got roamed (may be %NULL if %links.bssid is set)
 */
struct cfg80211_roam_info {
	const u8 *req_ie;
	size_t req_ie_len;
	const u8 *resp_ie;
	size_t resp_ie_len;
	struct cfg80211_fils_resp_params fils;

	const u8 *ap_mld_addr;
	u16 valid_links;
	struct {
		const u8 *addr;
		const u8 *bssid;
		struct ieee80211_channel *channel;
		struct cfg80211_bss *bss;
	} links[IEEE80211_MLD_MAX_NUM_LINKS];
};

struct cfg80211_event {
	struct list_head list;
	enum cfg80211_event_type type;

	union {
		struct cfg80211_connect_resp_params cr;
		struct cfg80211_roam_info rm;
		struct {
			const u8 *ie;
			size_t ie_len;
			u16 reason;
			bool locally_generated;
		} dc;
		struct {
			u8 bssid[ETH_ALEN];
			struct ieee80211_channel *channel;
		} ij;
		struct {
			u8 peer_addr[ETH_ALEN];
			const u8 *td_bitmap;
			u8 td_bitmap_len;
		} pa;
	};
};

struct cfg80211_beacon_registration {
	struct list_head list;
	u32 nlportid;
};

struct cfg80211_cqm_config {
	struct rcu_head rcu_head;
	u32 rssi_hyst;
	s32 last_rssi_event_value;
	enum nl80211_cqm_rssi_threshold_event last_rssi_event_type;
	bool use_range_api;
	int n_rssi_thresholds;
	s32 rssi_thresholds[] __counted_by(n_rssi_thresholds);
};

/*
 * struct ieee80211_scan_ies - descriptors for different blocks of IEs
 *
 * This structure is used to point to different blocks of IEs in HW scan
 * and scheduled scan. These blocks contain the IEs passed by userspace
 * and the ones generated by mac80211.
 *
 * @ies: pointers to band specific IEs.
 * @len: lengths of band_specific IEs.
 * @common_ies: IEs for all bands (especially vendor specific ones)
 * @common_ie_len: length of the common_ies
 */
struct ieee80211_scan_ies {
	const u8 *ies[NUM_NL80211_BANDS];
	size_t len[NUM_NL80211_BANDS];
	const u8 *common_ies;
	size_t common_ie_len;
};

/*
 * struct ieee80211_tx_info - skb transmit information
 *
 * This structure is placed in skb->cb for three uses:
 *  (1) mac80211 TX control - mac80211 tells the driver what to do
 *  (2) driver internal use (if applicable)
 *  (3) TX status information - driver tells mac80211 what happened
 *
 * @flags: transmit info flags, defined above
 * @band: the band to transmit on (use e.g. for checking for races),
 *	not valid if the interface is an MLD since we won't know which
 *	link the frame will be transmitted on
 * @hw_queue: HW queue to put the frame on, skb_get_queue_mapping() gives the AC
 * @status_data: internal data for TX status handling, assigned privately,
 *	see also &enum ieee80211_status_data for the internal documentation
 * @status_data_idr: indicates status data is IDR allocated ID for ack frame
 * @tx_time_est: TX time estimate in units of 4us, used internally
 * @control: union part for control data
 * @control.rates: TX rates array to try
 * @control.rts_cts_rate_idx: rate for RTS or CTS
 * @control.use_rts: use RTS
 * @control.use_cts_prot: use RTS/CTS
 * @control.short_preamble: use short preamble (CCK only)
 * @control.skip_table: skip externally configured rate table
 * @control.jiffies: timestamp for expiry on powersave clients
 * @control.vif: virtual interface (may be NULL)
 * @control.hw_key: key to encrypt with (may be NULL)
 * @control.flags: control flags, see &enum mac80211_tx_control_flags
 * @control.enqueue_time: enqueue time (for iTXQs)
 * @driver_rates: alias to @control.rates to reserve space
 * @pad: padding
 * @rate_driver_data: driver use area if driver needs @control.rates
 * @status: union part for status data
 * @status.rates: attempted rates
 * @status.ack_signal: ACK signal
 * @status.ampdu_ack_len: AMPDU ack length
 * @status.ampdu_len: AMPDU length
 * @status.antenna: (legacy, kept only for iwlegacy)
 * @status.tx_time: airtime consumed for transmission; note this is only
 *	used for WMM AC, not for airtime fairness
 * @status.flags: status flags, see &enum mac80211_tx_status_flags
 * @status.status_driver_data: driver use area
 * @ack: union part for pure ACK data
 * @ack.cookie: cookie for the ACK
 * @driver_data: array of driver_data pointers
 */
struct ieee80211_tx_info {
	/* common information */
	u32 flags;
	u32 band:3,
	    status_data_idr:1,
	    status_data:13,
	    hw_queue:4,
	    tx_time_est:10;
	/* 1 free bit */

	union {
		struct {
			union {
				/* rate control */
				struct {
					struct ieee80211_tx_rate rates[IEEE80211_TX_MAX_RATES];
					s8 rts_cts_rate_idx;
					u8 use_rts:1;
					u8 use_cts_prot:1;
					u8 short_preamble:1;
					u8 skip_table:1;

					/* for injection only (bitmap) */
					u8 antennas:2;

					/* 14 bits free */
				};
				/* only needed before rate control */
				unsigned long jiffies;
			};
			/* NB: vif can be NULL for injected frames */
			struct ieee80211_vif *vif;
			struct ieee80211_key_conf *hw_key;
			u32 flags;
			u32 enqueue_time;
		} control;
		struct {
			u64 cookie;
		} ack;
		struct {
			struct ieee80211_tx_rate rates[IEEE80211_TX_MAX_RATES];
			s32 ack_signal;
			u8 ampdu_ack_len;
			u8 ampdu_len;
			u8 antenna;
			u8 pad;
			u16 tx_time;
			u8 flags;
			u8 pad2;
			void *status_driver_data[16 / sizeof(void *)];
		} status;
		struct {
			struct ieee80211_tx_rate driver_rates[IEEE80211_TX_MAX_RATES];
			u8 pad[4];

			void *rate_driver_data[IEEE80211_TX_INFO_RATE_DRIVER_DATA_SIZE / sizeof(void *)];
		};
		void *driver_data[IEEE80211_TX_INFO_DRIVER_DATA_SIZE / sizeof(void *)];
	};
};

/*
 * struct cfg80211_pmsr_capabilities - cfg80211 peer measurement capabilities
 * @max_peers: maximum number of peers in a single measurement
 * @report_ap_tsf: can report assoc AP's TSF for radio resource measurement
 * @randomize_mac_addr: can randomize MAC address for measurement
 * @ftm: FTM measurement data
 * @ftm.supported: FTM measurement is supported
 * @ftm.asap: ASAP-mode is supported
 * @ftm.non_asap: non-ASAP-mode is supported
 * @ftm.request_lci: can request LCI data
 * @ftm.request_civicloc: can request civic location data
 * @ftm.preambles: bitmap of preambles supported (&enum nl80211_preamble)
 * @ftm.bandwidths: bitmap of bandwidths supported (&enum nl80211_chan_width)
 * @ftm.max_bursts_exponent: maximum burst exponent supported
 *	(set to -1 if not limited; note that setting this will necessarily
 *	forbid using the value 15 to let the responder pick)
 * @ftm.max_ftms_per_burst: maximum FTMs per burst supported (set to 0 if
 *	not limited)
 * @ftm.trigger_based: trigger based ranging measurement is supported
 * @ftm.non_trigger_based: non trigger based ranging measurement is supported
 */
struct cfg80211_pmsr_capabilities {
	unsigned int max_peers;
	u8 report_ap_tsf:1,
	   randomize_mac_addr:1;

	struct {
		u32 preambles;
		u32 bandwidths;
		s8 max_bursts_exponent;
		u8 max_ftms_per_burst;
		u8 supported:1,
		   asap:1,
		   non_asap:1,
		   request_lci:1,
		   request_civicloc:1,
		   trigger_based:1,
		   non_trigger_based:1;
	} ftm;
};

/*
 * struct ieee80211_sta_he_cap - STA's HE capabilities
 *
 * This structure describes most essential parameters needed
 * to describe 802.11ax HE capabilities for a STA.
 *
 * @has_he: true iff HE data is valid.
 * @he_cap_elem: Fixed portion of the HE capabilities element.
 * @he_mcs_nss_supp: The supported NSS/MCS combinations.
 * @ppe_thres: Holds the PPE Thresholds data.
 */
struct ieee80211_sta_he_cap {
	bool has_he;
	struct ieee80211_he_cap_elem he_cap_elem;
	struct ieee80211_he_mcs_nss_supp he_mcs_nss_supp;
	u8 ppe_thres[IEEE80211_HE_PPE_THRES_MAX_LEN];
};

struct ieee80211_he_6ghz_capa {
	/* uses IEEE80211_HE_6GHZ_CAP_* below */
	__le16 capa;
} __packed;

/*
 * struct ieee80211_eht_mcs_nss_supp_20mhz_only - EHT 20MHz only station max
 * supported NSS for per MCS.
 *
 * For each field below, bits 0 - 3 indicate the maximal number of spatial
 * streams for Rx, and bits 4 - 7 indicate the maximal number of spatial streams
 * for Tx.
 *
 * @rx_tx_mcs7_max_nss: indicates the maximum number of spatial streams
 *     supported for reception and the maximum number of spatial streams
 *     supported for transmission for MCS 0 - 7.
 * @rx_tx_mcs9_max_nss: indicates the maximum number of spatial streams
 *     supported for reception and the maximum number of spatial streams
 *     supported for transmission for MCS 8 - 9.
 * @rx_tx_mcs11_max_nss: indicates the maximum number of spatial streams
 *     supported for reception and the maximum number of spatial streams
 *     supported for transmission for MCS 10 - 11.
 * @rx_tx_mcs13_max_nss: indicates the maximum number of spatial streams
 *     supported for reception and the maximum number of spatial streams
 *     supported for transmission for MCS 12 - 13.
 * @rx_tx_max_nss: array of the previous fields for easier loop access
 */
struct ieee80211_eht_mcs_nss_supp_20mhz_only {
	union {
		struct {
			u8 rx_tx_mcs7_max_nss;
			u8 rx_tx_mcs9_max_nss;
			u8 rx_tx_mcs11_max_nss;
			u8 rx_tx_mcs13_max_nss;
		};
		u8 rx_tx_max_nss[4];
	};
};

/*
 * struct ieee80211_eht_mcs_nss_supp_bw - EHT max supported NSS per MCS (except
 * 20MHz only stations).
 *
 * For each field below, bits 0 - 3 indicate the maximal number of spatial
 * streams for Rx, and bits 4 - 7 indicate the maximal number of spatial streams
 * for Tx.
 *
 * @rx_tx_mcs9_max_nss: indicates the maximum number of spatial streams
 *     supported for reception and the maximum number of spatial streams
 *     supported for transmission for MCS 0 - 9.
 * @rx_tx_mcs11_max_nss: indicates the maximum number of spatial streams
 *     supported for reception and the maximum number of spatial streams
 *     supported for transmission for MCS 10 - 11.
 * @rx_tx_mcs13_max_nss: indicates the maximum number of spatial streams
 *     supported for reception and the maximum number of spatial streams
 *     supported for transmission for MCS 12 - 13.
 * @rx_tx_max_nss: array of the previous fields for easier loop access
 */
struct ieee80211_eht_mcs_nss_supp_bw {
	union {
		struct {
			u8 rx_tx_mcs9_max_nss;
			u8 rx_tx_mcs11_max_nss;
			u8 rx_tx_mcs13_max_nss;
		};
		u8 rx_tx_max_nss[3];
	};
};

/*
 * struct ieee80211_eht_mcs_nss_supp - EHT max supported NSS per MCS
 *
 * See P802.11be_D1.3 Table 9-401k - "Subfields of the Supported EHT-MCS
 * and NSS Set field"
 *
 * @only_20mhz: MCS/NSS support for 20 MHz-only STA.
 * @bw: MCS/NSS support for 80, 160 and 320 MHz
 * @bw._80: MCS/NSS support for BW <= 80 MHz
 * @bw._160: MCS/NSS support for BW = 160 MHz
 * @bw._320: MCS/NSS support for BW = 320 MHz
 */
struct ieee80211_eht_mcs_nss_supp {
	union {
		struct ieee80211_eht_mcs_nss_supp_20mhz_only only_20mhz;
		struct {
			struct ieee80211_eht_mcs_nss_supp_bw _80;
			struct ieee80211_eht_mcs_nss_supp_bw _160;
			struct ieee80211_eht_mcs_nss_supp_bw _320;
		} __packed bw;
	} __packed;
} __packed;

/*
 * struct ieee80211_sta_eht_cap - STA's EHT capabilities
 *
 * This structure describes most essential parameters needed
 * to describe 802.11be EHT capabilities for a STA.
 *
 * @has_eht: true iff EHT data is valid.
 * @eht_cap_elem: Fixed portion of the eht capabilities element.
 * @eht_mcs_nss_supp: The supported NSS/MCS combinations.
 * @eht_ppe_thres: Holds the PPE Thresholds data.
 */
struct ieee80211_sta_eht_cap {
	bool has_eht;
	struct ieee80211_eht_cap_elem_fixed eht_cap_elem;
	struct ieee80211_eht_mcs_nss_supp eht_mcs_nss_supp;
	u8 eht_ppe_thres[IEEE80211_EHT_PPE_THRES_MAX_LEN];
};

/*
 * struct ieee80211_sband_iftype_data - sband data per interface type
 *
 * This structure encapsulates sband data that is relevant for the
 * interface types defined in @types_mask.  Each type in the
 * @types_mask must be unique across all instances of iftype_data.
 *
 * @types_mask: interface types mask
 * @he_cap: holds the HE capabilities
 * @he_6ghz_capa: HE 6 GHz capabilities, must be filled in for a
 *	6 GHz band channel (and 0 may be valid value).
 * @eht_cap: STA's EHT capabilities
 * @vendor_elems: vendor element(s) to advertise
 * @vendor_elems.data: vendor element(s) data
 * @vendor_elems.len: vendor element(s) length
 */
struct ieee80211_sband_iftype_data {
	u16 types_mask;
	struct ieee80211_sta_he_cap he_cap;
	struct ieee80211_he_6ghz_capa he_6ghz_capa;
	struct ieee80211_sta_eht_cap eht_cap;
	struct {
		const u8 *data;
		unsigned int len;
	} vendor_elems;
};

/*
 * struct ieee80211_supported_band - frequency band definition
 *
 * This structure describes a frequency band a wiphy
 * is able to operate in.
 *
 * @channels: Array of channels the hardware can operate with
 *	in this band.
 * @band: the band this structure represents
 * @n_channels: Number of channels in @channels
 * @bitrates: Array of bitrates the hardware can operate with
 *	in this band. Must be sorted to give a valid "supported
 *	rates" IE, i.e. CCK rates first, then OFDM.
 * @n_bitrates: Number of bitrates in @bitrates
 * @ht_cap: HT capabilities in this band
 * @vht_cap: VHT capabilities in this band
 * @s1g_cap: S1G capabilities in this band
 * @edmg_cap: EDMG capabilities in this band
 * @s1g_cap: S1G capabilities in this band (S1B band only, of course)
 * @n_iftype_data: number of iftype data entries
 * @iftype_data: interface type data entries.  Note that the bits in
 *	@types_mask inside this structure cannot overlap (i.e. only
 *	one occurrence of each type is allowed across all instances of
 *	iftype_data).
 */
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

/*
 * struct cfg80211_pkt_pattern - packet pattern
 * @mask: bitmask where to match pattern and where to ignore bytes,
 *	one bit per byte, in same format as nl80211
 * @pattern: bytes to match where bitmask is 1
 * @pattern_len: length of pattern (in bytes)
 * @pkt_offset: packet offset (in bytes)
 *
 * Internal note: @mask and @pattern are allocated in one chunk of
 * memory, free @mask only!
 */
struct cfg80211_pkt_pattern {
	const u8 *mask;
	const u8 *pattern;
	int pattern_len;
	int pkt_offset;
};

/*
 * struct cfg80211_ssid - SSID description
 * @ssid: the SSID
 * @ssid_len: length of the ssid
 */
struct cfg80211_ssid {
        u8 ssid[IEEE80211_MAX_SSID_LEN];
        u8 ssid_len;
};

/*
 * struct cfg80211_wowlan_nd_match - information about the match
 *
 * @ssid: SSID of the match that triggered the wake up
 * @n_channels: Number of channels where the match occurred.  This
 *	value may be zero if the driver can't report the channels.
 * @channels: center frequencies of the channels where a match
 *	occurred (in MHz)
 */
struct cfg80211_wowlan_nd_match {
	struct cfg80211_ssid ssid;
	int n_channels;
	u32 channels[];
};

/*
 * struct cfg80211_wowlan_nd_info - net detect wake up information
 *
 * @n_matches: Number of match information instances provided in
 *	@matches.  This value may be zero if the driver can't provide
 *	match information.
 * @matches: Array of pointers to matches containing information about
 *	the matches that triggered the wake up.
 */
struct cfg80211_wowlan_nd_info {
	int n_matches;
	struct cfg80211_wowlan_nd_match *matches[];
};

/*
 * struct cfg80211_wowlan_wakeup - wakeup report
 * @disconnect: woke up by getting disconnected
 * @magic_pkt: woke up by receiving magic packet
 * @gtk_rekey_failure: woke up by GTK rekey failure
 * @eap_identity_req: woke up by EAP identity request packet
 * @four_way_handshake: woke up by 4-way handshake
 * @rfkill_release: woke up by rfkill being released
 * @pattern_idx: pattern that caused wakeup, -1 if not due to pattern
 * @packet_present_len: copied wakeup packet data
 * @packet_len: original wakeup packet length
 * @packet: The packet causing the wakeup, if any.
 * @packet_80211:  For pattern match, magic packet and other data
 *	frame triggers an 802.3 frame should be reported, for
 *	disconnect due to deauth 802.11 frame. This indicates which
 *	it is.
 * @tcp_match: TCP wakeup packet received
 * @tcp_connlost: TCP connection lost or failed to establish
 * @tcp_nomoretokens: TCP data ran out of tokens
 * @net_detect: if not %NULL, woke up because of net detect
 */
struct cfg80211_wowlan_wakeup {
	bool disconnect;
	bool magic_pkt;
	bool gtk_rekey_failure;
	bool eap_identity_req;
	bool four_way_handshake;
	bool rfkill_release;
	bool packet_80211;
	bool tcp_match;
	bool tcp_connlost;
	bool tcp_nomoretokens;
	s32 pattern_idx;
	u32 packet_present_len;
	u32 packet_len;
	const void *packet;
	struct cfg80211_wowlan_nd_info *net_detect;
};

/*
 * struct cfg80211_gtk_rekey_data - rekey data
 * @kek: key encryption key (@kek_len bytes)
 * @kck: key confirmation key (@kck_len bytes)
 * @replay_ctr: replay counter (NL80211_REPLAY_CTR_LEN bytes)
 * @kek_len: length of kek
 * @kck_len: length of kck
 * @akm: akm (oui, id)
 */
struct cfg80211_gtk_rekey_data {
	const u8 *kek;
	const u8 *kck;
	const u8 *replay_ctr;
	u32 akm;
	u8 kek_len;
	u8 kck_len;
};

/*
 * struct cfg80211_wowlan_tcp - TCP connection parameters
 *
 * @sock: (internal) socket for source port allocation
 * @src: source IP address
 * @dst: destination IP address
 * @dst_mac: destination MAC address
 * @src_port: source port
 * @dst_port: destination port
 * @payload_len: data payload length
 * @payload: data payload buffer
 * @payload_seq: payload sequence stamping configuration
 * @data_interval: interval at which to send data packets
 * @wake_len: wakeup payload match length
 * @wake_data: wakeup payload match data
 * @wake_mask: wakeup payload match mask
 * @tokens_size: length of the tokens buffer
 * @payload_tok: payload token usage configuration
 */
struct cfg80211_wowlan_tcp {
	struct socket *sock;
	__be32 src;
	__be32 dst;
	u16 src_port;
	u16 dst_port;
	u8 dst_mac[ETH_ALEN];
	int payload_len;
	const u8 *payload;
	struct nl80211_wowlan_tcp_data_seq payload_seq;
	u32 data_interval;
	u32 wake_len;
	const u8 *wake_data;
	const u8 *wake_mask;
	u32 tokens_size;
	/* must be last, variable member */
	struct nl80211_wowlan_tcp_data_token payload_tok;
};

/*
 * struct cfg80211_wowlan - Wake on Wireless-LAN support info
 *
 * This structure defines the enabled WoWLAN triggers for the device.
 * @any: wake up on any activity -- special trigger if device continues
 *	operating as normal during suspend
 * @disconnect: wake up if getting disconnected
 * @magic_pkt: wake up on receiving magic packet
 * @patterns: wake up on receiving packet matching a pattern
 * @n_patterns: number of patterns
 * @gtk_rekey_failure: wake up on GTK rekey failure
 * @eap_identity_req: wake up on EAP identity request packet
 * @four_way_handshake: wake up on 4-way handshake
 * @rfkill_release: wake up when rfkill is released
 * @tcp: TCP connection establishment/wakeup parameters, see nl80211.h.
 *	NULL if not configured.
 * @nd_config: configuration for the scan to be used for net detect wake.
 */
struct cfg80211_wowlan {
	bool any;
	bool disconnect;
	bool magic_pkt;
	bool gtk_rekey_failure;
	bool eap_identity_req;
	bool four_way_handshake;
	bool rfkill_release;
	struct cfg80211_pkt_pattern *patterns;
	struct cfg80211_wowlan_tcp *tcp;
	int n_patterns;
	struct cfg80211_sched_scan_request *nd_config;
};

/*
 * struct ieee80211_tx_control - TX control data
 *
 * @sta: station table entry, this sta pointer may be NULL and
 * 	it is not allowed to copy the pointer, due to RCU.
 */
struct ieee80211_tx_control {
	struct ieee80211_sta *sta;
};

struct ieee80211_ops {
	void (*tx)(struct ieee80211_hw *hw,
		   struct ieee80211_tx_control *control,
		   struct sk_buff *skb);
	int (*start)(struct ieee80211_hw *hw);
	void (*stop)(struct ieee80211_hw *hw, bool suspend);
	#ifdef CONFIG_PM
	int (*suspend)(struct ieee80211_hw *hw, struct cfg80211_wowlan *wowlan);
	int (*resume)(struct ieee80211_hw *hw);
	void (*set_wakeup)(struct ieee80211_hw *hw, bool enabled);
	#endif
	int (*add_interface)(struct ieee80211_hw *hw,
			     struct ieee80211_vif *vif);
	int (*change_interface)(struct ieee80211_hw *hw,
				struct ieee80211_vif *vif,
				enum nl80211_iftype new_type, bool p2p);
	void (*remove_interface)(struct ieee80211_hw *hw,
				 struct ieee80211_vif *vif);
	int (*config)(struct ieee80211_hw *hw, int radio_idx, u32 changed);
	void (*bss_info_changed)(struct ieee80211_hw *hw,
				 struct ieee80211_vif *vif,
				 struct ieee80211_bss_conf *info,
				 u64 changed);
	/* after 'bss_info_changed' the function pointers changes across Linux versions, for now use a generic array of pointers for indicating them... */
	void *backup_cb_ptr[IEEE80211_CB_PTR];
};

/*
 * struct ieee80211_conf - configuration of the device
 *
 * This struct indicates how the driver shall configure the hardware.
 *
 * @flags: configuration flags defined above
 *
 * @listen_interval: listen interval in units of beacon interval
 * @ps_dtim_period: The DTIM period of the AP we're connected to, for use
 *	in power saving. Power saving will not be enabled until a beacon
 *	has been received and the DTIM period is known.
 * @dynamic_ps_timeout: The dynamic powersave timeout (in ms), see the
 *	powersave documentation below. This variable is valid only when
 *	the CONF_PS flag is set.
 *
 * @power_level: requested transmit power (in dBm), backward compatibility
 *	value only that is set to the minimum of all interfaces
 *
 * @chandef: the channel definition to tune to
 * @radar_enabled: whether radar detection is enabled
 *
 * @long_frame_max_tx_count: Maximum number of transmissions for a "long" frame
 *	(a frame not RTS protected), called "dot11LongRetryLimit" in 802.11,
 *	but actually means the number of transmissions not the number of retries
 * @short_frame_max_tx_count: Maximum number of transmissions for a "short"
 *	frame, called "dot11ShortRetryLimit" in 802.11, but actually means the
 *	number of transmissions not the number of retries
 *
 * @smps_mode: spatial multiplexing powersave mode; note that
 *	%IEEE80211_SMPS_STATIC is used when the device is not
 *	configured for an HT channel.
 *	Note that this is only valid if channel contexts are not used,
 *	otherwise each channel context has the number of chains listed.
 */
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

struct rate_control_ops {
	unsigned long capa;
	const char *name;
	void *(*alloc)(struct ieee80211_hw *hw);
	void (*add_debugfs)(struct ieee80211_hw *hw, void *priv, void *debugfsdir);
	void (*free)(void *priv);

	void *(*alloc_sta)(void *priv, struct ieee80211_sta *sta, int gfp);
	void (*rate_init)(void *priv, struct ieee80211_supported_band *sband,
			  struct cfg80211_chan_def *chandef, struct ieee80211_sta *sta, void *priv_sta);
	void (*rate_update)(void *priv, struct ieee80211_supported_band *sband,
			    struct cfg80211_chan_def *chandef, struct ieee80211_sta *sta, void *priv_sta, u32 changed);
	void (*free_sta)(void *priv, struct ieee80211_sta *sta, void *priv_sta);

	void (*tx_status_ext)(void *priv, struct ieee80211_supported_band *sband, void *priv_sta, struct ieee80211_tx_status *st);
	void (*tx_status)(void *priv, struct ieee80211_supported_band *sband, struct ieee80211_sta *sta, void *priv_sta, struct sk_buff *skb);
	void (*get_rate)(void *priv, struct ieee80211_sta *sta, void *priv_sta, struct ieee80211_tx_rate_control *txrc);

	void (*add_sta_debugfs)(void *priv, void *priv_sta, void *dir);
	u32 (*get_expected_throughput)(void *priv_sta);
};

struct rate_control_ref {
	const struct rate_control_ops *rops;
	void *priv;
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
	atomic_uint aql_total_pending_airtime;
	atomic_uint aql_ac_pending_airtime[IEEE80211_NUM_ACS];
	atomic_uint agg_queue_stop[IEEE80211_MAX_QUEUES];
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

/* Generic userspace 802.11 authentication, association, and WPA2-PSK FSM. */

struct connection_fsm {
	enum connection_state state;
	bool protected_network;
	bool ptk_installed;
	bool gtk_installed;
	bool retry_worker_started;
	u8 auth_attempts;
	u8 assoc_attempts;
	u8 descriptor_version;
	u8 ssid[IEEE80211_MAX_SSID_LEN];
	u8 ssid_len;
	char passphrase[64];
	u8 passphrase_len;
	u8 bssid[ETH_ALEN];
	u32 center_freq;
	u16 capability;
	u16 aid;
	u16 tx_sequence;
	u64 replay_counter;
	u64 m1_replay_counter;
	u64 m3_replay_counter;
	u8 anonce[32];
	u8 snonce[32];
	u8 pmk[32];
	u8 ptk[64];
	u8 gtk[32];
	u8 gtk_len;
	u8 gtk_key_idx;
	u8 m2[192];
	size_t m2_len;
	u8 m4[128];
	size_t m4_len;
	struct ieee80211_sta *sta;
	struct sta_info *sta_info;
	struct link_sta_info *link_sta_info;
	struct ieee80211_chanctx_conf *chanctx;
	struct ieee80211_key_conf *ptk_key;
	struct ieee80211_key_conf *gtk_key;
};

struct connection_retry_job {
	struct ieee80211_local *local;
	struct connection_fsm *fsm;
};

static const u8 connection_rsn_ie[] = {
	0x30, 0x14, 0x01, 0x00, 0x00, 0x0f, 0xac, 0x04,
	0x01, 0x00, 0x00, 0x0f, 0xac, 0x04, 0x01, 0x00,
	0x00, 0x0f, 0xac, 0x02, 0x00, 0x00
};

const struct element * cfg80211_find_elem_match(u8 eid, const u8 *ies, unsigned int len,  const u8 *match, unsigned int match_len, unsigned int match_offset);

/*
 * ieee80211_is_mgmt - check if type is IEEE80211_FTYPE_MGMT
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame type is management
 */
static inline bool ieee80211_is_mgmt(__le16 fc){
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_MGMT);
}

/*
 * ieee80211_is_ctl - check if type is IEEE80211_FTYPE_CTL
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame type is control
 */
static inline bool ieee80211_is_ctl(__le16 fc){
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_CTL);
}

/*
 * ieee80211_is_data - check if type is IEEE80211_FTYPE_DATA
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame is a data frame
 */
static inline bool ieee80211_is_data(__le16 fc){
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_DATA);
}

/*
 * ieee80211_is_ext - check if type is IEEE80211_FTYPE_EXT
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame type is extended
 */
static inline bool ieee80211_is_ext(__le16 fc){
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_EXT);
}


/*
 * ieee80211_is_data_qos - check if type is IEEE80211_FTYPE_DATA and IEEE80211_STYPE_QOS_DATA is set
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame is a QoS data frame
 */
static inline bool ieee80211_is_data_qos(__le16 fc){
	/*
	 * mask with QOS_DATA rather than IEEE80211_FCTL_STYPE as we just need
	 * to check the one bit
	 */
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_STYPE_QOS_DATA)) ==
	       cpu_to_le16(IEEE80211_FTYPE_DATA | IEEE80211_STYPE_QOS_DATA);
}

/*
 * ieee80211_is_data_present - check if type is IEEE80211_FTYPE_DATA and has data
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame is a QoS data frame that has data
 *	(i.e. is not null data)
 */
static inline bool ieee80211_is_data_present(__le16 fc){
	/*
	 * mask with 0x40 and test that that bit is clear to only return true
	 * for the data-containing substypes.
	 */
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | 0x40)) ==
	       cpu_to_le16(IEEE80211_FTYPE_DATA);
}

/*
 * ieee80211_is_assoc_req - check if IEEE80211_FTYPE_MGMT && IEEE80211_STYPE_ASSOC_REQ
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame is an association request
 */
static inline bool ieee80211_is_assoc_req(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_ASSOC_REQ);
}

/*
 * ieee80211_is_assoc_resp - check if IEEE80211_FTYPE_MGMT && IEEE80211_STYPE_ASSOC_RESP
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame is an association response
 */
static inline bool ieee80211_is_assoc_resp(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_ASSOC_RESP);
}

/*
 * ieee80211_is_reassoc_req - check if IEEE80211_FTYPE_MGMT && IEEE80211_STYPE_REASSOC_REQ
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame is a reassociation request
 */
static inline bool ieee80211_is_reassoc_req(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_REASSOC_REQ);
}

/*
 * ieee80211_is_reassoc_resp - check if IEEE80211_FTYPE_MGMT && IEEE80211_STYPE_REASSOC_RESP
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame is a reassociation response
 */
static inline bool ieee80211_is_reassoc_resp(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_REASSOC_RESP);
}

/*
 * ieee80211_is_probe_req - check if IEEE80211_FTYPE_MGMT && IEEE80211_STYPE_PROBE_REQ
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame is a probe request
 */
static inline bool ieee80211_is_probe_req(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_PROBE_REQ);
}

/*
 * ieee80211_is_probe_resp - check if IEEE80211_FTYPE_MGMT && IEEE80211_STYPE_PROBE_RESP
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame is a probe response
 */
static inline bool ieee80211_is_probe_resp(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_PROBE_RESP);
}

/*
 * ieee80211_is_beacon - check if IEEE80211_FTYPE_MGMT && IEEE80211_STYPE_BEACON
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame is a (regular, not S1G) beacon
 */
static inline bool ieee80211_is_beacon(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_BEACON);
}

/*
 * ieee80211_is_s1g_beacon - check if IEEE80211_FTYPE_EXT &&
 * IEEE80211_STYPE_S1G_BEACON
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame is an S1G beacon
 */
static inline bool ieee80211_is_s1g_beacon(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE |
				 IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_EXT | IEEE80211_STYPE_S1G_BEACON);
}

static u32 cfg80211_calculate_bitrate_ht(struct rate_info *rate){
	int modulation = 0;
	int streams = 0;
	int bitrate = 0;

	/* the formula below does only work for MCS values smaller than 32 */
	if (rate->mcs >= 32){
		return 0;
	}

	modulation = rate->mcs & 7;
	streams = (rate->mcs >> 3) + 1;

	bitrate = (rate->bw == RATE_INFO_BW_40) ? 13500000 : 6500000;

	if (modulation < 4){
		bitrate *= (modulation + 1);
	}else if (modulation == 4){
		bitrate *= (modulation + 2);
	}else{
		bitrate *= (modulation + 3);
	}

	bitrate *= streams;

	if (rate->flags & RATE_INFO_FLAGS_SHORT_GI){
		bitrate = (bitrate / 9) * 10;
	}

	/* do NOT round down here */
	return (bitrate + 50000) / 100000;
}

static u32 cfg80211_calculate_bitrate_dmg(struct rate_info *rate){
	static const u32 __mcs2bitrate[] = {
		/* control PHY */
		[0] =   275,
		/* SC PHY */
		[1] =  3850,
		[2] =  7700,
		[3] =  9625,
		[4] = 11550,
		[5] = 12512, /* 1251.25 mbps */
		[6] = 15400,
		[7] = 19250,
		[8] = 23100,
		[9] = 25025,
		[10] = 30800,
		[11] = 38500,
		[12] = 46200,
		/* OFDM PHY */
		[13] =  6930,
		[14] =  8662, /* 866.25 mbps */
		[15] = 13860,
		[16] = 17325,
		[17] = 20790,
		[18] = 27720,
		[19] = 34650,
		[20] = 41580,
		[21] = 45045,
		[22] = 51975,
		[23] = 62370,
		[24] = 67568, /* 6756.75 mbps */
		/* LP-SC PHY */
		[25] =  6260,
		[26] =  8340,
		[27] = 11120,
		[28] = 12510,
		[29] = 16680,
		[30] = 22240,
		[31] = 25030,
	};

	if (rate->mcs >= ARRAY_SIZE(__mcs2bitrate)){
		return 0;
	}

	return __mcs2bitrate[rate->mcs];
}

static u32 cfg80211_calculate_bitrate_extended_sc_dmg(struct rate_info *rate){
	int ret = 0;
	static const u32 __mcs2bitrate[] = {
		[6 - 6] = 26950, /* MCS 9.1 : 2695.0 mbps */
		[7 - 6] = 50050, /* MCS 12.1 */
		[8 - 6] = 53900,
		[9 - 6] = 57750,
		[10 - 6] = 63900,
		[11 - 6] = 75075,
		[12 - 6] = 80850,
	};

	/* Extended SC MCS not defined for base MCS below 6 or above 12 */
	if (rate->mcs < 6 || rate->mcs > 12){
		return ret;
	}

	return __mcs2bitrate[rate->mcs - 6];
}

static u32 cfg80211_calculate_bitrate_edmg(struct rate_info *rate){
	int ret = 0;
	static const u32 __mcs2bitrate[] = {
		/* control PHY */
		[0] =   275,
		/* SC PHY */
		[1] =  3850,
		[2] =  7700,
		[3] =  9625,
		[4] = 11550,
		[5] = 12512, /* 1251.25 mbps */
		[6] = 13475,
		[7] = 15400,
		[8] = 19250,
		[9] = 23100,
		[10] = 25025,
		[11] = 26950,
		[12] = 30800,
		[13] = 38500,
		[14] = 46200,
		[15] = 50050,
		[16] = 53900,
		[17] = 57750,
		[18] = 69300,
		[19] = 75075,
		[20] = 80850,
	};

	if (rate->mcs >= ARRAY_SIZE(__mcs2bitrate)){
		return ret;
	}

	return __mcs2bitrate[rate->mcs] * rate->n_bonded_ch;
}

static u32 cfg80211_calculate_bitrate_vht(struct rate_info *rate){
	int ret = 0;
	int idx = 0;
	u32 bitrate = 0;
	static const u32 base[4][12] = {
		{   6500000,
		   13000000,
		   19500000,
		   26000000,
		   39000000,
		   52000000,
		   58500000,
		   65000000,
		   78000000,
		/* not in the spec, but some devices use this: */
		   86700000,
		   97500000,
		  108300000,
		},
		{  13500000,
		   27000000,
		   40500000,
		   54000000,
		   81000000,
		  108000000,
		  121500000,
		  135000000,
		  162000000,
		  180000000,
		  202500000,
		  225000000,
		},
		{  29300000,
		   58500000,
		   87800000,
		  117000000,
		  175500000,
		  234000000,
		  263300000,
		  292500000,
		  351000000,
		  390000000,
		  438800000,
		  487500000,
		},
		{  58500000,
		  117000000,
		  175500000,
		  234000000,
		  351000000,
		  468000000,
		  526500000,
		  585000000,
		  702000000,
		  780000000,
		  877500000,
		  975000000,
		},
	};

	if (rate->mcs > 11){
		goto warn;
	}

	switch (rate->bw) {
	case RATE_INFO_BW_160:
		idx = 3;
		break;
	case RATE_INFO_BW_80:
		idx = 2;
		break;
	case RATE_INFO_BW_40:
		idx = 1;
		break;
	case RATE_INFO_BW_5:
	case RATE_INFO_BW_10:
	default:
		goto warn;
	case RATE_INFO_BW_20:
		idx = 0;
	}

	bitrate = base[idx][rate->mcs];
	bitrate *= rate->nss;

	if (rate->flags & RATE_INFO_FLAGS_SHORT_GI){
		bitrate = (bitrate / 9) * 10;
	}

	/* do NOT round down here */
	return (bitrate + 50000) / 100000;
	warn:
	#ifdef ENABLE_DBG_PRINT
	printf("[!] invalid rate bw=%d, mcs=%d, nss=%d\n", rate->bw, rate->mcs, rate->nss);
	#endif
	return ret;
}

static u32 cfg80211_calculate_bitrate_he(struct rate_info *rate){
	int ret = 0;

	#define SCALE 6144
	u32 mcs_divisors[14] = {
		102399, /* 16.666666... */
		 51201, /*  8.333333... */
		 34134, /*  5.555555... */
		 25599, /*  4.166666... */
		 17067, /*  2.777777... */
		 12801, /*  2.083333... */
		 11377, /*  1.851725... */
		 10239, /*  1.666666... */
		  8532, /*  1.388888... */
		  7680, /*  1.250000... */
		  6828, /*  1.111111... */
		  6144, /*  1.000000... */
		  5690, /*  0.926106... */
		  5120, /*  0.833333... */
	};
	u32 rates_160M[3] = { 960777777, 907400000, 816666666 };
	u32 rates_996[3] =  { 480388888, 453700000, 408333333 };
	u32 rates_484[3] =  { 229411111, 216666666, 195000000 };
	u32 rates_242[3] =  { 114711111, 108333333,  97500000 };
	u32 rates_106[3] =  {  40000000,  37777777,  34000000 };
	u32 rates_52[3]  =  {  18820000,  17777777,  16000000 };
	u32 rates_26[3]  =  {   9411111,   8888888,   8000000 };
	u64 tmp = 0;
	u32 result = 0;

	if (rate->mcs > 13){
		return ret;
	}

	if (rate->he_gi > NL80211_RATE_INFO_HE_GI_3_2){
		return ret;
	}

	if (rate->he_ru_alloc > NL80211_RATE_INFO_HE_RU_ALLOC_2x996){
		return ret;
	}

	if (rate->nss < 1 || rate->nss > 8){
		return ret;
	}

	if (rate->bw == RATE_INFO_BW_160 || (rate->bw == RATE_INFO_BW_HE_RU && rate->he_ru_alloc == NL80211_RATE_INFO_HE_RU_ALLOC_2x996)){
		result = rates_160M[rate->he_gi];
	}else if (rate->bw == RATE_INFO_BW_80 || (rate->bw == RATE_INFO_BW_HE_RU && rate->he_ru_alloc == NL80211_RATE_INFO_HE_RU_ALLOC_996)){
		result = rates_996[rate->he_gi];
	}else if (rate->bw == RATE_INFO_BW_40 || (rate->bw == RATE_INFO_BW_HE_RU && rate->he_ru_alloc == NL80211_RATE_INFO_HE_RU_ALLOC_484)){
		result = rates_484[rate->he_gi];
	}else if (rate->bw == RATE_INFO_BW_20 || (rate->bw == RATE_INFO_BW_HE_RU &&  rate->he_ru_alloc == NL80211_RATE_INFO_HE_RU_ALLOC_242)){
		result = rates_242[rate->he_gi];
	}else if (rate->bw == RATE_INFO_BW_HE_RU && rate->he_ru_alloc == NL80211_RATE_INFO_HE_RU_ALLOC_106){
		result = rates_106[rate->he_gi];
	}else if (rate->bw == RATE_INFO_BW_HE_RU && rate->he_ru_alloc == NL80211_RATE_INFO_HE_RU_ALLOC_52){
		result = rates_52[rate->he_gi];
	}else if (rate->bw == RATE_INFO_BW_HE_RU &&  rate->he_ru_alloc == NL80211_RATE_INFO_HE_RU_ALLOC_26){
		result = rates_26[rate->he_gi];
	}else{
		#ifdef ENABLE_DBG_PRINT
		printf("[!] invalid HE MCS: bw:%d, ru:%d\n", rate->bw, rate->he_ru_alloc);
		#endif
		return ret;
	}

	/* now scale to the appropriate MCS */
	tmp = result;
	tmp *= SCALE;
	do_div(tmp, mcs_divisors[rate->mcs]);

	/* and take NSS, DCM into account */
	tmp *= rate->nss;
	do_div(tmp, 8);
	if (rate->he_dcm){
		do_div(tmp, 2);
	}

	result = tmp;
	return result / 10000;
}

static u32 cfg80211_calculate_bitrate_eht(struct rate_info *rate){
	int ret = 0;

	#define SCALE 6144
	static const u32 mcs_divisors[16] = {
		102399, /* 16.666666... */
		 51201, /*  8.333333... */
		 34134, /*  5.555555... */
		 25599, /*  4.166666... */
		 17067, /*  2.777777... */
		 12801, /*  2.083333... */
		 11377, /*  1.851725... */
		 10239, /*  1.666666... */
		  8532, /*  1.388888... */
		  7680, /*  1.250000... */
		  6828, /*  1.111111... */
		  6144, /*  1.000000... */
		  5690, /*  0.926106... */
		  5120, /*  0.833333... */
		409600, /* 66.666666... */
		204800, /* 33.333333... */
	};
	static const u32 rates_996[3] =  { 480388888, 453700000, 408333333 };
	static const u32 rates_484[3] =  { 229411111, 216666666, 195000000 };
	static const u32 rates_242[3] =  { 114711111, 108333333,  97500000 };
	static const u32 rates_106[3] =  {  40000000,  37777777,  34000000 };
	static const u32 rates_52[3]  =  {  18820000,  17777777,  16000000 };
	static const u32 rates_26[3]  =  {   9411111,   8888888,   8000000 };
	u64 tmp = 0;
	u32 result = 0;

	if (rate->mcs > 15){
		return ret;
	}

	if (rate->eht_gi > NL80211_RATE_INFO_EHT_GI_3_2){
		return ret;
	}

	if (rate->eht_ru_alloc > NL80211_RATE_INFO_EHT_RU_ALLOC_4x996){
		return ret;
	}

	if (rate->nss < 1 || rate->nss > 8){
		return ret;
	}

	/* Bandwidth checks for MCS 14 */
	if (rate->mcs == 14) {
		if ((rate->bw != RATE_INFO_BW_EHT_RU &&
		     rate->bw != RATE_INFO_BW_80 &&
		     rate->bw != RATE_INFO_BW_160 &&
		     rate->bw != RATE_INFO_BW_320) ||
		    (rate->bw == RATE_INFO_BW_EHT_RU &&
		     rate->eht_ru_alloc != NL80211_RATE_INFO_EHT_RU_ALLOC_996 &&
		     rate->eht_ru_alloc != NL80211_RATE_INFO_EHT_RU_ALLOC_2x996 &&
		     rate->eht_ru_alloc != NL80211_RATE_INFO_EHT_RU_ALLOC_4x996)) {
			#ifdef ENABLE_DBG_PRINT
			printf("[!] invalid EHT BW for MCS 14: bw:%d, ru:%d\n", rate->bw, rate->eht_ru_alloc);
			#endif
			return ret;
		}
	}

	if (rate->bw == RATE_INFO_BW_320 || (rate->bw == RATE_INFO_BW_EHT_RU && rate->eht_ru_alloc == NL80211_RATE_INFO_EHT_RU_ALLOC_4x996))
		result = 4 * rates_996[rate->eht_gi];
	else if (rate->bw == RATE_INFO_BW_EHT_RU && rate->eht_ru_alloc == NL80211_RATE_INFO_EHT_RU_ALLOC_3x996P484)
		result = 3 * rates_996[rate->eht_gi] + rates_484[rate->eht_gi];
	else if (rate->bw == RATE_INFO_BW_EHT_RU && rate->eht_ru_alloc == NL80211_RATE_INFO_EHT_RU_ALLOC_3x996)
		result = 3 * rates_996[rate->eht_gi];
	else if (rate->bw == RATE_INFO_BW_EHT_RU && rate->eht_ru_alloc == NL80211_RATE_INFO_EHT_RU_ALLOC_2x996P484)
		result = 2 * rates_996[rate->eht_gi] + rates_484[rate->eht_gi];
	else if (rate->bw == RATE_INFO_BW_160 || (rate->bw == RATE_INFO_BW_EHT_RU && rate->eht_ru_alloc == NL80211_RATE_INFO_EHT_RU_ALLOC_2x996))
		result = 2 * rates_996[rate->eht_gi];
	else if (rate->bw == RATE_INFO_BW_EHT_RU && rate->eht_ru_alloc == NL80211_RATE_INFO_EHT_RU_ALLOC_996P484P242)
		result = rates_996[rate->eht_gi] + rates_484[rate->eht_gi]
			 + rates_242[rate->eht_gi];
	else if (rate->bw == RATE_INFO_BW_EHT_RU && rate->eht_ru_alloc == NL80211_RATE_INFO_EHT_RU_ALLOC_996P484)
		result = rates_996[rate->eht_gi] + rates_484[rate->eht_gi];
	else if (rate->bw == RATE_INFO_BW_80 ||(rate->bw == RATE_INFO_BW_EHT_RU && rate->eht_ru_alloc == NL80211_RATE_INFO_EHT_RU_ALLOC_996))
		result = rates_996[rate->eht_gi];
	else if (rate->bw == RATE_INFO_BW_EHT_RU && rate->eht_ru_alloc == NL80211_RATE_INFO_EHT_RU_ALLOC_484P242)
		result = rates_484[rate->eht_gi] + rates_242[rate->eht_gi];
	else if (rate->bw == RATE_INFO_BW_40 || (rate->bw == RATE_INFO_BW_EHT_RU && rate->eht_ru_alloc == NL80211_RATE_INFO_EHT_RU_ALLOC_484))
		result = rates_484[rate->eht_gi];
	else if (rate->bw == RATE_INFO_BW_20 || (rate->bw == RATE_INFO_BW_EHT_RU && rate->eht_ru_alloc == NL80211_RATE_INFO_EHT_RU_ALLOC_242))
		result = rates_242[rate->eht_gi];
	else if (rate->bw == RATE_INFO_BW_EHT_RU && rate->eht_ru_alloc == NL80211_RATE_INFO_EHT_RU_ALLOC_106P26)
		result = rates_106[rate->eht_gi] + rates_26[rate->eht_gi];
	else if (rate->bw == RATE_INFO_BW_EHT_RU && rate->eht_ru_alloc == NL80211_RATE_INFO_EHT_RU_ALLOC_106)
		result = rates_106[rate->eht_gi];
	else if (rate->bw == RATE_INFO_BW_EHT_RU && rate->eht_ru_alloc == NL80211_RATE_INFO_EHT_RU_ALLOC_52P26)
		result = rates_52[rate->eht_gi] + rates_26[rate->eht_gi];
	else if (rate->bw == RATE_INFO_BW_EHT_RU && rate->eht_ru_alloc == NL80211_RATE_INFO_EHT_RU_ALLOC_52)
		result = rates_52[rate->eht_gi];
	else if (rate->bw == RATE_INFO_BW_EHT_RU && rate->eht_ru_alloc == NL80211_RATE_INFO_EHT_RU_ALLOC_26)
		result = rates_26[rate->eht_gi];
	else {
		#ifdef ENABLE_DBG_PRINT
		printf("[!] invalid EHT MCS: bw:%d, ru:%d\n", rate->bw, rate->eht_ru_alloc);
		#endif
		return ret;
	}

	/* now scale to the appropriate MCS */
	tmp = result;
	tmp *= SCALE;
	do_div(tmp, mcs_divisors[rate->mcs]);

	/* and take NSS */
	tmp *= rate->nss;
	do_div(tmp, 8);

	result = tmp;
	return result / 10000;
}

static u32 cfg80211_calculate_bitrate_s1g(struct rate_info *rate){
	/* For 1, 2, 4, 8 and 16 MHz channels */
	static const u32 base[5][11] = {
		{  300000,
		   600000,
		   900000,
		  1200000,
		  1800000,
		  2400000,
		  2700000,
		  3000000,
		  3600000,
		  4000000,
		  /* MCS 10 supported in 1 MHz only */
		  150000,
		},
		{  650000,
		  1300000,
		  1950000,
		  2600000,
		  3900000,
		  5200000,
		  5850000,
		  6500000,
		  7800000,
		  /* MCS 9 not valid */
		},
		{  1350000,
		   2700000,
		   4050000,
		   5400000,
		   8100000,
		  10800000,
		  12150000,
		  13500000,
		  16200000,
		  18000000,
		},
		{  2925000,
		   5850000,
		   8775000,
		  11700000,
		  17550000,
		  23400000,
		  26325000,
		  29250000,
		  35100000,
		  39000000,
		},
		{  8580000,
		  11700000,
		  17550000,
		  23400000,
		  35100000,
		  46800000,
		  52650000,
		  58500000,
		  70200000,
		  78000000,
		},
	};
	u32 bitrate;
	/* default is 1 MHz index */
	int idx = 0;

	if (rate->mcs >= 11){
		goto warn;
	}

	switch (rate->bw) {
	case RATE_INFO_BW_16:
		idx = 4;
		break;
	case RATE_INFO_BW_8:
		idx = 3;
		break;
	case RATE_INFO_BW_4:
		idx = 2;
		break;
	case RATE_INFO_BW_2:
		idx = 1;
		break;
	case RATE_INFO_BW_1:
		idx = 0;
		break;
	case RATE_INFO_BW_5:
	case RATE_INFO_BW_10:
	case RATE_INFO_BW_20:
	case RATE_INFO_BW_40:
	case RATE_INFO_BW_80:
	case RATE_INFO_BW_160:
	default:
		goto warn;
	}

	bitrate = base[idx][rate->mcs];
	bitrate *= rate->nss;

	if (rate->flags & RATE_INFO_FLAGS_SHORT_GI){
		bitrate = (bitrate / 9) * 10;
	}
	/* do NOT round down here */
	return (bitrate + 50000) / 100000;
	warn:
	#ifdef ENABLE_DBG_PRINT
	printf("[!] invalid rate bw=%d, mcs=%d, nss=%d\n", rate->bw, rate->mcs, rate->nss);
	#endif
	return 0;
}

static inline struct ieee80211_tx_info *IEEE80211_SKB_CB(struct sk_buff *skb){
	return (struct ieee80211_tx_info *)skb->cb;
}

static inline struct ieee80211_rx_status *IEEE80211_SKB_RXCB(struct sk_buff *skb){
	return (struct ieee80211_rx_status *)skb->cb;
}

static inline u32 ieee80211_rx_status_to_khz(struct ieee80211_rx_status *rx_status){
	return MHZ_TO_KHZ(rx_status->freq) + (rx_status->freq_offset ? 500 : 0);
}

static inline bool ieee80211_is_auth(__le16 fc);
static bool is_zero_ether_addr(const u8 *addr);

static inline struct ieee80211_vif *connection_get_vif(struct ieee80211_local *local){
	return local != NULL ? local->ieee80211_vif : NULL;
}

static inline struct connection_fsm *connection_get(void){
	return (struct connection_fsm *)director.connection_fsm;
}

static inline void connection_set_state(struct connection_fsm *fsm, enum connection_state state){
	if (fsm == NULL || fsm->state == state){
		goto end;
	}
	fsm->state = state;
	end:
}

static inline u16 connection_get_be16(const u8 *buf){
	return (u16)((buf[0] << 8) | buf[1]);
}

static inline u64 connection_get_be64(const u8 *buf){
	u64 value = 0;

	for (int i = 0; i < 8; i++){
		value = (value << 8) | buf[i];
	}
	return value;
}

static inline void connection_put_be16(u8 *buf, u16 value){
	buf[0] = (u8)(value >> 8);
	buf[1] = (u8)value;
}

static inline void connection_put_be64(u8 *buf, u64 value){
	for (int i = 7; i >= 0; i--){
		buf[i] = (u8)value;
		value >>= 8;
	}
}

static inline int connection_hmac_sha1(const u8 *key, int key_len, const u8 *data, int data_len, u8 out[20]){
	int ret = 0;
	unsigned int out_len = 0;

	#ifdef __linux__
	if (HMAC(EVP_sha1(), key, key_len, data, data_len, out, &out_len) == NULL || out_len != 20){
		ret = -EINVAL;
		return ret;
	}
	return ret;
	#else
	ret = -EOPNOTSUPP;
	return ret;
	#endif
}

static inline int connection_derive_ptk(struct connection_fsm *fsm, const u8 *station_addr){
	int ret = 0;
	int written = 0;
	int data_len = 0;
	int copy_len = 0;
	u8 data[128] = {};
	u8 digest[20] = {};
	const u8 *mac1 = fsm->bssid;
	const u8 *mac2 = station_addr;
	const u8 *nonce1 = fsm->anonce;
	const u8 *nonce2 = fsm->snonce;
	const u8 label[] = "Pairwise key expansion";
	#ifdef __linux__
	if (PKCS5_PBKDF2_HMAC(fsm->passphrase, fsm->passphrase_len, fsm->ssid, fsm->ssid_len, 4096, EVP_sha1(), sizeof(fsm->pmk), fsm->pmk) != 1){
		ret = -EINVAL;
		return ret;
	}
	if (memcmp(mac1, mac2, ETH_ALEN) > 0){
		mac1 = station_addr;
		mac2 = fsm->bssid;
	}
	if (memcmp(nonce1, nonce2, 32) > 0){
		nonce1 = fsm->snonce;
		nonce2 = fsm->anonce;
	}
	memcpy(data, label, sizeof(label));
	data_len = sizeof(label);
	memcpy(data + data_len, mac1, ETH_ALEN);
	data_len += ETH_ALEN;
	memcpy(data + data_len, mac2, ETH_ALEN);
	data_len += ETH_ALEN;
	memcpy(data + data_len, nonce1, 32);
	data_len += 32;
	memcpy(data + data_len, nonce2, 32);
	data_len += 32;

	for (u8 i = 0; written < sizeof(fsm->ptk); i++){
		copy_len = 0;

		data[data_len] = i;
		if (connection_hmac_sha1(fsm->pmk, sizeof(fsm->pmk), data, data_len + 1, digest)){
			ret = -EINVAL;
			return ret;
		}
		copy_len = min(sizeof(fsm->ptk) - written, sizeof(digest));
		memcpy(fsm->ptk + written, digest, copy_len);
		written += copy_len;
	}
	return ret;
	#else
	/* TODO: implement the support for Windows */
	ret = -EOPNOTSUPP;
	return ret;
	#endif
}

static inline int connection_aes_unwrap(const u8 kek[16], const u8 *wrapped, int wrapped_len, u8 *plain, int *plain_len){
	#ifdef __linux__
	int ret = 0;
	int update_len = 0;
	int final_len = 0;
	EVP_CIPHER_CTX *ctx = NULL;

	if (wrapped == NULL || plain == NULL || plain_len == NULL || wrapped_len < 16 || (wrapped_len & 7)){
		ret = -EINVAL;
		return ret;
	}
	ctx = EVP_CIPHER_CTX_new();
	if (ctx == NULL){
		ret = -ENOMEM;
		return ret;
	}
	EVP_CIPHER_CTX_set_flags(ctx, EVP_CIPHER_CTX_FLAG_WRAP_ALLOW);
	if (EVP_DecryptInit_ex(ctx, EVP_aes_128_wrap(), NULL, kek, NULL) == 1 &&
	    EVP_DecryptUpdate(ctx, plain, &update_len, wrapped, wrapped_len) == 1 &&
	    EVP_DecryptFinal_ex(ctx, plain + update_len, &final_len) == 1){
		*plain_len = (update_len + final_len);
		ret = 0;
	}
	EVP_CIPHER_CTX_free(ctx);
	return ret;
	#else
	int ret = 0;
	ret = -EOPNOTSUPP;
	return ret;
	#endif
}

static inline struct exsul_scan_result *connection_find_target(const char *ssid){
	int ssid_len = 0;
	struct exsul_scan_result *best = NULL;

	if (ssid == NULL){
		return NULL;
	}
	ssid_len = strlen(ssid);
	for (u32 i = 0; i < EXSUL_MAX_SCAN_RESULTS; i++){
		struct exsul_scan_result *entry = &director.scan_results[i];

		if (!entry->valid || entry->ssid_len != ssid_len || memcmp(entry->ssid, ssid, ssid_len) != 0){
			continue;
		}
		if (best == NULL || entry->signal > best->signal){
			best = entry;
		}
	}
	return best;
}

static inline struct ieee80211_channel *connection_find_channel(struct wiphy *wiphy, u32 center_freq){
	if (wiphy == NULL || center_freq == 0){
		return NULL;
	}
	return ieee80211_get_channel_khz(wiphy, MHZ_TO_KHZ(center_freq));
}

/*
 * ieee80211_channel_to_frequency - convert channel number to frequency
 * @chan: channel number
 * @band: band, necessary due to channel number overlap
 * Return: The corresponding frequency (in MHz), or 0 if the conversion failed.
 */
static inline int ieee80211_channel_to_frequency(int chan, enum nl80211_band band){
	int ret = 0;

	ret = KHZ_TO_MHZ(ieee80211_channel_to_freq_khz(chan, band));
	return ret;
}

/*
 * ieee80211_frequency_to_channel - convert frequency to channel number
 * @freq: center frequency in MHz
 * Return: The corresponding channel, or 0 if the conversion failed.
 */
static inline int ieee80211_frequency_to_channel(int freq){
	int ret = 0;

	ret = ieee80211_freq_khz_to_channel(MHZ_TO_KHZ(freq));
	return ret;
}

/*
 * ieee80211_get_channel - get channel struct from wiphy for specified frequency
 *
 * @wiphy: the struct wiphy to get the channel for
 * @freq: the center frequency (in MHz) of the channel
 * Return: The channel struct from @wiphy at @freq.
 */
static inline struct ieee80211_channel *ieee80211_get_channel(struct wiphy *wiphy, int freq){
	if(wiphy == NULL || freq == 0){
		return NULL;
	}
	return ieee80211_get_channel_khz(wiphy, MHZ_TO_KHZ(freq));
}

/*
 * cfg80211_channel_is_psc - Check if the channel is a 6 GHz PSC
 * @chan: control channel to check
 *
 * The Preferred Scanning Channels (PSC) are defined in
 * Draft IEEE P802.11ax/D5.0, 26.17.2.3.3
 *
 * Return: %true if channel is a PSC, %false otherwise
 */
static inline bool cfg80211_channel_is_psc(struct ieee80211_channel *chan){
	if(chan == NULL){
		return false;
	}
	if (chan->band != NL80211_BAND_6GHZ){
		return false;
	}
	return ieee80211_frequency_to_channel(chan->center_freq) % 16 == 5;
}

static inline struct ieee80211_sub_if_data *connection_find_sdata(struct ieee80211_local *local, struct ieee80211_vif *connection_vif){
	struct list_head *pos = NULL;

	if (local == NULL || connection_vif == NULL){
		return NULL;
	}
	for (pos = local->interfaces.next; pos != &local->interfaces; pos = pos->next){
		struct ieee80211_sub_if_data *sdata = (struct ieee80211_sub_if_data *)pos;

		if (sdata->vif == connection_vif){
			return sdata;
		}
	}
	return NULL;
}

static inline int connection_configure_channel(struct ieee80211_local *local, struct ieee80211_channel *channel){
	int ret = 0;
	struct ieee80211_bss_conf *conf = NULL;
	struct ieee80211_ops *connection_ops = NULL;
	struct ieee80211_vif *connection_vif = NULL;

	connection_ops = director.ieee80211_ops;
	connection_vif = connection_get_vif(local);

	if (local == NULL || channel == NULL || connection_ops == NULL || connection_vif == NULL){
		ret = -EINVAL;
		return ret;
	}
	cfg80211_chandef_create(&local->hw.conf.chandef, channel, NL80211_CHAN_NO_HT);
	conf = &connection_vif->bss_conf;
	conf->chanreq.oper = local->hw.conf.chandef;
	conf->chanreq.ap = local->hw.conf.chandef;

	if (connection_ops->add_chanctx == NULL && connection_ops->config != NULL){
		if (director.kernel_major_version > 6 || (director.kernel_major_version == 6 && director.kernel_minor_version >= 17)){
			int (*config)(struct ieee80211_hw *, int, u32) = connection_ops->config;
			ret = config(local->ieee80211_hw, -1, IEEE80211_CONF_CHANGE_CHANNEL);
		}else{
			int (*config)(struct ieee80211_hw *, u32) = connection_ops->config;
			ret = config(local->ieee80211_hw, IEEE80211_CONF_CHANGE_CHANNEL);
		}
	}
	return ret;
}

static inline int connection_program_target(struct ieee80211_local *local){
	int ret = 0;
	u64 changed = 0;
	struct connection_fsm *fsm = NULL;
	struct ieee80211_bss_conf *conf = NULL;
	struct ieee80211_ops *connection_ops = NULL;
	struct ieee80211_vif *connection_vif = NULL;
	void (*link_info_changed)(struct ieee80211_hw *, struct ieee80211_vif *, struct ieee80211_bss_conf *, u64) = NULL;
	void (*bss_info_changed)(struct ieee80211_hw *, struct ieee80211_vif *, struct ieee80211_bss_conf *, u64) = NULL;

	changed = BSS_CHANGED_BSSID | BSS_CHANGED_BASIC_RATES | BSS_CHANGED_BEACON_INT;
	fsm = connection_get();
	connection_ops = director.ieee80211_ops;
	connection_vif = connection_get_vif(local);

	if (fsm == NULL || local == NULL || connection_ops == NULL || connection_vif == NULL){
		ret = -EINVAL;
		return ret;
	}
	link_info_changed = connection_ops->link_info_changed;
	bss_info_changed = connection_ops->bss_info_changed;
	conf = &connection_vif->bss_conf;
	if (link_info_changed != NULL){
		link_info_changed(local->ieee80211_hw, connection_vif, conf, changed);
	}else if (bss_info_changed != NULL){
		bss_info_changed(local->ieee80211_hw, connection_vif, conf, changed);
	}
	return ret;
}

static inline int connection_configure_queues(struct ieee80211_local *local){
	int ret = 0;
	int current_ret = 0;
	struct ieee80211_ops *ops = NULL;
	struct ieee80211_vif *vif = NULL;
	struct ieee80211_bss_conf *conf = NULL;
	void (*link_info_changed)(struct ieee80211_hw *, struct ieee80211_vif *, struct ieee80211_bss_conf *, u64) = NULL;
	static const struct ieee80211_tx_queue_params params[IEEE80211_NUM_ACS] = {
		[IEEE80211_AC_VO] = { .txop = 47, .cw_min = 3, .cw_max = 7, .aifs = 2 },
		[IEEE80211_AC_VI] = { .txop = 94, .cw_min = 7, .cw_max = 15, .aifs = 2 },
		[IEEE80211_AC_BE] = { .txop = 0, .cw_min = 15, .cw_max = 1023, .aifs = 3 },
		[IEEE80211_AC_BK] = { .txop = 0, .cw_min = 15, .cw_max = 1023, .aifs = 7 },
	};

	ops = director.ieee80211_ops;
	vif = connection_get_vif(local);

	if (local == NULL || ops == NULL || vif == NULL){
		ret = -EINVAL;
		return ret;
	}
	conf = &vif->bss_conf;
	link_info_changed = ops->link_info_changed;
	conf->use_cts_prot = false;
	conf->use_short_preamble = true;
	conf->use_short_slot = true;
	if (link_info_changed != NULL){
		link_info_changed(local->ieee80211_hw, vif, conf,
				  BSS_CHANGED_ERP_CTS_PROT | BSS_CHANGED_ERP_PREAMBLE | BSS_CHANGED_ERP_SLOT);
	}
	if (ops->conf_tx != NULL){
		for (u16 ac = 0; ac < IEEE80211_NUM_ACS; ac++){
			current_ret = 0;

			if (director.kernel_major_version >= 6){
				int (*conf_tx)(struct ieee80211_hw *, struct ieee80211_vif *, unsigned int,
					       u16, const struct ieee80211_tx_queue_params *) = ops->conf_tx;
				current_ret = conf_tx(local->ieee80211_hw, vif, 0, ac, &params[ac]);
			}else{
				int (*conf_tx)(struct ieee80211_hw *, struct ieee80211_vif *, u16,
					       const struct ieee80211_tx_queue_params *) = ops->conf_tx;
				current_ret = conf_tx(local->ieee80211_hw, vif, ac, &params[ac]);
			}
			if (current_ret != 0 && ret == 0){
				ret = current_ret;
			}
		}
	}
	conf->qos = true;
	if (link_info_changed != NULL){
		link_info_changed(local->ieee80211_hw, vif, conf, BSS_CHANGED_QOS);
	}
	return ret;
}

static inline int connection_leave_idle(struct ieee80211_local *local){
	int ret = 0;
	struct ieee80211_ops *ops = NULL;

	ops = director.ieee80211_ops;

	if (local == NULL || ops == NULL || ops->config == NULL){
		return ret;
	}
	local->hw.conf.flags &= ~IEEE80211_CONF_IDLE;
	if (director.kernel_major_version > 6 ||
	    (director.kernel_major_version == 6 && director.kernel_minor_version >= 17)){
		int (*config)(struct ieee80211_hw *, int, u32) = ops->config;
		ret = config(local->ieee80211_hw, -1, IEEE80211_CONF_CHANGE_IDLE);
		return ret;
	}else{
		int (*config)(struct ieee80211_hw *, u32) = ops->config;
		ret = config(local->ieee80211_hw, IEEE80211_CONF_CHANGE_IDLE);
		return ret;
	}
}

static inline int connection_assign_chanctx(struct ieee80211_local *local){
	int ret = 0;
	struct connection_fsm *fsm = NULL;
	struct ieee80211_ops *ops = NULL;
	struct ieee80211_vif *vif = NULL;
	struct ieee80211_bss_conf *conf = NULL;
	struct ieee80211_chanctx_conf *ctx = NULL;
	int (*add_chanctx)(struct ieee80211_hw *, struct ieee80211_chanctx_conf *) = NULL;
	void (*remove_chanctx)(struct ieee80211_hw *, struct ieee80211_chanctx_conf *) = NULL;
	void (*change_chanctx)(struct ieee80211_hw *, struct ieee80211_chanctx_conf *, u32) = NULL;
	int (*assign_vif_chanctx)(struct ieee80211_hw *, struct ieee80211_vif *, struct ieee80211_bss_conf *, struct ieee80211_chanctx_conf *) = NULL;

	fsm = connection_get();
	ops = director.ieee80211_ops;
	vif = connection_get_vif(local);

	if (fsm == NULL || local == NULL || ops == NULL || vif == NULL){
		ret = -EINVAL;
		return ret;
	}
	add_chanctx = ops->add_chanctx;
	remove_chanctx = ops->remove_chanctx;
	change_chanctx = ops->change_chanctx;
	assign_vif_chanctx = ops->assign_vif_chanctx;
	if (add_chanctx == NULL || assign_vif_chanctx == NULL){
		return ret;
	}
	if (fsm->chanctx != NULL){
		return ret;
	}
	ctx = calloc(1, sizeof(*ctx) + local->hw.chanctx_data_size);
	if (ctx == NULL){
		ret = -ENOMEM;
		return ret;
	}
	ctx->def = local->hw.conf.chandef;
	ctx->min_def = local->hw.conf.chandef;
	ctx->ap = local->hw.conf.chandef;
	ctx->radio_idx = -1;
	ctx->rx_chains_static = 1;
	ctx->rx_chains_dynamic = 1;
	ret = add_chanctx(local->ieee80211_hw, ctx);
	if (ret != 0){
		free(ctx);
		return ret;
	}
	if (change_chanctx != NULL){
		change_chanctx(local->ieee80211_hw, ctx, IEEE80211_CHANCTX_CHANGE_WIDTH);
	}
	conf = &vif->bss_conf;
	conf->chanctx_conf = ctx;
	ret = assign_vif_chanctx(local->ieee80211_hw, vif, conf, ctx);
	if (ret != 0){
		conf->chanctx_conf = NULL;
		if (remove_chanctx != NULL){
			remove_chanctx(local->ieee80211_hw, ctx);
		}
		free(ctx);
		return ret;
	}
	fsm->chanctx = ctx;
	return ret;
}

static inline void connection_notify_pre_auth(struct ieee80211_local *local){
	struct connection_fsm *fsm = NULL;
	struct ieee80211_ops *ops = NULL;
	struct ieee80211_vif *vif = NULL;
	struct ieee80211_bss_conf *conf = NULL;
	void (*vif_cfg_changed)(struct ieee80211_hw *, struct ieee80211_vif *, u64) = NULL;
	void (*link_info_changed)(struct ieee80211_hw *, struct ieee80211_vif *, struct ieee80211_bss_conf *, u64) = NULL;

	fsm = connection_get();
	ops = director.ieee80211_ops;
	vif = connection_get_vif(local);
	if (fsm == NULL || local == NULL || ops == NULL || vif == NULL){
		goto end;
	}
	conf = &vif->bss_conf;
	link_info_changed = ops->link_info_changed;
	vif_cfg_changed = ops->vif_cfg_changed;
	if (link_info_changed != NULL){
		link_info_changed(local->ieee80211_hw, vif, conf, BSS_CHANGED_TXPOWER);
	}
	vif->cfg.idle = false;
	memcpy(vif->cfg.ap_addr, fsm->bssid, ETH_ALEN);
	if (vif_cfg_changed != NULL){
		vif_cfg_changed(local->ieee80211_hw, vif, BSS_CHANGED_IDLE);
	}
	end:
}

static inline int ieee80211_update_rx_filter(struct ieee80211_local *local, unsigned int flags, bool enable){
	int ret = 0;
	unsigned int new_flags = 0;
	unsigned int changed_flags = 0;
	struct ieee80211_ops *ops = NULL;
	void (*configure_filter)(struct ieee80211_hw *, unsigned int, unsigned int *, u64) = NULL;

	ops = director.ieee80211_ops;
	if (local == NULL || local->ieee80211_hw == NULL || ops == NULL){
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
	return ieee80211_update_rx_filter(local, EXSUL_RX_DUMP_FILTER_FLAGS, enable);
}

static inline int connection_enable_management_rx(struct ieee80211_local *local){
	return ieee80211_update_rx_filter(local,
					  EXSUL_FIF_BCN_PRBRESP_PROMISC |
					  EXSUL_FIF_OTHER_BSS,
					  true);
}

static inline int connection_send_frame(struct ieee80211_local *local, struct sk_buff *skb,
					struct ieee80211_sta *sta, enum nl80211_band band,
					enum connection_tx_kind kind){
	int ret = 0;
	enum ieee80211_ac_numbers ac = IEEE80211_AC_BE;
	struct ieee80211_tx_info *info = NULL;
	struct ieee80211_ops *connection_ops = NULL;
	struct ieee80211_vif *connection_vif = NULL;
	void (*tx)(struct ieee80211_hw *, struct ieee80211_tx_control *, struct sk_buff *) = NULL;
	struct ieee80211_tx_control control = {};

	connection_ops = director.ieee80211_ops;
	connection_vif = connection_get_vif(local);
	if (connection_ops != NULL){
		tx = connection_ops->tx;
	}

	if (local == NULL || skb == NULL || connection_vif == NULL || connection_ops == NULL || tx == NULL){
		if (skb != NULL){
			kfree_skb(skb);
		}
		ret = -EOPNOTSUPP;
		return ret;
	}
	info = IEEE80211_SKB_CB(skb);
	memset(info, 0, sizeof(*info));
	info->flags = IEEE80211_TX_CTL_FIRST_FRAGMENT | IEEE80211_TX_INTFL_DONT_ENCRYPT | IEEE80211_TX_CTL_USE_MINRATE;
	if (kind == CONNECTION_TX_MANAGEMENT){
		ac = IEEE80211_AC_VO;
		skb->priority = 7;
		info->flags |= IEEE80211_TX_CTL_ASSIGN_SEQ | IEEE80211_TX_CTL_REQ_TX_STATUS | IEEE80211_TX_INTFL_MLME_CONN_TX;
	}else if (kind == CONNECTION_TX_EAPOL){
		ac = IEEE80211_AC_BE;
		skb->priority = 0;
		info->control.flags |= IEEE80211_TX_CTRL_PORT_CTRL_PROTO;
	}else{
		kfree_skb(skb);
		ret = -EINVAL;
		return ret;
	}
	if (connection_vif->hw_queue[IEEE80211_AC_VI] == 0 &&
	    connection_vif->hw_queue[IEEE80211_AC_BE] == 0 &&
	    connection_vif->hw_queue[IEEE80211_AC_BK] == 0){
		for (u8 queue_ac = 0; queue_ac < IEEE80211_NUM_ACS; queue_ac++){
			connection_vif->hw_queue[queue_ac] = queue_ac;
		}
		connection_vif->cab_queue = IEEE80211_INVAL_HW_QUEUE;
	}
	skb_set_queue_mapping(skb, ac);
	info->band = band;
	info->hw_queue = connection_vif->hw_queue[ac];
	info->control.vif = connection_vif;
	info->control.rates[0].idx = 0;
	info->control.rates[0].count = 1;
	info->control.rates[1].idx = -1;
	control.sta = sta;
	director.connection_tx_trace = true;
	tx(local->ieee80211_hw, &control, skb);
	director.connection_tx_trace = false;

	return ret;
}

static inline int wifi_connection_send_auth(struct ieee80211_local *local, struct ieee80211_channel *channel){
	int ret = 0;
	int frame_len = 0;
	struct sk_buff *skb = NULL;
	struct ieee80211_mgmt *mgmt = NULL;
	struct connection_fsm *fsm = NULL;
	struct ieee80211_vif *connection_vif = NULL;

	fsm = connection_get();
	connection_vif = connection_get_vif(local);
	frame_len = offsetof(struct ieee80211_mgmt, u.auth.variable);

	if (fsm == NULL || local == NULL || connection_vif == NULL || channel == NULL){
		ret = -EINVAL;
		return ret;
	}
	skb = dev_alloc_skb(local->hw.extra_tx_headroom + frame_len);
	if (skb == NULL){
		ret = -ENOMEM;
		return ret;
	}
	skb_reserve(skb, local->hw.extra_tx_headroom);
	mgmt = skb_put_zero(skb, frame_len);
	mgmt->frame_control = cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_AUTH);
	mgmt->seq_ctrl = cpu_to_le16((fsm->tx_sequence++ & 0x0fff) << 4);
	memcpy(mgmt->da, fsm->bssid, ETH_ALEN);
	memcpy(mgmt->sa, connection_vif->addr, ETH_ALEN);
	memcpy(mgmt->bssid, fsm->bssid, ETH_ALEN);
	mgmt->u.auth.auth_alg = cpu_to_le16(0);
	mgmt->u.auth.auth_transaction = cpu_to_le16(1);
	mgmt->u.auth.status_code = cpu_to_le16(WLAN_STATUS_SUCCESS);
	fsm->auth_attempts++;
	ret = connection_send_frame(local, skb, fsm->sta, channel->band, CONNECTION_TX_MANAGEMENT);
	connection_set_state(fsm, ret == 0 ? CONNECTION_AUTH_SENT : CONNECTION_FAILED);
	return ret;
}

static inline int connection_prepare_station(struct ieee80211_local *local, struct ieee80211_channel *channel){
	int ret = 0;
	int sta_data_size = 0;
	size_t sta_size = 0;
	size_t sta_priv_size = 0;
	size_t sta_alloc_size = 0;
	struct connection_fsm *fsm = NULL;
	struct ieee80211_ops *connection_ops = NULL;
	struct ieee80211_vif *connection_vif = NULL;
	struct ieee80211_sub_if_data *sdata = NULL;
	struct ieee80211_sta *connection_sta = NULL;
	struct ieee80211_link_sta *connection_link_sta = NULL;
	int (*sta_state)(struct ieee80211_hw *, struct ieee80211_vif *, struct ieee80211_sta *, enum ieee80211_sta_state, enum ieee80211_sta_state) = NULL;

	static const struct rhashtable_params sta_rht_params = {
		.nelem_hint = 3, .automatic_shrinking = true,
		.head_offset = offsetof(struct sta_info, hash_node),
		.key_offset = offsetof(struct sta_info, addr), .key_len = ETH_ALEN,
		.max_size = CONFIG_MAC80211_STA_HASH_MAX_SIZE,
	};
	static const struct rhashtable_params link_sta_rht_params = {
		.nelem_hint = 3, .automatic_shrinking = true,
		.head_offset = offsetof(struct link_sta_info, link_hash_node),
		.key_offset = offsetof(struct link_sta_info, addr), .key_len = ETH_ALEN,
		.max_size = CONFIG_MAC80211_STA_HASH_MAX_SIZE,
	};
	fsm = connection_get();
	connection_ops = director.ieee80211_ops;
	connection_vif = connection_get_vif(local);

	if (connection_ops != NULL){
		sta_state = connection_ops->sta_state;
	}
	if (fsm == NULL || local == NULL || channel == NULL || connection_ops == NULL || sta_state == NULL){
		ret = -EOPNOTSUPP;
		return ret;
	}
	if (fsm->sta != NULL){
		return ret;
	}
	sta_size = director.struct_sizes.ieee80211_sta;
	if (sta_size == 0){
		ret = -EINVAL;
		return ret;
	}
	if (sta_size < CONNECTION_STA_BASE_FALLBACK_SIZE){
		sta_size = CONNECTION_STA_BASE_FALLBACK_SIZE;
	}
	sta_data_size = local->hw.sta_data_size;
	if (sta_data_size > 0){
		sta_priv_size = (size_t)sta_data_size;
	}
	if (sta_priv_size == 0 || sta_priv_size > CONNECTION_STA_PRIV_MAX_SIZE){
		sta_priv_size = CONNECTION_STA_PRIV_FALLBACK_SIZE;
		local->hw.sta_data_size = sta_priv_size;
	}
	sta_alloc_size = sta_size + sta_priv_size;
	if (sta_alloc_size < CONNECTION_STA_ALLOC_FALLBACK_SIZE){
		sta_alloc_size = CONNECTION_STA_ALLOC_FALLBACK_SIZE;
	}
	fsm->sta = calloc(1, sta_alloc_size);
	connection_sta = fsm->sta;
	fsm->sta_info = calloc(1, sizeof(*fsm->sta_info));
	fsm->link_sta_info = calloc(1, sizeof(*fsm->link_sta_info));
	if (fsm->sta == NULL || fsm->sta_info == NULL || fsm->link_sta_info == NULL){
		ret = -ENOMEM;
		return ret;
	}
	memcpy(connection_sta->addr, fsm->bssid, ETH_ALEN);
	connection_sta->wme = true;
	connection_link_sta = &connection_sta->deflink;
	connection_link_sta->sta = connection_sta;
	memcpy(connection_link_sta->addr, fsm->bssid, ETH_ALEN);
	connection_link_sta->link_id = 0;
	connection_link_sta->supp_rates[channel->band] = channel->band == NL80211_BAND_2GHZ ? 0xfff : 0xff;
	connection_link_sta->rx_nss = 1;
	connection_sta->link[0] = connection_link_sta;

	sdata = connection_find_sdata(local, connection_vif);
	INIT_LIST_HEAD(&fsm->sta_info->list);
	memcpy(fsm->sta_info->addr, fsm->bssid, ETH_ALEN);
	fsm->sta_info->sta = fsm->sta;
	fsm->sta_info->vif = connection_vif;
	fsm->sta_info->uploaded = true;
	fsm->sta_info->sta_state = IEEE80211_STA_NOTEXIST;
	fsm->sta_info->sdata.local = local;
	fsm->sta_info->sdata.vif = connection_vif;
	if (sdata != NULL){
		fsm->sta_info->sdata.bss = sdata->bss;
		fsm->sta_info->sdata.state = sdata->state;
	}
	memcpy(fsm->link_sta_info->addr, fsm->bssid, ETH_ALEN);
	fsm->link_sta_info->sta = fsm->sta_info;

	ret = sta_state(local->ieee80211_hw, connection_vif, fsm->sta, IEEE80211_STA_NOTEXIST, IEEE80211_STA_NONE);
	if (ret){
		return ret;
	}

	fsm->sta_info->sta_state = IEEE80211_STA_AUTH;
	ret = rhltable_insert_simple(&local->sta_hash, &fsm->sta_info->hash_node, sta_rht_params);
	if (ret){
		return ret;
	}
	ret = rhltable_insert_simple(&local->link_sta_hash, &fsm->link_sta_info->link_hash_node, link_sta_rht_params);
	if (ret){
		return ret;
	}
	list_add_tail(&fsm->sta_info->list, &local->sta_list);
	local->num_sta++;
	return ret;
}

static inline int connection_send_assoc(struct ieee80211_local *local, struct ieee80211_channel *channel){
	int ret = 0;
	int ies_len = 0;
	int fixed_len = 0;
	u8 *pos = NULL;
	const u8 *rates = NULL;
	struct sk_buff *skb = NULL;
	struct ieee80211_mgmt *mgmt = NULL;
	struct connection_fsm *fsm = NULL;
	struct ieee80211_vif *connection_vif = NULL;

	static const u8 rates_ext_2ghz[] = { 0x30, 0x48, 0x60, 0x6c };
	static const u8 rates_2ghz[] = { 0x82, 0x84, 0x8b, 0x96, 0x0c, 0x12, 0x18, 0x24 };
	static const u8 rates_5ghz[] = { 0x8c, 0x12, 0x98, 0x24, 0xb0, 0x48, 0x60, 0x6c };

	fixed_len = offsetof(struct ieee80211_mgmt, u.assoc_req.variable);
	fsm = connection_get();
	connection_vif = connection_get_vif(local);
	if (fsm == NULL || local == NULL || channel == NULL){
		ret = -EINVAL;
		return ret;
	}
	ret = connection_prepare_station(local, channel);
	if (ret){
		return ret;
	}
	rates = channel->band == NL80211_BAND_2GHZ ? rates_2ghz : rates_5ghz;
	ies_len = 2 + fsm->ssid_len + 2 + 8 +
		  (channel->band == NL80211_BAND_2GHZ ? 2 + sizeof(rates_ext_2ghz) : 0) +
		  (fsm->protected_network ? sizeof(connection_rsn_ie) : 0);
	skb = dev_alloc_skb(local->hw.extra_tx_headroom + fixed_len + ies_len);
	if (skb == NULL){
		ret = -ENOMEM;
		return ret;
	}
	skb_reserve(skb, local->hw.extra_tx_headroom);
	mgmt = skb_put_zero(skb, fixed_len);
	mgmt->frame_control = cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_ASSOC_REQ);
	mgmt->seq_ctrl = cpu_to_le16((fsm->tx_sequence++ & 0x0fff) << 4);
	memcpy(mgmt->da, fsm->bssid, ETH_ALEN);
	memcpy(mgmt->sa, connection_vif->addr, ETH_ALEN);
	memcpy(mgmt->bssid, fsm->bssid, ETH_ALEN);
	mgmt->u.assoc_req.capab_info = cpu_to_le16(fsm->capability &
						     (WLAN_CAPABILITY_ESS | WLAN_CAPABILITY_PRIVACY |
						      WLAN_CAPABILITY_SHORT_PREAMBLE | WLAN_CAPABILITY_SHORT_SLOT_TIME));
	mgmt->u.assoc_req.listen_interval = cpu_to_le16(10);
	pos = skb_put(skb, ies_len);
	*pos++ = WLAN_EID_SSID;
	*pos++ = fsm->ssid_len;
	memcpy(pos, fsm->ssid, fsm->ssid_len);
	pos += fsm->ssid_len;
	*pos++ = WLAN_EID_SUPP_RATES;
	*pos++ = 8;
	memcpy(pos, rates, 8);
	pos += 8;
	if (channel->band == NL80211_BAND_2GHZ){
		*pos++ = WLAN_EID_EXT_SUPP_RATES;
		*pos++ = sizeof(rates_ext_2ghz);
		memcpy(pos, rates_ext_2ghz, sizeof(rates_ext_2ghz));
		pos += sizeof(rates_ext_2ghz);
	}
	if (fsm->protected_network){
		memcpy(pos, connection_rsn_ie, sizeof(connection_rsn_ie));
	}

	fsm->assoc_attempts++;
	ret = connection_send_frame(local, skb, fsm->sta, channel->band, CONNECTION_TX_MANAGEMENT);
	connection_set_state(fsm, ret == 0 ? CONNECTION_ASSOC_SENT : CONNECTION_FAILED);
	return ret;
}

static void *connection_retry_worker(void *arg){
	struct connection_retry_job *job = arg;
	struct ieee80211_local *local = job != NULL ? job->local : NULL;
	struct connection_fsm *fsm = job != NULL ? job->fsm : NULL;

	free(job);
	while (fsm != NULL && local != NULL && director.connection_fsm == fsm){
		struct ieee80211_channel *channel;

		usleep(700000);
		if (director.connection_fsm != fsm){
			break;
		}
		channel = connection_find_channel(local->wiphy, fsm->center_freq);
		if (channel == NULL){
			connection_set_state(fsm, CONNECTION_FAILED);
			break;
		}
		if (fsm->state == CONNECTION_AUTH_SENT){
			if (fsm->auth_attempts >= 4){
				#ifdef ENABLE_DBG_PRINT
				printf("[!] authentication timed out after %u attempts\n", fsm->auth_attempts);
				#endif
				connection_set_state(fsm, CONNECTION_FAILED);
				break;
			}
			wifi_connection_send_auth(local, channel);
			continue;
		}
		if (fsm->state == CONNECTION_ASSOC_SENT){
			if (fsm->assoc_attempts >= 4){
				#ifdef ENABLE_DBG_PRINT
				printf("[!] association timed out after %u attempts\n", fsm->assoc_attempts);
				#endif
				connection_set_state(fsm, CONNECTION_FAILED);
				break;
			}
			connection_send_assoc(local, channel);
			continue;
		}
		break;
	}
	return NULL;
}

static inline int connection_start_retry_worker(struct ieee80211_local *local, struct connection_fsm *fsm){
	int ret = 0;
	pthread_t thread;
	struct connection_retry_job *job = NULL;

	if (local == NULL || fsm == NULL || fsm->retry_worker_started){
		return ret;
	}
	job = calloc(1, sizeof(*job));
	if (job == NULL){
		ret = -ENOMEM;
		return ret;
	}
	job->local = local;
	job->fsm = fsm;
	if (pthread_create(&thread, NULL, connection_retry_worker, job) != 0){
		free(job);
		ret = -EAGAIN;
		return ret;
	}
	fsm->retry_worker_started = true;
	#ifdef __linux__
	pthread_detach(thread);
	#endif
	return ret;
}

static inline int connection_notify_assoc(struct ieee80211_local *local, u16 aid){
	int ret = 0;
	struct connection_fsm *fsm = NULL;
	struct ieee80211_vif_cfg *cfg = NULL;
	struct ieee80211_bss_conf *conf = NULL;
	struct ieee80211_ops *connection_ops = NULL;
	struct ieee80211_vif *connection_vif = NULL;
	struct ieee80211_sta *connection_sta = NULL;
	void (*vif_cfg_changed)(struct ieee80211_hw *, struct ieee80211_vif *, u64) = NULL;
	void (*bss_info_changed)(struct ieee80211_hw *, struct ieee80211_vif *, struct ieee80211_bss_conf *, u64) = NULL;
	void (*link_info_changed)(struct ieee80211_hw *, struct ieee80211_vif *, struct ieee80211_bss_conf *, u64) = NULL;
	int (*sta_state)(struct ieee80211_hw *, struct ieee80211_vif *, struct ieee80211_sta *, enum ieee80211_sta_state, enum ieee80211_sta_state) = NULL;

	fsm = connection_get();
	connection_ops = director.ieee80211_ops;
	connection_vif = connection_get_vif(local);
	connection_sta = fsm != NULL ? fsm->sta : NULL;
	if (connection_ops != NULL){
		sta_state = connection_ops->sta_state;
		link_info_changed = connection_ops->link_info_changed;
		vif_cfg_changed = connection_ops->vif_cfg_changed;
		bss_info_changed = connection_ops->bss_info_changed;
	}
	if (fsm == NULL || connection_vif == NULL || connection_sta == NULL || sta_state == NULL){
		ret = -EINVAL;
		return ret;
	}
	cfg = &connection_vif->cfg;
	conf = &connection_vif->bss_conf;
	fsm->aid = aid;
	connection_sta->aid = aid;
	cfg->assoc = true;
	cfg->idle = false;
	cfg->aid = aid;
	cfg->ssid_len = fsm->ssid_len;
	memcpy(cfg->ssid, fsm->ssid, fsm->ssid_len);
	memcpy(cfg->ap_addr, fsm->bssid, ETH_ALEN);
	conf->bssid = cfg->ap_addr;
	conf->assoc_capability = fsm->capability;
	if (conf->dtim_period == 0){
		conf->dtim_period = 1;
	}
	ret = sta_state(local->ieee80211_hw, connection_vif, fsm->sta, IEEE80211_STA_AUTH, IEEE80211_STA_ASSOC);
	if (ret){
		return ret;
	}
	fsm->sta_info->sta_state = IEEE80211_STA_ASSOC;
	if (link_info_changed != NULL){
		link_info_changed(local->ieee80211_hw, connection_vif, conf,
				  BSS_CHANGED_BSSID | BSS_CHANGED_BASIC_RATES | BSS_CHANGED_BEACON_INT);
	}
	if (vif_cfg_changed != NULL){
		vif_cfg_changed(local->ieee80211_hw, connection_vif, BSS_CHANGED_ASSOC | BSS_CHANGED_IDLE);
	}else if (bss_info_changed != NULL){
		bss_info_changed(local->ieee80211_hw, connection_vif, conf, BSS_CHANGED_ASSOC | BSS_CHANGED_IDLE);
	}
	return ret;
}

static inline int connection_authorize(struct ieee80211_local *local){
	int ret = 0;
	struct connection_fsm *fsm = NULL;
	struct ieee80211_ops *connection_ops = NULL;
	struct ieee80211_vif *connection_vif = NULL;
	int (*sta_state)(struct ieee80211_hw *, struct ieee80211_vif *, struct ieee80211_sta *, enum ieee80211_sta_state, enum ieee80211_sta_state) = NULL;

	fsm = connection_get();
	connection_ops = director.ieee80211_ops;
	connection_vif = connection_get_vif(local);
	if (connection_ops != NULL){
		sta_state = connection_ops->sta_state;
	}
	if (fsm == NULL || connection_vif == NULL || sta_state == NULL){
		ret = -EINVAL;
		return ret;
	}
	ret = sta_state(local->ieee80211_hw, connection_vif, fsm->sta, IEEE80211_STA_ASSOC, IEEE80211_STA_AUTHORIZED);
	if (ret == 0){
		fsm->sta_info->sta_state = IEEE80211_STA_AUTHORIZED;
		connection_set_state(fsm, CONNECTION_CONNECTED);
		printf("[*] connected: ssid=\"%.*s\" bssid=%02x:%02x:%02x:%02x:%02x:%02x aid=%u\n",
		       fsm->ssid_len, fsm->ssid, fsm->bssid[0], fsm->bssid[1], fsm->bssid[2],
		       fsm->bssid[3], fsm->bssid[4], fsm->bssid[5], fsm->aid);
	}
	return ret;
}

static inline int connection_send_eapol(struct ieee80211_local *local, const u8 *eapol, int eapol_len){
	int ret = 0;
	int frame_len = 0;
	u8 *payload = NULL;
	struct sk_buff *skb = NULL;
	struct connection_fsm *fsm = NULL;
	struct ieee80211_hdr_3addr *hdr = NULL;
	struct ieee80211_channel *channel = NULL;
	struct ieee80211_vif *connection_vif = NULL;
	static const u8 snap[] = { 0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00, 0x88, 0x8e };

	fsm = connection_get();
	connection_vif = connection_get_vif(local);
	frame_len = sizeof(*hdr) + sizeof(snap) + eapol_len;

	if (fsm == NULL || local == NULL || eapol == NULL || eapol_len == 0){
		ret = -EINVAL;
		return ret;
	}
	channel = connection_find_channel(local->wiphy, fsm->center_freq);
	if (channel == NULL){
		ret = -EINVAL;
		return ret;
	}
	skb = dev_alloc_skb(local->hw.extra_tx_headroom + frame_len);
	if (skb == NULL){
		ret = -ENOMEM;
		return ret;
	}
	skb_reserve(skb, local->hw.extra_tx_headroom);
	hdr = skb_put_zero(skb, sizeof(*hdr));
	hdr->frame_control = cpu_to_le16(IEEE80211_FTYPE_DATA | IEEE80211_STYPE_DATA | IEEE80211_FCTL_TODS);
	hdr->seq_ctrl = cpu_to_le16((fsm->tx_sequence++ & 0x0fff) << 4);
	memcpy(hdr->addr1, fsm->bssid, ETH_ALEN);
	memcpy(hdr->addr2, connection_vif->addr, ETH_ALEN);
	memcpy(hdr->addr3, fsm->bssid, ETH_ALEN);
	payload = skb_put(skb, sizeof(snap) + eapol_len);
	memcpy(payload, snap, sizeof(snap));
	memcpy(payload + sizeof(snap), eapol, eapol_len);
	ret = connection_send_frame(local, skb, fsm->sta, channel->band, CONNECTION_TX_EAPOL);
	return ret;
}

static inline int connection_build_key_eapol(u8 *out, size_t out_cap, u16 key_info,
					     u64 replay_counter, const u8 *nonce,
					     const u8 *key_data, u16 key_data_len,
					     const u8 kck[16], int *out_len){
	int ret = 0;
	int total = 0;
	u8 *key = NULL;
	u8 mic[20] = {};
	total = 4 + CONNECTION_EAPOL_KEY_FIXED_LEN + key_data_len;

	if (out == NULL || kck == NULL || out_len == NULL || total > out_cap){
		ret = -EINVAL;
		return ret;
	}
	memset(out, 0, total);
	out[0] = CONNECTION_EAPOL_VERSION;
	out[1] = CONNECTION_EAPOL_TYPE_KEY;
	connection_put_be16(out + 2, CONNECTION_EAPOL_KEY_FIXED_LEN + key_data_len);
	key = out + 4;
	key[0] = CONNECTION_EAPOL_KEY_DESC_RSN;
	connection_put_be16(key + 1, key_info);
	connection_put_be16(key + 3, 16);
	connection_put_be64(key + 5, replay_counter);
	if (nonce != NULL){
		memcpy(key + 13, nonce, 32);
	}
	connection_put_be16(key + 93, key_data_len);
	if (key_data != NULL && key_data_len != 0){
		memcpy(key + 95, key_data, key_data_len);
	}
	if (connection_hmac_sha1(kck, 16, out, total, mic)){
		ret = -EINVAL;
		return ret;
	}
	memcpy(key + 77, mic, 16);
	*out_len = total;
	return ret;
}

static inline int connection_find_gtk(struct connection_fsm *fsm, const u8 *buf, int len){
	int ret = 0;
	int pos = 0;

	while (pos + 2 <= len){
		u8 eid = buf[pos];
		u8 elen = buf[pos + 1];

		if (pos + 2 + elen > len){
			break;
		}
		if (eid == 0xdd && elen >= 22 && buf[pos + 2] == 0x00 && buf[pos + 3] == 0x0f &&
		    buf[pos + 4] == 0xac && buf[pos + 5] == 0x01){
			fsm->gtk_key_idx = buf[pos + 6] & 0x03;
			fsm->gtk_len = min((elen - 6), sizeof(fsm->gtk));
			memcpy(fsm->gtk, buf + pos + 8, fsm->gtk_len);
			return ret;
		}
		pos += 2 + elen;
	}
	ret = -ENOENT;
	return ret;
}

static inline int connection_install_key(struct ieee80211_local *local, const u8 *key, u8 key_len, u8 key_idx, bool pairwise, struct ieee80211_key_conf **stored){
	int ret = 0;
	struct connection_fsm *fsm = NULL;
	struct ieee80211_key_conf *conf = NULL;
	struct ieee80211_ops *connection_ops = NULL;
	struct ieee80211_vif *connection_vif = NULL;
	int (*set_key)(struct ieee80211_hw *, enum set_key_cmd, struct ieee80211_vif *, struct ieee80211_sta *, struct ieee80211_key_conf *) = NULL;

	fsm = connection_get();
	connection_ops = director.ieee80211_ops;
	connection_vif = connection_get_vif(local);
	if (connection_ops != NULL){
		set_key = connection_ops->set_key;
	}
	if (fsm == NULL || connection_vif == NULL || set_key == NULL || key == NULL || stored == NULL){
		ret = -EOPNOTSUPP;
		return ret;
	}
	conf = calloc(1, sizeof(*conf) + key_len);
	if (conf == NULL){
		ret = -ENOMEM;
		return ret;
	}
	conf->cipher = WLAN_CIPHER_SUITE_CCMP;
	conf->keyidx = key_idx;
	conf->keylen = key_len;
	conf->iv_len = 8;
	conf->icv_len = 8;
	conf->link_id = pairwise ? -1 : 0;
	if (pairwise){
		conf->flags |= IEEE80211_KEY_FLAG_PAIRWISE;
	}
	memcpy(conf->key, key, key_len);
	ret = set_key(local->ieee80211_hw, SET_KEY, connection_vif, pairwise ? fsm->sta : NULL, conf);
	if (ret){
		free(conf);
		return ret;
	}
	*stored = conf;
	return ret;
}

static inline int connection_install_keys(struct ieee80211_local *local){
	int ret = 0;
	struct connection_fsm *fsm = NULL;

	fsm = connection_get();

	if (!fsm->ptk_installed){
		ret = connection_install_key(local, fsm->ptk + 32, 16, 0, true, &fsm->ptk_key);
		if (ret){
			return ret;
		}
		fsm->ptk_installed = true;
	}
	if (fsm->gtk_len >= 16 && !fsm->gtk_installed){
		ret = connection_install_key(local, fsm->gtk, 16, fsm->gtk_key_idx, false, &fsm->gtk_key);
		if (ret){
			return ret;
		}
		fsm->gtk_installed = true;
	}
	return ret;
}

static inline int connection_handle_m1(struct ieee80211_local *local, const u8 *eapol, int eapol_len, u16 key_info){
	int ret = 0;
	u8 descriptor_version = 0;
	u64 replay_counter = 0;
	struct connection_fsm *fsm = NULL;
	struct ieee80211_vif *connection_vif = NULL;

	fsm = connection_get();
	connection_vif = connection_get_vif(local);
	descriptor_version = key_info & CONNECTION_EAPOL_KEY_INFO_VERSION_MASK;

	if (fsm == NULL || connection_vif == NULL || eapol_len < CONNECTION_EAPOL_KEY_MIN_LEN){
		ret = -EINVAL;
		return ret;
	}
	replay_counter = connection_get_be64(eapol + 9);
	if (fsm->state == CONNECTION_WAIT_EAPOL_M3 && replay_counter == fsm->m1_replay_counter &&
	    memcmp(fsm->anonce, eapol + 17, sizeof(fsm->anonce)) == 0 && fsm->m2_len != 0){
		ret = connection_send_eapol(local, fsm->m2, fsm->m2_len);
		return ret;
	}
	if (fsm->state != CONNECTION_WAIT_EAPOL_M1){
		return ret;
	}
	if (descriptor_version != 2){
		ret = -EOPNOTSUPP;
		return ret;
	}
	if (getrandom(fsm->snonce, sizeof(fsm->snonce), 0) != sizeof(fsm->snonce)){
		ret = -EIO;
		return ret;
	}
	memcpy(fsm->anonce, eapol + 17, sizeof(fsm->anonce));
	fsm->descriptor_version = descriptor_version;
	fsm->replay_counter = replay_counter;
	fsm->m1_replay_counter = replay_counter;
	ret = connection_derive_ptk(fsm, connection_vif->addr);
	if (ret){
		return ret;
	}
	ret = connection_build_key_eapol(fsm->m2, sizeof(fsm->m2),
					descriptor_version | CONNECTION_EAPOL_KEY_INFO_PAIRWISE | CONNECTION_EAPOL_KEY_INFO_MIC,
					replay_counter, fsm->snonce, connection_rsn_ie, sizeof(connection_rsn_ie),
					fsm->ptk, &fsm->m2_len);
	if (ret){
		return ret;
	}

	ret = connection_send_eapol(local, fsm->m2, fsm->m2_len);
	if (ret == 0){
		connection_set_state(fsm, CONNECTION_WAIT_EAPOL_M3);
	}
	return ret;
}

static inline int connection_handle_m3(struct ieee80211_local *local, const u8 *eapol, int eapol_len, u16 key_info){
	int ret = 0;
	int plain_len = 0;
	u16 key_data_len = 0;
	u64 replay_counter = 0;
	const u8 *key_data = NULL;
	struct connection_fsm *fsm = NULL;
	u8 mic[20] = {};
	u8 plain[512] = {};
	u8 check[2048] = {};

	fsm = connection_get();
	if (fsm == NULL || eapol_len < CONNECTION_EAPOL_KEY_MIN_LEN || eapol_len > sizeof(check)){
		ret = -EINVAL;
		return ret;
	}
	replay_counter = connection_get_be64(eapol + 9);
	if (fsm->state == CONNECTION_CONNECTED && replay_counter == fsm->m3_replay_counter && fsm->m4_len != 0){
		ret = connection_send_eapol(local, fsm->m4, fsm->m4_len);
		return ret;
	}
	if (fsm->state != CONNECTION_WAIT_EAPOL_M3 || replay_counter <= fsm->m1_replay_counter){
		ret = -EACCES;
		return ret;
	}
	if ((key_info & CONNECTION_EAPOL_KEY_INFO_VERSION_MASK) != fsm->descriptor_version ||
	    memcmp(fsm->anonce, eapol + 17, sizeof(fsm->anonce)) != 0){
		ret = -EACCES;
		return ret;
	}
	memcpy(check, eapol, eapol_len);
	memset(check + 81, 0, 16);
	if (connection_hmac_sha1(fsm->ptk, 16, check, eapol_len, mic) || memcmp(mic, eapol + 81, 16) != 0){
		ret = -EACCES;
		return ret;
	}
	key_data_len = connection_get_be16(eapol + 97);
	key_data = eapol + 99;
	if (key_data_len + 99 > eapol_len){
		ret = -EINVAL;
		return ret;
	}
	if (key_data_len != 0){
		if ((key_info & CONNECTION_EAPOL_KEY_INFO_ENCRYPTED) && key_data_len > sizeof(plain) + 8){
			ret = -E2BIG;
			return ret;
		}
		if (key_info & CONNECTION_EAPOL_KEY_INFO_ENCRYPTED){
			ret = connection_aes_unwrap(fsm->ptk + 16, key_data, key_data_len, plain, &plain_len);
			if (ret){
				return ret;
			}
			connection_find_gtk(fsm, plain, plain_len);
		}else{
			connection_find_gtk(fsm, key_data, key_data_len);
		}
	}
	ret = connection_build_key_eapol(fsm->m4, sizeof(fsm->m4),
					fsm->descriptor_version | CONNECTION_EAPOL_KEY_INFO_PAIRWISE |
					CONNECTION_EAPOL_KEY_INFO_MIC | CONNECTION_EAPOL_KEY_INFO_SECURE,
					replay_counter, NULL, NULL, 0, fsm->ptk, &fsm->m4_len);
	if (ret){
		return ret;
	}
	ret = connection_install_keys(local);
	if (ret){
		return ret;
	}

	ret = connection_send_eapol(local, fsm->m4, fsm->m4_len);
	if (ret){
		return ret;
	}
	fsm->replay_counter = replay_counter;
	fsm->m3_replay_counter = replay_counter;
	ret = connection_authorize(local);
	return ret;
}

static inline int connection_handle_eapol(struct ieee80211_local *local, const u8 *eapol, int eapol_len){
	int ret = 0;
	u16 body_len = 0;
	u16 key_info = 0;
	u16 required = 0;
	struct connection_fsm *fsm = NULL;

	required = CONNECTION_EAPOL_KEY_INFO_PAIRWISE | CONNECTION_EAPOL_KEY_INFO_ACK;
	fsm = connection_get();
	if (fsm == NULL || !fsm->protected_network || eapol == NULL || eapol_len < CONNECTION_EAPOL_KEY_MIN_LEN ||
	    eapol[0] != CONNECTION_EAPOL_VERSION || eapol[1] != CONNECTION_EAPOL_TYPE_KEY ||
	    eapol[4] != CONNECTION_EAPOL_KEY_DESC_RSN){
		return ret;
	}
	body_len = connection_get_be16(eapol + 2);
	if (body_len < CONNECTION_EAPOL_KEY_FIXED_LEN || body_len + 4 > eapol_len){
		ret = -EINVAL;
		return ret;
	}
	eapol_len = body_len + 4;
	key_info = connection_get_be16(eapol + 5);
	if ((key_info & required) != required ||
	    (key_info & (CONNECTION_EAPOL_KEY_INFO_ERROR | CONNECTION_EAPOL_KEY_INFO_REQUEST))){
		return ret;
	}
	if (!(key_info & CONNECTION_EAPOL_KEY_INFO_MIC)){
		if (key_info & CONNECTION_EAPOL_KEY_INFO_INSTALL){
			ret = -EINVAL;
			return ret;
		}
		ret = connection_handle_m1(local, eapol, eapol_len, key_info);
		return ret;
	}
	if (!(key_info & CONNECTION_EAPOL_KEY_INFO_INSTALL)){
		return ret;
	}
	ret = connection_handle_m3(local, eapol, eapol_len, key_info);
	return ret;
}

static inline int connection_finish_assoc(struct ieee80211_local *local, u16 aid){
	int ret = 0;
	struct connection_fsm *fsm = NULL;

	fsm = connection_get();
	ret = connection_notify_assoc(local, aid);

	if (ret){
		connection_set_state(fsm, CONNECTION_FAILED);
		return ret;
	}
	if (fsm->protected_network){
		connection_set_state(fsm, CONNECTION_WAIT_EAPOL_M1);
		return ret;
	}
	ret = connection_authorize(local);
	return ret;
}

static inline int connection_retry_active(struct ieee80211_local *local, const char *ssid, const char *password){
	int ret = 0;
	int ssid_len = 0;
	int password_len = 0;
	struct connection_fsm *fsm = NULL;
	struct ieee80211_channel *channel = NULL;

	fsm = connection_get();
	ssid_len = ssid != NULL ? strlen(ssid) : 0;
	password_len = password != NULL ? strlen(password) : 0;

	if (fsm == NULL){
		ret = -ENOENT;
		return ret;
	}
	if (ssid_len != fsm->ssid_len || memcmp(ssid, fsm->ssid, ssid_len) != 0 ||
	    password_len != fsm->passphrase_len ||
	    (password_len != 0 && memcmp(password, fsm->passphrase, password_len) != 0)){
		ret = -EBUSY;
		return ret;
	}
	channel = connection_find_channel(local->wiphy, fsm->center_freq);
	if (channel == NULL){
		ret = -EINVAL;
		return ret;
	}
	switch (fsm->state){
	case CONNECTION_AUTH_SENT:
		ret = wifi_connection_send_auth(local, channel);
		return ret;
	case CONNECTION_ASSOC_SENT:
		ret = connection_send_assoc(local, channel);
		return ret;
	case CONNECTION_WAIT_EAPOL_M3:
		if (fsm->m2_len != 0){
			ret = connection_send_eapol(local, fsm->m2, fsm->m2_len);
			return ret;
		}
		ret = -EAGAIN;
		return ret;
	case CONNECTION_WAIT_EAPOL_M1:
		ret = -EAGAIN;
		return ret;
	case CONNECTION_CONNECTED:
		ret = -EISCONN;
		return ret;
	case CONNECTION_FAILED:
		if (fsm->sta == NULL){
			ret = wifi_connection_send_auth(local, channel);
			return ret;
		}
		ret = -EBUSY;
		return ret;
	default:
		ret = -EBUSY;
		return ret;
	}
}

static inline int wifi_auth_connection_start(struct ieee80211_local *local, const char *ssid, const char *password){
	int ret = 0;
	int ssid_len = 0;
	int password_len = 0;
	struct connection_fsm *fsm = NULL;
	struct ieee80211_bss_conf *conf = NULL;
	struct exsul_scan_result *target = NULL;
	struct ieee80211_channel *channel = NULL;
	struct ieee80211_vif *connection_vif = NULL;
	const u8 local_addr[ETH_ALEN] = { 0x02, 0x00, 0x00, 0x52, 0xc0, 0x01 };

	connection_vif = connection_get_vif(local);
	password_len = password != NULL ? strlen(password) : 0;

	if (local == NULL || connection_vif == NULL || director.ieee80211_ops == NULL || ssid == NULL){
		ret = -EINVAL;
		return ret;
	}
	if (director.connection_fsm != NULL){
		ret = connection_retry_active(local, ssid, password);
		return ret;
	}

	ssid_len = strlen(ssid);
	if (ssid_len == 0 || ssid_len > IEEE80211_MAX_SSID_LEN){
		ret = -EINVAL;
		return ret;
	}

	target = connection_find_target(ssid);
	if (target == NULL){
		ret = -ENOENT;
		return ret;
	}
	if ((target->capability & WLAN_CAPABILITY_PRIVACY) && (password_len < 8 || password_len > 63)){
		ret = -EINVAL;
		return ret;
	}
	if (!(target->capability & WLAN_CAPABILITY_PRIVACY) && password_len != 0){
		ret = -EINVAL;
		return ret;
	}
	channel = connection_find_channel(local->wiphy, target->center_freq);
	if (channel == NULL || (channel->flags & IEEE80211_CHAN_DISABLED)){
		ret = -EINVAL;
		return ret;
	}

	fsm = calloc(1, sizeof(*fsm));
	if (fsm == NULL){
		ret = -ENOMEM;
		return ret;
	}
	director.connection_fsm = fsm;
	connection_set_state(fsm, CONNECTION_IDLE);
	fsm->protected_network = !!(target->capability & WLAN_CAPABILITY_PRIVACY);
	fsm->ssid_len = target->ssid_len;
	fsm->passphrase_len = password_len;
	fsm->center_freq = target->center_freq;
	fsm->capability = target->capability;
	memcpy(fsm->ssid, target->ssid, target->ssid_len);
	memcpy(fsm->bssid, target->bssid, ETH_ALEN);
	if (password_len != 0){
		memcpy(fsm->passphrase, password, password_len);
	}
	conf = &connection_vif->bss_conf;
	if (is_zero_ether_addr(connection_vif->addr)){
		memcpy(connection_vif->addr, local_addr, ETH_ALEN);
		memcpy(conf->addr, local_addr, ETH_ALEN);
	}
	conf->bssid = fsm->bssid;
	conf->beacon_int = target->beacon_interval;
	conf->assoc_capability = target->capability;
	conf->basic_rates = channel->band == NL80211_BAND_2GHZ ? 0xf : 0xff;
	ret = connection_configure_queues(local);
	if (ret){
		connection_set_state(fsm, CONNECTION_FAILED);
		return ret;
	}
	ret = connection_enable_management_rx(local);
	if (ret){
		connection_set_state(fsm, CONNECTION_FAILED);
		return ret;
	}
	ret = connection_leave_idle(local);
	if (ret){
		connection_set_state(fsm, CONNECTION_FAILED);
		return ret;
	}
	ret = connection_configure_channel(local, channel);
	if (ret){
		connection_set_state(fsm, CONNECTION_FAILED);
		return ret;
	}
	ret = connection_assign_chanctx(local);
	if (ret){
		connection_set_state(fsm, CONNECTION_FAILED);
		return ret;
	}
	connection_notify_pre_auth(local);
	ret = connection_program_target(local);
	if (ret){
		connection_set_state(fsm, CONNECTION_FAILED);
		return ret;
	}
	ret = connection_prepare_station(local, channel);
	if (ret){
		connection_set_state(fsm, CONNECTION_FAILED);
		return ret;
	}
	ret = wifi_connection_send_auth(local, channel);
	if (ret == 0){
		connection_start_retry_worker(local, fsm);
	}
	return ret;
}

static inline int connection_rx_observe(struct ieee80211_local *local, struct sk_buff *skb){
	int ret = 0;
	int hdr_len = 0;
	u16 fc = 0;
	u16 status = 0;
	u16 transaction = 0;
	const u8 *payload = NULL;

	struct connection_fsm *fsm = NULL;
	struct ieee80211_hdr *hdr = NULL;
	struct ieee80211_mgmt *mgmt = NULL;
	struct ieee80211_channel *channel = NULL;
	struct ieee80211_vif *connection_vif = NULL;
	static const u8 eapol_snap[] = { 0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00, 0x88, 0x8e };

	fsm = connection_get();
	connection_vif = connection_get_vif(local);
	if (fsm == NULL || local == NULL || connection_vif == NULL || skb == NULL || skb->data == NULL ||
	    skb->len < sizeof(struct ieee80211_hdr_3addr)){
		return ret;
	}
	hdr = (struct ieee80211_hdr *)skb->data;
	fc = le16_to_cpu(hdr->frame_control);
	if (ieee80211_is_mgmt(hdr->frame_control)){
		mgmt = (struct ieee80211_mgmt *)skb->data;
		if (memcmp(mgmt->bssid, fsm->bssid, ETH_ALEN) != 0 ||
		    memcmp(mgmt->sa, fsm->bssid, ETH_ALEN) != 0 ||
		    memcmp(mgmt->da, connection_vif->addr, ETH_ALEN) != 0){
			return ret;
		}
		channel = connection_find_channel(local->wiphy, fsm->center_freq);
		if (channel == NULL){
			ret = -EINVAL;
			return ret;
		}
		if ((fsm->state == CONNECTION_AUTH_SENT || fsm->state == CONNECTION_ASSOC_SENT) &&
		    ieee80211_is_auth(mgmt->frame_control)){
			if (skb->len < offsetof(struct ieee80211_mgmt, u.auth.variable)){
				ret = -EINVAL;
				return ret;
			}
			transaction = le16_to_cpu(mgmt->u.auth.auth_transaction);
			status = le16_to_cpu(mgmt->u.auth.status_code);
			if (le16_to_cpu(mgmt->u.auth.auth_alg) != 0 || transaction != 2 || status != WLAN_STATUS_SUCCESS){
				connection_set_state(fsm, CONNECTION_FAILED);
				ret = -EACCES;
				return ret;
			}
			#ifdef ENABLE_DBG_PRINT
			printf("[*] authentication response accepted%s\n", fsm->state == CONNECTION_ASSOC_SENT ? "; retransmitting association request" : "");
			#endif
			ret = connection_send_assoc(local, channel);
			if (ret){
				connection_set_state(fsm, CONNECTION_FAILED);
			}
			return ret;
		}
		if (fsm->state == CONNECTION_ASSOC_SENT && ieee80211_is_assoc_resp(mgmt->frame_control)){
			if (skb->len < offsetof(struct ieee80211_mgmt, u.assoc_resp.variable)){
				ret = -EINVAL;
				return ret;
			}
			status = le16_to_cpu(mgmt->u.assoc_resp.status_code);
			if (status != WLAN_STATUS_SUCCESS){
				connection_set_state(fsm, CONNECTION_FAILED);
				ret = -EACCES;
				return ret;
			}

			ret = connection_finish_assoc(local, le16_to_cpu(mgmt->u.assoc_resp.aid) & 0x3fff);
			if (ret){
				connection_set_state(fsm, CONNECTION_FAILED);
			}
			return ret;
		}
		if ((fc & IEEE80211_FCTL_STYPE) == IEEE80211_STYPE_DEAUTH ||
		    (fc & IEEE80211_FCTL_STYPE) == IEEE80211_STYPE_DISASSOC){
			connection_set_state(fsm, CONNECTION_FAILED);
		}
		return ret;
	}
	if (!ieee80211_is_data(hdr->frame_control) ||
	    (fc & (IEEE80211_FCTL_TODS | IEEE80211_FCTL_FROMDS)) != IEEE80211_FCTL_FROMDS ||
	    memcmp(hdr->addr1, connection_vif->addr, ETH_ALEN) != 0 ||
	    memcmp(hdr->addr2, fsm->bssid, ETH_ALEN) != 0){
		return ret;
	}
	hdr_len = ieee80211_hdrlen(hdr->frame_control);
	if (hdr_len > skb->len || skb->len - hdr_len < sizeof(eapol_snap) + 4){
		return ret;
	}
	payload = skb->data + hdr_len;
	if (memcmp(payload, eapol_snap, sizeof(eapol_snap)) != 0){
		return ret;
	}
	ret = connection_handle_eapol(local, payload + sizeof(eapol_snap), skb->len - hdr_len - sizeof(eapol_snap));
	return ret;
}

/* Start a new scan generation while retaining recently observed BSS entries. */
static inline void scan_cache_reset(void){
	pthread_mutex_lock(&director.scan_cache_lock);
	atomic_store(&director.scan_result_epoch, 0);
	atomic_fetch_add(&director.scan_generation, 1);
	pthread_mutex_unlock(&director.scan_cache_lock);
}

static inline const u8 *exsul_find_ie(u8 eid, const u8 *ies, size_t ielen, u8 *elen){
	const u8 *pos = ies;

	if (elen != NULL){
		*elen = 0;
	}
	while (ielen >= 2) {
		u8 id = pos[0];
		u8 len = pos[1];

		pos += 2;
		ielen -= 2;
		if (len > ielen){
			break;
		}
		if (id == eid) {
			if (elen != NULL){
				*elen = len;
			}
			return pos;
		}
		pos += len;
		ielen -= len;
	}
	return NULL;
}

static inline void scan_cache_observe(struct ieee80211_local *local,
                                      struct sk_buff *skb,
                                      struct ieee80211_channel *channel,
                                      int fixed_hdr_len){
	u8 ds_len = 0;
	u8 ssid_len = 0;
	const u8 *ds = NULL;
	const u8 *ies = NULL;
	const u8 *ssid = NULL;
	struct ieee80211_mgmt *mgmt = NULL;
	struct exsul_scan_result *entry = NULL;
	struct ieee80211_rx_status *rx_status = NULL;
	u32 generation = 0;

	generation = atomic_load(&director.scan_generation);
	if (local == NULL || skb == NULL || skb->data == NULL || channel == NULL || fixed_hdr_len < 0){
		goto end;
	}
	if (skb->len < fixed_hdr_len || skb->len > 4096){
		goto end;
	}

	mgmt = (struct ieee80211_mgmt *)skb->data;
	if (!ieee80211_is_probe_resp(mgmt->frame_control) && !ieee80211_is_beacon(mgmt->frame_control)){
		goto end;
	}

	rx_status = IEEE80211_SKB_RXCB(skb);
	ies = skb->data + fixed_hdr_len;

	ssid = exsul_find_ie(WLAN_EID_SSID, ies, skb->len - fixed_hdr_len, &ssid_len);
	ds = exsul_find_ie(WLAN_EID_DS_PARAMS, ies, skb->len - fixed_hdr_len, &ds_len);

	pthread_mutex_lock(&director.scan_cache_lock);
	for (u32 i = 0; i < EXSUL_MAX_SCAN_RESULTS; i++) {
		if (director.scan_results[i].valid &&
		    memcmp(director.scan_results[i].bssid, mgmt->bssid, ETH_ALEN) == 0) {
			entry = &director.scan_results[i];
			break;
		}
	}
	if (entry == NULL) {
		for (u32 i = 0; i < EXSUL_MAX_SCAN_RESULTS; i++) {
			if (!director.scan_results[i].valid) {
				entry = &director.scan_results[i];
				director.scan_result_count++;
				break;
			}
		}
		if (entry == NULL) {
			entry = &director.scan_results[0];
			for (u32 i = 1; i < EXSUL_MAX_SCAN_RESULTS; i++) {
				if (director.scan_results[i].last_seen_generation < entry->last_seen_generation)
					entry = &director.scan_results[i];
			}
		}
		memset(entry, 0, sizeof(*entry));
		entry->valid = true;
		memcpy(entry->bssid, mgmt->bssid, ETH_ALEN);
	}
	if (entry == NULL){
		goto unlock;
	}
	if (ssid != NULL && ssid_len > 0 && ssid_len <= EXSUL_MAX_SCAN_SSID_LEN) {
		memcpy(entry->ssid, ssid, ssid_len);
		entry->ssid_len = ssid_len;
	}
	if (ieee80211_is_probe_resp(mgmt->frame_control)){
		entry->from_probe_resp = true;
	}
	entry->beacon_interval = le16_to_cpu(mgmt->u.beacon.beacon_int);
	entry->capability = le16_to_cpu(mgmt->u.beacon.capab_info);
	entry->center_freq = channel->center_freq;
	if (ds != NULL && ds_len >= 1){
		entry->channel = ds[0];
	}
	if (rx_status != NULL && !(rx_status->flag & RX_FLAG_NO_SIGNAL_VAL)){
		entry->signal = rx_status->signal;
	}
	entry->seen++;
	entry->last_seen_generation = generation;
	atomic_fetch_add(&director.scan_result_epoch, 1);

	unlock:
	pthread_mutex_unlock(&director.scan_cache_lock);
	end:
}

static inline void scan_cache_observe_rx_status(struct ieee80211_local *local, struct sk_buff *skb){
	int fixed_hdr_len = 0;
	struct wiphy *wiphy = NULL;
	struct ieee80211_mgmt *mgmt = NULL;
	struct ieee80211_channel *channel = NULL;
	struct ieee80211_rx_status *rx_status = NULL;

	fixed_hdr_len = offsetof(struct ieee80211_mgmt, u.probe_resp.variable);
	if (local == NULL || skb == NULL || skb->data == NULL || skb->len < IEEE80211_MIN_HDR_LEN){
		goto end;
	}
	if (skb->len < fixed_hdr_len || skb->len > 4096){
		goto end;
	}
	mgmt = (struct ieee80211_mgmt *)skb->data;
	if (!ieee80211_is_probe_resp(mgmt->frame_control) && !ieee80211_is_beacon(mgmt->frame_control)){
		goto end;
	}
	rx_status = IEEE80211_SKB_RXCB(skb);
	if (rx_status == NULL || rx_status->freq == 0){
		goto end;
	}
	wiphy = local->hw.wiphy != NULL ? local->hw.wiphy : local->wiphy;
	if (wiphy == NULL){
		goto end;
	}
	channel = ieee80211_get_channel_khz(wiphy, ieee80211_rx_status_to_khz(rx_status));
	if (channel == NULL || (channel->flags & IEEE80211_CHAN_DISABLED)){
		goto end;
	}
	scan_cache_observe(local, skb, channel, fixed_hdr_len);
	end:
}

/* print the scan results */
static inline void scan_cache_print(void){
	int printed = 0;
	u32 current_generation = 0;
	struct exsul_scan_result *entry = NULL;

	current_generation = atomic_load(&director.scan_generation);
	pthread_mutex_lock(&director.scan_cache_lock);
	for (u32 i = 0; i < EXSUL_MAX_SCAN_RESULTS; i++) {
		entry = &director.scan_results[i];

		if (!entry->valid){
			continue;
		}

		printf("[%03u] %02x:%02x:%02x:%02x:%02x:%02x  ",
		       printed,
		       entry->bssid[0], entry->bssid[1], entry->bssid[2],
		       entry->bssid[3], entry->bssid[4], entry->bssid[5]);
		printf("ssid=\"");
		if (entry->ssid_len == 0) {
			printf("<hidden>");
		}else{
			for (u8 j = 0; j < entry->ssid_len; j++) {
				u8 c = entry->ssid[j];

				if (c >= 32 && c <= 126){
					putchar(c);
				}else{
					putchar('.');
				}
			}
		}
		printf("\" freq=%uMHz", entry->center_freq);
		if (entry->channel != 0){
			printf(" channel=%u", entry->channel);
		}
		printf(" signal=%ddBm\n", entry->signal);
		printed++;
	}
	pthread_mutex_unlock(&director.scan_cache_lock);
}

struct scan_cache_print_job {
	u32 generation;
};

static void *scan_cache_print_worker(void *arg){
	struct scan_cache_print_job *job = arg;
	u32 generation = job != NULL ? job->generation : 0;
	u32 last_epoch = atomic_load(&director.scan_result_epoch);
	unsigned int stable_intervals = 0;

	free(job);
	/* Wait for 500 ms of cache quiescence; stop waiting after two seconds. */
	for (unsigned int i = 0; i < 20; i++){
		u32 epoch;

		usleep(100000);
		if (atomic_load(&director.scan_generation) != generation){
			return NULL;
		}

		epoch = atomic_load(&director.scan_result_epoch);
		if (epoch == last_epoch){
			if (++stable_intervals >= 5){
				break;
			}
		}else{
			last_epoch = epoch;
			stable_intervals = 0;
		}
	}

	if (atomic_load(&director.scan_generation) == generation &&
	    !atomic_load(&director.scan_in_progress)){
		scan_cache_print();
	}
	return NULL;
}

static inline void scan_cache_schedule_print(void){
	struct scan_cache_print_job *job = calloc(1, sizeof(*job));
	pthread_t thread;

	if (job == NULL){
		scan_cache_print();
		goto end;
	}
	job->generation = atomic_load(&director.scan_generation);
	if (pthread_create(&thread, NULL, scan_cache_print_worker, job) != 0){
		free(job);
		scan_cache_print();
		goto end;
	}
	#ifdef __linux__
	pthread_detach(thread);
	#endif
	end:
}

#define SCAN_COMPLETE_GRACE_USEC 200000

struct scan_complete_job {
	u32 generation;
};

static void *scan_complete_worker(void *arg){
	struct scan_complete_job *job = arg;
	u32 generation = job != NULL ? job->generation : 0;

	free(job);
	usleep(SCAN_COMPLETE_GRACE_USEC);
	if (atomic_load(&director.scan_generation) == generation &&
	    atomic_exchange(&director.scan_in_progress, false)){
		scan_cache_schedule_print();
	}
	return NULL;
}

static inline void scan_complete_defer(void){
	struct scan_complete_job *job = calloc(1, sizeof(*job));
	pthread_t thread;

	if (job == NULL){
		if (atomic_exchange(&director.scan_in_progress, false)){
			scan_cache_schedule_print();
		}
		return;
	}
	job->generation = atomic_load(&director.scan_generation);
	if (pthread_create(&thread, NULL, scan_complete_worker, job) != 0){
		free(job);
		if (atomic_exchange(&director.scan_in_progress, false)){
			scan_cache_schedule_print();
		}
		return;
	}
	#ifdef __linux__
	pthread_detach(thread);
	#endif
}

struct scan_watchdog_job {
	u32 generation;
	struct ieee80211_local *local;
	void (*cancel_hw_scan)(struct ieee80211_hw *, struct ieee80211_vif *);
};

static void *scan_watchdog_worker(void *arg){
	struct scan_watchdog_job *job = arg;
	u32 generation = job != NULL ? job->generation : 0;
	struct ieee80211_local *local = job != NULL ? job->local : NULL;
	void (*cancel_hw_scan)(struct ieee80211_hw *, struct ieee80211_vif *) =
		job != NULL ? job->cancel_hw_scan : NULL;

	free(job);
	for (unsigned int i = 0; i < 150; i++){
		usleep(100000);
		if (atomic_load(&director.scan_generation) != generation ||
		    !atomic_load(&director.scan_in_progress))
			return NULL;
	}
	if (local == NULL || atomic_load(&director.scan_generation) != generation ||
	    !atomic_load(&director.scan_in_progress))
		return NULL;

	#ifdef ENABLE_DBG_PRINT
	printf("[!] hw_scan completion timed out; cancelling the driver scan\n");
	#endif
	if (cancel_hw_scan != NULL && local->ieee80211_vif != NULL){
		cancel_hw_scan(local->ieee80211_hw, local->ieee80211_vif);
	}
	if (atomic_exchange(&director.scan_in_progress, false)){
		scan_cache_schedule_print();
	}
	return NULL;
}

static inline void scan_watchdog_start(struct ieee80211_local *local, void (*cancel_hw_scan)(struct ieee80211_hw *, struct ieee80211_vif *)){
	struct scan_watchdog_job *job = calloc(1, sizeof(*job));
	pthread_t thread;

	if (job == NULL){
		goto end;
	}
	job->generation = atomic_load(&director.scan_generation);
	job->local = local;
	job->cancel_hw_scan = cancel_hw_scan;
	if (pthread_create(&thread, NULL, scan_watchdog_worker, job) != 0){
		free(job);
		goto end;
	}
	#ifdef __linux__
	pthread_detach(thread);
	#endif
	end:
}

/*
 * ieee80211_channel_to_khz - convert ieee80211_channel to frequency in KHz
 * @chan: struct ieee80211_channel to convert
 * Return: The corresponding frequency (in KHz)
 */
static inline u32 ieee80211_channel_to_khz(const struct ieee80211_channel *chan){
	return MHZ_TO_KHZ(chan->center_freq) + chan->freq_offset;
}

/* avoid any debugfs function */
static inline void rate_control_add_debugfs(struct ieee80211_local *local){
	/* do nothing */
}

static inline void debugfs_hw_add(struct ieee80211_local *local){
	/* do nothing */
}

/* same for leds... */
static inline void ieee80211_led_assoc(struct ieee80211_local *local, bool associated){
	/* do nothing */
}

static inline void ieee80211_led_radio(struct ieee80211_local *local, bool enabled){
	/* do nothing */
}

static inline void ieee80211_alloc_led_names(struct ieee80211_local *local){
	/* do nothing */
}

static inline void ieee80211_free_led_names(struct ieee80211_local *local){
	/* do nothing */
}

static inline void ieee80211_led_init(struct ieee80211_local *local){
	/* do nothing */
}

static inline void ieee80211_led_exit(struct ieee80211_local *local){
	/* do nothing */
}

static inline void ieee80211_mod_tpt_led_trig(struct ieee80211_local *local, unsigned int types_on, unsigned int types_off){
	/* do nothing */
}

static inline bool _ieee80211_hw_check(struct ieee80211_hw *hw, enum ieee80211_hw_flags flg){
	return test_bit(flg, hw->flags);
}
#define ieee80211_hw_check(hw, flg)	_ieee80211_hw_check(hw, IEEE80211_HW_##flg)

static inline void _ieee80211_hw_set(struct ieee80211_hw *hw, enum ieee80211_hw_flags flg){
	struct ieee80211_emulated_hw *ehw = (struct ieee80211_emulated_hw *)hw;
	__set_bit(flg, ehw->flags);
}
#define ieee80211_hw_set(hw, flg)	_ieee80211_hw_set(hw, IEEE80211_HW_##flg)

static inline void set_sta_flag(struct sta_info *sta, enum ieee80211_sta_info_flags flag){
	set_bit(flag, &sta->_flags);
}

static inline void clear_sta_flag(struct sta_info *sta, enum ieee80211_sta_info_flags flag){
	clear_bit(flag, &sta->_flags);
}

static inline int test_sta_flag(struct sta_info *sta, enum ieee80211_sta_info_flags flag){
	return test_bit(flag, &sta->_flags);
}

static inline int test_and_clear_sta_flag(struct sta_info *sta, enum ieee80211_sta_info_flags flag){
	return test_and_clear_bit(flag, &sta->_flags);
}

static inline int test_and_set_sta_flag(struct sta_info *sta, enum ieee80211_sta_info_flags flag){
	return test_and_set_bit(flag, &sta->_flags);
}

static inline bool ieee80211_sdata_running(struct ieee80211_sub_if_data *sdata){
	return test_bit(SDATA_STATE_RUNNING, &sdata->state);
}

/*
 * wiphy_name - get wiphy name
 *
 * @wiphy: The wiphy whose name to return
 * Return: The name of @wiphy.
 */
static inline const char *wiphy_name(const struct wiphy *wiphy){
	struct device *dev = NULL;

	dev = &wiphy->dev;

	/* inline here 'dev_name' for simplicity */
	if (dev->init_name != NULL){
		return dev->init_name;
	}

	return kobject_name(&dev->kobj);

}

/*
 * wiphy_lock - lock the wiphy
 * @wiphy: the wiphy to lock
 *
 * This is needed around registering and unregistering netdevs that
 * aren't created through cfg80211 calls, since that requires locking
 * in cfg80211 when the notifiers is called, but that cannot
 * differentiate which way it's called.
 *
 * It can also be used by drivers for their own purposes.
 *
 * When cfg80211 ops are called, the wiphy is already locked.
 *
 * Note that this makes sure that no workers that have been queued
 * with wiphy_queue_work() are running.
 */
static inline void wiphy_lock(struct wiphy *wiphy){
	mutex_lock(&wiphy->mtx);
}

/*
 * wiphy_unlock - unlock the wiphy again
 * @wiphy: the wiphy to unlock
 */
static inline void wiphy_unlock(struct wiphy *wiphy){
	mutex_unlock(&wiphy->mtx);
}

/*
 * wiphy_ext_feature_set - set the extended feature flag
 *
 * @wiphy: the wiphy to modify.
 * @ftidx: extended feature bit index.
 *
 * The extended features are flagged in multiple bytes (see
 * &struct wiphy.@ext_features)
 */
static inline void wiphy_ext_feature_set(struct wiphy *wiphy, enum nl80211_ext_feature_index ftidx){
	u8 *ft_byte = 0;

	ft_byte = &wiphy->ext_features + (sizeof(u8) * (ftidx / 8));
	*ft_byte |= BIT(ftidx % 8);
}

static inline u16 ieee80211_info_get_tx_time_est(struct ieee80211_tx_info *info){
	return info->tx_time_est << 2;
}

static inline bool ieee80211_rate_valid(struct ieee80211_tx_rate *rate){
	return rate->idx >= 0 && rate->count > 0;
}

static inline void ieee80211_rate_set_vht(struct ieee80211_tx_rate *rate, u8 mcs, u8 nss){
	rate->idx = ((nss - 1) << 4) | mcs;
}

static inline u8 ieee80211_rate_get_vht_mcs(const struct ieee80211_tx_rate *rate){
	return rate->idx & 0xF;
}

static inline u8 ieee80211_rate_get_vht_nss(const struct ieee80211_tx_rate *rate){
	return (rate->idx >> 4) + 1;
}

/*
 * wiphy_ext_feature_isset - check the extended feature flag
 *
 * @wiphy: the wiphy to modify.
 * @ftidx: extended feature bit index.
 *
 * The extended features are flagged in multiple bytes (see
 * &struct wiphy.@ext_features)
 *
 * Return: %true if extended feature flag is set, %false otherwise
 */
static inline bool wiphy_ext_feature_isset(struct wiphy *wiphy, enum nl80211_ext_feature_index ftidx){
	u8 ft_byte = 0;

	ft_byte = wiphy->ext_features + (sizeof(u8) * (ftidx / 8));
	return (ft_byte & BIT(ftidx % 8)) != 0;
}

/*
 * ieee80211_has_tods - check if IEEE80211_FCTL_TODS is set
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame has to-DS set
 */
static inline bool ieee80211_has_tods(__le16 fc){
	return (fc & cpu_to_le16(IEEE80211_FCTL_TODS)) != 0;
}

/*
 * ieee80211_has_fromds - check if IEEE80211_FCTL_FROMDS is set
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame has from-DS set
 */
static inline bool ieee80211_has_fromds(__le16 fc){
	return (fc & cpu_to_le16(IEEE80211_FCTL_FROMDS)) != 0;
}

/*
 * ieee80211_has_a4 - check if IEEE80211_FCTL_TODS and IEEE80211_FCTL_FROMDS are set
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not it's a 4-address frame (from-DS and to-DS set)
 */
static inline bool ieee80211_has_a4(__le16 fc){
	__le16 tmp = cpu_to_le16(IEEE80211_FCTL_TODS | IEEE80211_FCTL_FROMDS);
	return (fc & tmp) == tmp;
}

/*
 * ieee80211_has_morefrags - check if IEEE80211_FCTL_MOREFRAGS is set
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame has more fragments (more frags bit set)
 */
static inline bool ieee80211_has_morefrags(__le16 fc){
	return (fc & cpu_to_le16(IEEE80211_FCTL_MOREFRAGS)) != 0;
}

/*
 * ieee80211_has_retry - check if IEEE80211_FCTL_RETRY is set
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the retry flag is set
 */
static inline bool ieee80211_has_retry(__le16 fc){
	return (fc & cpu_to_le16(IEEE80211_FCTL_RETRY)) != 0;
}

/*
 * ieee80211_has_pm - check if IEEE80211_FCTL_PM is set
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the power management flag is set
 */
static inline bool ieee80211_has_pm(__le16 fc){
	return (fc & cpu_to_le16(IEEE80211_FCTL_PM)) != 0;
}

/*
 * ieee80211_has_moredata - check if IEEE80211_FCTL_MOREDATA is set
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the more data flag is set
 */
static inline bool ieee80211_has_moredata(__le16 fc){
	return (fc & cpu_to_le16(IEEE80211_FCTL_MOREDATA)) != 0;
}

/*
 * ieee80211_has_protected - check if IEEE80211_FCTL_PROTECTED is set
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the protected flag is set
 */
static inline bool ieee80211_has_protected(__le16 fc){
	return (fc & cpu_to_le16(IEEE80211_FCTL_PROTECTED)) != 0;
}

/*
 * ieee80211_has_order - check if IEEE80211_FCTL_ORDER is set
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the order flag is set
 */
static inline bool ieee80211_has_order(__le16 fc){
	return (fc & cpu_to_le16(IEEE80211_FCTL_ORDER)) != 0;
}

/*
 * ieee80211_s1g_has_next_tbtt - check if IEEE80211_S1G_BCN_NEXT_TBTT
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame contains the variable-length
 *	next TBTT field
 */
static inline bool ieee80211_s1g_has_next_tbtt(__le16 fc){
	return ieee80211_is_s1g_beacon(fc) && (fc & cpu_to_le16(IEEE80211_S1G_BCN_NEXT_TBTT));
}

/*
 * ieee80211_s1g_has_ano - check if IEEE80211_S1G_BCN_ANO
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame contains the variable-length
 *	ANO field
 */
static inline bool ieee80211_s1g_has_ano(__le16 fc){
	return ieee80211_is_s1g_beacon(fc) && (fc & cpu_to_le16(IEEE80211_S1G_BCN_ANO));
}

/*
 * ieee80211_s1g_has_cssid - check if IEEE80211_S1G_BCN_CSSID
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame contains the variable-length
 *	compressed SSID field
 */
static inline bool ieee80211_s1g_has_cssid(__le16 fc){
	return ieee80211_is_s1g_beacon(fc) && (fc & cpu_to_le16(IEEE80211_S1G_BCN_CSSID));
}

/*
 * ieee80211_is_atim - check if IEEE80211_FTYPE_MGMT && IEEE80211_STYPE_ATIM
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame is an ATIM frame
 */
static inline bool ieee80211_is_atim(__le16 fc){
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_ATIM);
}

/*
 * ieee80211_is_disassoc - check if IEEE80211_FTYPE_MGMT && IEEE80211_STYPE_DISASSOC
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame is a disassociation frame
 */
static inline bool ieee80211_is_disassoc(__le16 fc){
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_DISASSOC);
}

/*
 * ieee80211_is_auth - check if IEEE80211_FTYPE_MGMT && IEEE80211_STYPE_AUTH
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame is an authentication frame
 */
static inline bool ieee80211_is_auth(__le16 fc){
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_AUTH);
}

/*
 * ieee80211_is_deauth - check if IEEE80211_FTYPE_MGMT && IEEE80211_STYPE_DEAUTH
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame is a deauthentication frame
 */
static inline bool ieee80211_is_deauth(__le16 fc){
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_DEAUTH);
}

/*
 * ieee80211_is_action - check if IEEE80211_FTYPE_MGMT && IEEE80211_STYPE_ACTION
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame is an action frame
 */
static inline bool ieee80211_is_action(__le16 fc){
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_ACTION);
}

/*
 * ieee80211_is_back_req - check if IEEE80211_FTYPE_CTL && IEEE80211_STYPE_BACK_REQ
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame is a block-ACK request frame
 */
static inline bool ieee80211_is_back_req(__le16 fc){
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_BACK_REQ);
}

/*
 * ieee80211_is_back - check if IEEE80211_FTYPE_CTL && IEEE80211_STYPE_BACK
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame is a block-ACK frame
 */
static inline bool ieee80211_is_back(__le16 fc){
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_BACK);
}

/*
 * ieee80211_is_pspoll - check if IEEE80211_FTYPE_CTL && IEEE80211_STYPE_PSPOLL
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame is a PS-poll frame
 */
static inline bool ieee80211_is_pspoll(__le16 fc){
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_PSPOLL);
}

/*
 * ieee80211_is_rts - check if IEEE80211_FTYPE_CTL && IEEE80211_STYPE_RTS
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame is an RTS frame
 */
static inline bool ieee80211_is_rts(__le16 fc){
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_RTS);
}

/*
 * ieee80211_is_cts - check if IEEE80211_FTYPE_CTL && IEEE80211_STYPE_CTS
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame is a CTS frame
 */
static inline bool ieee80211_is_cts(__le16 fc){
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_CTS);
}

/*
 * ieee80211_is_ack - check if IEEE80211_FTYPE_CTL && IEEE80211_STYPE_ACK
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame is an ACK frame
 */
static inline bool ieee80211_is_ack(__le16 fc){
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_ACK);
}

/*
 * ieee80211_is_cfend - check if IEEE80211_FTYPE_CTL && IEEE80211_STYPE_CFEND
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame is a CF-end frame
 */
static inline bool ieee80211_is_cfend(__le16 fc){
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_CFEND);
}

/*
 * ieee80211_is_cfendack - check if IEEE80211_FTYPE_CTL && IEEE80211_STYPE_CFENDACK
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame is a CF-end-ack frame
 */
static inline bool ieee80211_is_cfendack(__le16 fc){
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_CFENDACK);
}

/*
 * ieee80211_is_nullfunc - check if frame is a regular (non-QoS) nullfunc frame
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame is a nullfunc frame
 */
static inline bool ieee80211_is_nullfunc(__le16 fc){
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == cpu_to_le16(IEEE80211_FTYPE_DATA | IEEE80211_STYPE_NULLFUNC);
}

/*
 * ieee80211_is_qos_nullfunc - check if frame is a QoS nullfunc frame
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame is a QoS nullfunc frame
 */
static inline bool ieee80211_is_qos_nullfunc(__le16 fc){
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == cpu_to_le16(IEEE80211_FTYPE_DATA | IEEE80211_STYPE_QOS_NULLFUNC);
}

/*
 * ieee80211_is_trigger - check if frame is trigger frame
 * @fc: frame control field in little-endian byteorder
 * Return: whether or not the frame is a trigger frame
 */
static inline bool ieee80211_is_trigger(__le16 fc){
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_TRIGGER);
}

/*
 * ieee80211_is_any_nullfunc - check if frame is regular or QoS nullfunc frame
 * @fc: frame control bytes in little-endian byteorder
 * Return: whether or not the frame is a nullfunc or QoS nullfunc frame
 */
static inline bool ieee80211_is_any_nullfunc(__le16 fc){
	return (ieee80211_is_nullfunc(fc) || ieee80211_is_qos_nullfunc(fc));
}

/*
 * ieee80211_is_first_frag - check if IEEE80211_SCTL_FRAG is not set
 * @seq_ctrl: frame sequence control bytes in little-endian byteorder
 * Return: whether or not the frame is the first fragment (also true if
 *	it's not fragmented at all)
 */
static inline bool ieee80211_is_first_frag(__le16 seq_ctrl){
	return (seq_ctrl & cpu_to_le16(IEEE80211_SCTL_FRAG)) == 0;
}

/*
 * ieee80211_is_frag - check if a frame is a fragment
 * @hdr: 802.11 header of the frame
 * Return: whether or not the frame is a fragment
 */
static inline bool ieee80211_is_frag(struct ieee80211_hdr *hdr){
	return ieee80211_has_morefrags(hdr->frame_control) || hdr->seq_ctrl & cpu_to_le16(IEEE80211_SCTL_FRAG);
}

/*
 * ieee80211_s1g_optional_len - determine length of optional S1G beacon fields
 * @fc: frame control bytes in little-endian byteorder
 * Return: total length in bytes of the optional fixed-length fields
 *
 * S1G beacons may contain up to three optional fixed-length fields that
 * precede the variable-length elements. Whether these fields are present
 * is indicated by flags in the frame control field.
 *
 * From IEEE 802.11-2024 section 9.3.4.3:
 *  - Next TBTT field may be 0 or 3 bytes
 *  - Short SSID field may be 0 or 4 bytes
 *  - Access Network Options (ANO) field may be 0 or 1 byte
 */
static inline int ieee80211_s1g_optional_len(__le16 fc){
	int len = 0;

	if (ieee80211_s1g_has_next_tbtt(fc)){
		len += 3;
	}
	if (ieee80211_s1g_has_cssid(fc)){
		len += 4;
	}
	if (ieee80211_s1g_has_ano(fc)){
		len += 1;
	}
	return len;
}

/*
 * ieee80211_get_qos_ctl - get pointer to qos control bytes
 * @hdr: the frame
 * Return: a pointer to the QoS control field in the frame header
 *
 * The qos ctrl bytes come after the frame_control, duration, seq_num
 * and 3 or 4 addresses of length ETH_ALEN. Checks frame_control to choose
 * between struct ieee80211_qos_hdr_4addr and struct ieee80211_qos_hdr.
 */
static inline u8 *ieee80211_get_qos_ctl(struct ieee80211_hdr *hdr){
	union {
		struct ieee80211_qos_hdr	addr3;
		struct ieee80211_qos_hdr_4addr	addr4;
	} *qos;

	qos = (void *)hdr;
	if (ieee80211_has_a4(qos->addr3.frame_control)){
		return (u8 *)&qos->addr4.qos_ctrl;
	}else{
		return (u8 *)&qos->addr3.qos_ctrl;
	}
}

/*
 * ieee80211_get_tid - get qos TID
 * @hdr: the frame
 * Return: the TID from the QoS control field
 */
static inline u8 ieee80211_get_tid(struct ieee80211_hdr *hdr){
	u8 *qc = NULL;

	qc = ieee80211_get_qos_ctl(hdr);
	return qc[0] & IEEE80211_QOS_CTL_TID_MASK;
}

/*
 * ieee80211_get_SA - get pointer to SA
 * @hdr: the frame
 * Return: a pointer to the source address (SA)
 *
 * Given an 802.11 frame, this function returns the offset
 * to the source address (SA). It does not verify that the
 * header is long enough to contain the address, and the
 * header must be long enough to contain the frame control
 * field.
 */
static inline u8 *ieee80211_get_SA(struct ieee80211_hdr *hdr){
	if (ieee80211_has_a4(hdr->frame_control)){
		return hdr->addr4;
	}
	if (ieee80211_has_fromds(hdr->frame_control)){
		return hdr->addr3;
	}
	return hdr->addr2;
}

/*
 * ieee80211_get_DA - get pointer to DA
 * @hdr: the frame
 * Return: a pointer to the destination address (DA)
 *
 * Given an 802.11 frame, this function returns the offset
 * to the destination address (DA). It does not verify that
 * the header is long enough to contain the address, and the
 * header must be long enough to contain the frame control
 * field.
 */
static inline u8 *ieee80211_get_DA(struct ieee80211_hdr *hdr){
	if (ieee80211_has_tods(hdr->frame_control)){
		return hdr->addr3;
	}else{
		return hdr->addr1;
	}
}

static bool is_multicast_ether_addr(const u8 *addr){
	return addr && (addr[0] & 0x01);
}

static bool is_broadcast_ether_addr(const u8 *addr){
	if (addr == NULL){
		return false;
	}
	for (int i = 0; i < ETH_ALEN; i++) {
		if (addr[i] != 0xff){
			return false;
		}
	}
	return true;
}

static bool is_zero_ether_addr(const u8 *addr){
	if (addr == NULL){
		return true;
	}
	for (int i = 0; i < ETH_ALEN; i++) {
		if (addr[i] != 0){
			return false;
		}
	}
	return true;
}

static bool ether_addr_equal(const u8 *addr1, const u8 *addr2){
	if (addr1 == NULL || addr2 == NULL){
		return false;
	}
	return memcmp(addr1, addr2, ETH_ALEN) == 0;
}

static bool ieee80211_accept_frame(struct ieee80211_rx_data *rx){
	bool multicast = false;
	const u8 *da = NULL;
	const u8 *sa = NULL;
	struct sk_buff *skb = NULL;
	struct ieee80211_vif *vif = NULL;
	struct ieee80211_hdr *hdr = NULL;

	if (rx == NULL || rx->skb == NULL || rx->sdata == NULL || rx->sdata->vif == NULL){
		return false;
	}

	skb = rx->skb;
	hdr = (struct ieee80211_hdr *)skb->data;
	vif = rx->sdata->vif;

	if (skb->len < 10){ /* minimun length for the ieee80211 header */
		return false;
	}

	/*
	 * The userspace environment does not always know the final interface
	 * address while replaying a driver.  If the virtual interface address
	 * is still zero, keep the frame instead of applying a kernel-style
	 * address filter that would drop all unicast traffic.
	 */
	if (is_zero_ether_addr(vif->addr)){
		return true;
	}

	da = ieee80211_get_DA(hdr);
	sa = ieee80211_get_SA(hdr);
	multicast = is_multicast_ether_addr(hdr->addr1) || is_broadcast_ether_addr(hdr->addr1) || ieee80211_is_s1g_beacon(hdr->frame_control);

	switch (vif->type) {
	case NL80211_IFTYPE_MONITOR:
	case NL80211_IFTYPE_AP_VLAN:
		return false;
	case NL80211_IFTYPE_STATION:
		if (multicast){
			return true;
		}
		return ether_addr_equal(hdr->addr1, vif->addr) || ether_addr_equal(da, vif->addr);
	case NL80211_IFTYPE_AP:
	case NL80211_IFTYPE_P2P_GO:
		if (ether_addr_equal(sa, vif->addr)){
			return false;
		}
		if (multicast){
			return true;
		}
		return ether_addr_equal(hdr->addr1, vif->addr) || ieee80211_has_tods(hdr->frame_control);
	default:
		if (multicast){
			return true;
		}
		return ether_addr_equal(hdr->addr1, vif->addr) || ether_addr_equal(da, vif->addr);
	}
}

static struct rate_control_ref *rate_control_alloc(const char *name, struct ieee80211_local *local){
	struct rate_control_ref *ref = NULL;

	ref = malloc(sizeof(struct rate_control_ref));
	if (ref == NULL){
		return NULL;
	}

	//ref->rops = ieee80211_rate_control_ops_get(name);
	if (ref->rops == NULL){
		goto free;
	}

	ref->priv = ref->rops->alloc(&local->hw);
	if (ref->priv == NULL){
		goto free;
	}

	return ref;

	free:
	kfree(ref);
	return NULL;
}

static int ieee80211_init_rate_ctrl_alg(struct ieee80211_local *local, const char *name){
	int ret = 0;
	struct rate_control_ref *ref = NULL;
	struct ieee80211_ops *ieee80211_ops = NULL;

	ieee80211_ops = local->ieee80211_ops;
	if(ieee80211_ops == NULL){
		ret = -EINVAL;
		return ret;
	}

	if (local->open_count){
		ret = -EBUSY;
		return ret;
	}

	if (ieee80211_hw_check(&local->hw, HAS_RATE_CONTROL)) {
		if (ieee80211_ops->set_rts_threshold == NULL){
			ret = -EINVAL;
			return ret;
		}
		return ret;
	}

	ref = rate_control_alloc(name, local);
	if (ref == NULL) {
		//wiphy_warn(local->hw.wiphy, "Failed to select rate control algorithm\n");
		ret = -ENOENT;
		return ret;
	}

	local->rate_ctrl = ref;

	#ifdef ENABLE_DBG_PRINT
	printf("[*][wiphy] Selected rate control algorithm '%s'\n", ref->rops->name);
	#endif
	return ret;
}

/* for now avoid also the tx queue functions */
static void ieee80211_txq_teardown_flows(struct ieee80211_local *local){

}

static int ieee80211_txq_setup_flows(struct ieee80211_local *local){
	int ret = 0;

	return ret;
}

static void ieee80211_check_wbrf_support(struct ieee80211_local *local){

}

static void rate_control_free(struct ieee80211_local *local, struct rate_control_ref *ctrl_ref){
	ctrl_ref->rops->free(ctrl_ref->priv);

	kfree(ctrl_ref);
}

static void rate_control_deinitialize(struct ieee80211_local *local){
	struct rate_control_ref *ref = NULL;

	ref = local->rate_ctrl;

	if (ref == NULL){
		goto end;
	}

	local->rate_ctrl = NULL;
	rate_control_free(local, ref);
	end:
}

static int ieee80211_mcs_to_chains(const struct ieee80211_mcs_info *mcs){
	int ret = 0;

	if (mcs == NULL){
		ret = 1;
		return ret;
	}

	/* TODO: consider rx_highest */
	if (mcs->rx_mask[3]){
		ret = 4;
		return ret;
	}
	if (mcs->rx_mask[2]){
		ret = 3;
		return ret;
	}
	if (mcs->rx_mask[1]){
		ret = 2;
		return ret;
	}

	ret = 1;
	return ret;
}

#define cfg80211_find_elem(eid, ies, len) cfg80211_find_elem_match(eid, ies, len, NULL, 0, 0)
#define cfg80211_find_ie(eid, ies, len)   cfg80211_find_ie_match(eid, ies, len, NULL, 0, 0)
#define cfg80211_find_ext_elem(ext_eid, ies, len) cfg80211_find_elem_match(WLAN_EID_EXTENSION, ies, len, (u8 *)ext_eid, 1, 0)

/*
 * Nothing should have been stuffed into the workqueue during
 * the suspend->resume cycle. Since we can't check each caller
 * of this function if we are already quiescing / suspended,
 * check here and don't WARN since this can actually happen when
 * the rx path (for example) is racing against __ieee80211_suspend
 * and suspending / quiescing was set after the rx path checked
 * them.
 */
static bool ieee80211_can_queue_work(struct ieee80211_local *local){
	if (local->quiescing || (local->suspended && !local->resuming)) {
		#ifdef ENABLE_DBG_PRINT
		printf("[!] queueing ieee80211 work while going to suspend\n");
		#endif
		return false;
	}
	return true;
}

/*
 * ieee80211_he_oper_size - calculate 802.11ax HE Operations IE size
 * @he_oper_ie: byte data of the He Operations IE, stating from the byte
 *	after the ext ID byte. It is assumed that he_oper_ie has at least
 *	sizeof(struct ieee80211_he_operation) bytes, the caller must have
 *	validated this.
 * @return the actual size of the IE data (not including header), or 0 on error
 */
static inline u8 ieee80211_he_oper_size(const u8 *he_oper_ie){
	const struct ieee80211_he_operation *he_oper = (const void *)he_oper_ie;
	u8 oper_len = sizeof(struct ieee80211_he_operation);
	u32 he_oper_params;

	/* Make sure the input is not NULL */
	if (!he_oper_ie){
		return 0;
	}

	/* Calc required length */
	he_oper_params = le32_to_cpu(he_oper->he_oper_params);
	if (he_oper_params & IEEE80211_HE_OPERATION_VHT_OPER_INFO){
		oper_len += 3;
	}

	if (he_oper_params & IEEE80211_HE_OPERATION_CO_HOSTED_BSS){
		oper_len++;
	}

	if (he_oper_params & IEEE80211_HE_OPERATION_6GHZ_OP_INFO){
		oper_len += sizeof(struct ieee80211_he_6ghz_oper);
	}

	/* Add the first byte (extension ID) to the total length */
	oper_len++;

	return oper_len;
}

/*
 * ieee80211_he_6ghz_oper - obtain 6 GHz operation field
 * @he_oper: HE operation element (must be pre-validated for size)
 *	but may be %NULL
 *
 * Return: a pointer to the 6 GHz operation field, or %NULL
 */
static inline const struct ieee80211_he_6ghz_oper * ieee80211_he_6ghz_oper(const struct ieee80211_he_operation *he_oper){
	u32 he_oper_params = 0;
	const u8 *ret = NULL;

	if (!he_oper){
		return NULL;
	}

	ret = (const void *)&he_oper->optional;

	he_oper_params = le32_to_cpu(he_oper->he_oper_params);

	if (!(he_oper_params & IEEE80211_HE_OPERATION_6GHZ_OP_INFO)){
		return NULL;
	}

	if (he_oper_params & IEEE80211_HE_OPERATION_VHT_OPER_INFO){
		ret += 3;
	}

	if (he_oper_params & IEEE80211_HE_OPERATION_CO_HOSTED_BSS){
		ret++;
	}
	return (const void *)ret;
}

static bool ieee80211_fill_rate_info(struct ieee80211_hw *hw, struct ieee80211_rx_status *stat, u8 band, struct rate_info *rate_info){
	struct ieee80211_supported_band *sband = NULL;

	sband = &director.ieee80211_local->bands[band];
	if (!rate_info || !sband){
	    return false;
	}

	stat->bw = rate_info->bw;
	stat->nss = rate_info->nss;
	stat->rate_idx = rate_info->mcs;

	if (rate_info->flags & RATE_INFO_FLAGS_EHT_MCS){
		stat->encoding = RX_ENC_EHT;
	}else if (rate_info->flags & RATE_INFO_FLAGS_HE_MCS){
		stat->encoding = RX_ENC_HE;
	}else if (rate_info->flags & RATE_INFO_FLAGS_VHT_MCS){
		stat->encoding = RX_ENC_VHT;
	}else if (rate_info->flags & RATE_INFO_FLAGS_MCS){
		stat->encoding = RX_ENC_HT;
	}else{
		stat->encoding = RX_ENC_LEGACY;
	}

	if (rate_info->flags & RATE_INFO_FLAGS_SHORT_GI){
		stat->enc_flags |= RX_ENC_FLAG_SHORT_GI;
	}

	switch (stat->encoding) {
	case RX_ENC_EHT:
		stat->eht.gi = rate_info->eht_gi;
		break;
	default:
		stat->he_gi = rate_info->he_gi;
		break;
	}

	if (stat->encoding != RX_ENC_LEGACY){
		return true;
	}

	stat->rate_idx = 0;
	for (int i = 0; i < sband->n_bitrates; i++) {
		if (rate_info->legacy != sband->bitrates[i].bitrate){
			continue;
		}
		stat->rate_idx = i;
		return true;
	}
	return false;
}

static int ieee80211_fill_rx_status(struct ieee80211_rx_status *stat,
				    struct ieee80211_hw *hw,
				    struct ieee80211_tx_rate *rate,
				    struct rate_info *rate_info, u8 band, int len){
	int ret = 0;

	memset(stat, 0, sizeof(*stat));
	stat->band = band;

	if (ieee80211_fill_rate_info(hw, stat, band, rate_info)){
		return ret;
	}

	if (!ieee80211_rate_valid(rate)){
		ret = -EINVAL;
	}

	if (rate->flags & IEEE80211_TX_RC_160_MHZ_WIDTH){
		stat->bw = RATE_INFO_BW_160;
	}else if (rate->flags & IEEE80211_TX_RC_80_MHZ_WIDTH){
		stat->bw = RATE_INFO_BW_80;
	}else if (rate->flags & IEEE80211_TX_RC_40_MHZ_WIDTH){
		stat->bw = RATE_INFO_BW_40;
	}else{
		stat->bw = RATE_INFO_BW_20;
	}
	stat->enc_flags = 0;
	if (rate->flags & IEEE80211_TX_RC_USE_SHORT_PREAMBLE){
		stat->enc_flags |= RX_ENC_FLAG_SHORTPRE;
	}
	if (rate->flags & IEEE80211_TX_RC_SHORT_GI){
		stat->enc_flags |= RX_ENC_FLAG_SHORT_GI;
	}
	stat->rate_idx = rate->idx;
	if (rate->flags & IEEE80211_TX_RC_VHT_MCS){
		stat->encoding = RX_ENC_VHT;
		stat->rate_idx = ieee80211_rate_get_vht_mcs(rate);
		stat->nss = ieee80211_rate_get_vht_nss(rate);
	}else if (rate->flags & IEEE80211_TX_RC_MCS){
		stat->encoding = RX_ENC_HT;
	}else{
		stat->encoding = RX_ENC_LEGACY;
	}

	return ret;
}

static u32 ieee80211_get_rate_duration(struct ieee80211_hw *hw, struct ieee80211_rx_status *status, u32 *overhead){
	bool sgi = status->enc_flags & RX_ENC_FLAG_SHORT_GI;
	int bw = 0;
	int streams = 0;
	int group = 0;
	int idx = 0;
	u32 duration = 0;

	static const struct mcs_group airtime_mcs_groups[] = {
		MCS_GROUP(1, 0, BW_20),
		MCS_GROUP(2, 0, BW_20),
		MCS_GROUP(3, 0, BW_20),
		MCS_GROUP(4, 0, BW_20),

		MCS_GROUP(1, 1, BW_20),
		MCS_GROUP(2, 1, BW_20),
		MCS_GROUP(3, 1, BW_20),
		MCS_GROUP(4, 1, BW_20),

		MCS_GROUP(1, 0, BW_40),
		MCS_GROUP(2, 0, BW_40),
		MCS_GROUP(3, 0, BW_40),
		MCS_GROUP(4, 0, BW_40),

		MCS_GROUP(1, 1, BW_40),
		MCS_GROUP(2, 1, BW_40),
		MCS_GROUP(3, 1, BW_40),
		MCS_GROUP(4, 1, BW_40),

		VHT_GROUP(1, 0, BW_20),
		VHT_GROUP(2, 0, BW_20),
		VHT_GROUP(3, 0, BW_20),
		VHT_GROUP(4, 0, BW_20),

		VHT_GROUP(1, 1, BW_20),
		VHT_GROUP(2, 1, BW_20),
		VHT_GROUP(3, 1, BW_20),
		VHT_GROUP(4, 1, BW_20),

		VHT_GROUP(1, 0, BW_40),
		VHT_GROUP(2, 0, BW_40),
		VHT_GROUP(3, 0, BW_40),
		VHT_GROUP(4, 0, BW_40),

		VHT_GROUP(1, 1, BW_40),
		VHT_GROUP(2, 1, BW_40),
		VHT_GROUP(3, 1, BW_40),
		VHT_GROUP(4, 1, BW_40),

		VHT_GROUP(1, 0, BW_80),
		VHT_GROUP(2, 0, BW_80),
		VHT_GROUP(3, 0, BW_80),
		VHT_GROUP(4, 0, BW_80),

		VHT_GROUP(1, 1, BW_80),
		VHT_GROUP(2, 1, BW_80),
		VHT_GROUP(3, 1, BW_80),
		VHT_GROUP(4, 1, BW_80),

		VHT_GROUP(1, 0, BW_160),
		VHT_GROUP(2, 0, BW_160),
		VHT_GROUP(3, 0, BW_160),
		VHT_GROUP(4, 0, BW_160),

		VHT_GROUP(1, 1, BW_160),
		VHT_GROUP(2, 1, BW_160),
		VHT_GROUP(3, 1, BW_160),
		VHT_GROUP(4, 1, BW_160),

		HE_GROUP(1, HE_GI_08, BW_20),
		HE_GROUP(2, HE_GI_08, BW_20),
		HE_GROUP(3, HE_GI_08, BW_20),
		HE_GROUP(4, HE_GI_08, BW_20),
		HE_GROUP(5, HE_GI_08, BW_20),
		HE_GROUP(6, HE_GI_08, BW_20),
		HE_GROUP(7, HE_GI_08, BW_20),
		HE_GROUP(8, HE_GI_08, BW_20),

		HE_GROUP(1, HE_GI_16, BW_20),
		HE_GROUP(2, HE_GI_16, BW_20),
		HE_GROUP(3, HE_GI_16, BW_20),
		HE_GROUP(4, HE_GI_16, BW_20),
		HE_GROUP(5, HE_GI_16, BW_20),
		HE_GROUP(6, HE_GI_16, BW_20),
		HE_GROUP(7, HE_GI_16, BW_20),
		HE_GROUP(8, HE_GI_16, BW_20),

		HE_GROUP(1, HE_GI_32, BW_20),
		HE_GROUP(2, HE_GI_32, BW_20),
		HE_GROUP(3, HE_GI_32, BW_20),
		HE_GROUP(4, HE_GI_32, BW_20),
		HE_GROUP(5, HE_GI_32, BW_20),
		HE_GROUP(6, HE_GI_32, BW_20),
		HE_GROUP(7, HE_GI_32, BW_20),
		HE_GROUP(8, HE_GI_32, BW_20),

		HE_GROUP(1, HE_GI_08, BW_40),
		HE_GROUP(2, HE_GI_08, BW_40),
		HE_GROUP(3, HE_GI_08, BW_40),
		HE_GROUP(4, HE_GI_08, BW_40),
		HE_GROUP(5, HE_GI_08, BW_40),
		HE_GROUP(6, HE_GI_08, BW_40),
		HE_GROUP(7, HE_GI_08, BW_40),
		HE_GROUP(8, HE_GI_08, BW_40),

		HE_GROUP(1, HE_GI_16, BW_40),
		HE_GROUP(2, HE_GI_16, BW_40),
		HE_GROUP(3, HE_GI_16, BW_40),
		HE_GROUP(4, HE_GI_16, BW_40),
		HE_GROUP(5, HE_GI_16, BW_40),
		HE_GROUP(6, HE_GI_16, BW_40),
		HE_GROUP(7, HE_GI_16, BW_40),
		HE_GROUP(8, HE_GI_16, BW_40),

		HE_GROUP(1, HE_GI_32, BW_40),
		HE_GROUP(2, HE_GI_32, BW_40),
		HE_GROUP(3, HE_GI_32, BW_40),
		HE_GROUP(4, HE_GI_32, BW_40),
		HE_GROUP(5, HE_GI_32, BW_40),
		HE_GROUP(6, HE_GI_32, BW_40),
		HE_GROUP(7, HE_GI_32, BW_40),
		HE_GROUP(8, HE_GI_32, BW_40),

		HE_GROUP(1, HE_GI_08, BW_80),
		HE_GROUP(2, HE_GI_08, BW_80),
		HE_GROUP(3, HE_GI_08, BW_80),
		HE_GROUP(4, HE_GI_08, BW_80),
		HE_GROUP(5, HE_GI_08, BW_80),
		HE_GROUP(6, HE_GI_08, BW_80),
		HE_GROUP(7, HE_GI_08, BW_80),
		HE_GROUP(8, HE_GI_08, BW_80),

		HE_GROUP(1, HE_GI_16, BW_80),
		HE_GROUP(2, HE_GI_16, BW_80),
		HE_GROUP(3, HE_GI_16, BW_80),
		HE_GROUP(4, HE_GI_16, BW_80),
		HE_GROUP(5, HE_GI_16, BW_80),
		HE_GROUP(6, HE_GI_16, BW_80),
		HE_GROUP(7, HE_GI_16, BW_80),
		HE_GROUP(8, HE_GI_16, BW_80),

		HE_GROUP(1, HE_GI_32, BW_80),
		HE_GROUP(2, HE_GI_32, BW_80),
		HE_GROUP(3, HE_GI_32, BW_80),
		HE_GROUP(4, HE_GI_32, BW_80),
		HE_GROUP(5, HE_GI_32, BW_80),
		HE_GROUP(6, HE_GI_32, BW_80),
		HE_GROUP(7, HE_GI_32, BW_80),
		HE_GROUP(8, HE_GI_32, BW_80),

		HE_GROUP(1, HE_GI_08, BW_160),
		HE_GROUP(2, HE_GI_08, BW_160),
		HE_GROUP(3, HE_GI_08, BW_160),
		HE_GROUP(4, HE_GI_08, BW_160),
		HE_GROUP(5, HE_GI_08, BW_160),
		HE_GROUP(6, HE_GI_08, BW_160),
		HE_GROUP(7, HE_GI_08, BW_160),
		HE_GROUP(8, HE_GI_08, BW_160),

		HE_GROUP(1, HE_GI_16, BW_160),
		HE_GROUP(2, HE_GI_16, BW_160),
		HE_GROUP(3, HE_GI_16, BW_160),
		HE_GROUP(4, HE_GI_16, BW_160),
		HE_GROUP(5, HE_GI_16, BW_160),
		HE_GROUP(6, HE_GI_16, BW_160),
		HE_GROUP(7, HE_GI_16, BW_160),
		HE_GROUP(8, HE_GI_16, BW_160),

		HE_GROUP(1, HE_GI_32, BW_160),
		HE_GROUP(2, HE_GI_32, BW_160),
		HE_GROUP(3, HE_GI_32, BW_160),
		HE_GROUP(4, HE_GI_32, BW_160),
		HE_GROUP(5, HE_GI_32, BW_160),
		HE_GROUP(6, HE_GI_32, BW_160),
		HE_GROUP(7, HE_GI_32, BW_160),
		HE_GROUP(8, HE_GI_32, BW_160),

		EHT_GROUP_RANGE(EHT_GI_08, BW_20),
		EHT_GROUP_RANGE(EHT_GI_16, BW_20),
		EHT_GROUP_RANGE(EHT_GI_32, BW_20),

		EHT_GROUP_RANGE(EHT_GI_08, BW_40),
		EHT_GROUP_RANGE(EHT_GI_16, BW_40),
		EHT_GROUP_RANGE(EHT_GI_32, BW_40),

		EHT_GROUP_RANGE(EHT_GI_08, BW_80),
		EHT_GROUP_RANGE(EHT_GI_16, BW_80),
		EHT_GROUP_RANGE(EHT_GI_32, BW_80),

		EHT_GROUP_RANGE(EHT_GI_08, BW_160),
		EHT_GROUP_RANGE(EHT_GI_16, BW_160),
		EHT_GROUP_RANGE(EHT_GI_32, BW_160),

		EHT_GROUP_RANGE(EHT_GI_08, BW_320),
		EHT_GROUP_RANGE(EHT_GI_16, BW_320),
		EHT_GROUP_RANGE(EHT_GI_32, BW_320),
	};

	switch (status->bw) {
	case RATE_INFO_BW_20:
		bw = BW_20;
		break;
	case RATE_INFO_BW_40:
		bw = BW_40;
		break;
	case RATE_INFO_BW_80:
		bw = BW_80;
		break;
	case RATE_INFO_BW_160:
		bw = BW_160;
		break;
	case RATE_INFO_BW_320:
		bw = BW_320;
		break;
	default:
		return 0;
	}

	switch (status->encoding) {
	case RX_ENC_VHT:
		streams = status->nss;
		idx = status->rate_idx;
		group = VHT_GROUP_IDX(streams, sgi, bw);
		break;
	case RX_ENC_HT:
		streams = ((status->rate_idx >> 3) & 3) + 1;
		idx = status->rate_idx & 7;
		group = HT_GROUP_IDX(streams, sgi, bw);
		break;
	case RX_ENC_HE:
		streams = status->nss;
		idx = status->rate_idx;
		group = HE_GROUP_IDX(streams, status->he_gi, bw);
		break;
	case RX_ENC_EHT:
		streams = status->nss;
		idx = status->rate_idx;
		group = EHT_GROUP_IDX(streams, status->eht.gi, bw);
		break;
	default:
		return 0;
	}

	switch (status->encoding) {
	case RX_ENC_EHT:
	case RX_ENC_HE:
		if (streams > 8){
			return 0;
		}
		break;
	default:
		if (streams > 4){
			return 0;
		}
		break;
	}

	if (idx >= MCS_GROUP_RATES){
		return 0;
	}

	duration = airtime_mcs_groups[group].duration[idx];
	duration <<= airtime_mcs_groups[group].shift;
	*overhead = 36 + (streams << 2);

	return duration;
}

static u32 ieee80211_calc_legacy_rate_duration(u16 bitrate, bool short_pre, bool cck, int len){
	u32 duration = 0;

	if (cck) {
		duration = 144 + 48; /* preamble + PLCP */
		if (short_pre){
			duration >>= 1;
		}
		duration += 10; /* SIFS */
	} else {
		duration = 20 + 16; /* premable + SIFS */
	}

	len <<= 3;
	duration += (len * 10) / bitrate;

	return duration;
}

static u32 ieee80211_calc_tx_airtime_rate(struct ieee80211_hw *hw, struct ieee80211_tx_rate *rate, struct rate_info *rate_info, u8 band, int len){
	struct ieee80211_rx_status stat = {};

	if (ieee80211_fill_rx_status(&stat, hw, rate, rate_info, band, len)){
		return 0;
	}

	return ieee80211_calc_rx_airtime(hw, &stat, len);
}

static const struct element *cfg80211_get_profile_continuation(const u8 *ie, size_t ielen, const struct element *mbssid_elem, const struct element *sub_elem) {
	const u8 *mbssid_end = mbssid_elem->data + mbssid_elem->datalen;
	const struct element *next_mbssid = NULL;
	const struct element *next_sub = NULL;

	next_mbssid = cfg80211_find_elem(WLAN_EID_MULTIPLE_BSSID, mbssid_end, ielen - (mbssid_end - ie));

	/*
	 * If it is not the last subelement in current MBSSID IE or there isn't
	 * a next MBSSID IE - profile is complete.
	 */
	if ((sub_elem->data + sub_elem->datalen < mbssid_end - 1) || !next_mbssid){
		return NULL;
	}

	/* For any length error, just return NULL */

	if (next_mbssid->datalen < 4){
		return NULL;
	}

	next_sub = (void *)&next_mbssid->data[1];

	if (next_mbssid->data + next_mbssid->datalen < next_sub->data + next_sub->datalen){
		return NULL;
	}

	if (next_sub->id != 0 || next_sub->datalen < 2){
		return NULL;
	}
	/*
	 * Check if the first element in the next sub element is a start
	 * of a new profile
	 */
	return next_sub->data[0] == WLAN_EID_NON_TX_BSSID_CAP ? NULL : next_mbssid;
}

/*
 * cfg80211_chandef_get_width - return chandef width in MHz
 * @c: chandef to return bandwidth for
 * Return: channel width in MHz for the given chandef; note that it returns
 *	80 for 80+80 configurations
 */
static inline int cfg80211_chandef_get_width(const struct cfg80211_chan_def *c){
	return nl80211_chan_width_to_mhz(c->width);
}

/*
 * Checks if center frequency of chan falls with in the bandwidth
 * range of chandef.
 */
static bool cfg80211_is_sub_chan(struct cfg80211_chan_def *chandef, struct ieee80211_channel *chan,  bool primary_only){
	int width = 0;
	u32 freq = 0;

	if (!chandef->chan){
		return false;
	}

	if (chandef->chan->center_freq == chan->center_freq){
		return true;
	}

	if (primary_only){
		return false;
	}

	width = cfg80211_chandef_get_width(chandef);
	if (width <= 20){
		return false;
	}

	for (freq = chandef->center_freq1 - width / 2 + 10; freq <= chandef->center_freq1 + width / 2 - 10; freq += 20) {
		if (chan->center_freq == freq){
			return true;
		}
	}

	if (!chandef->center_freq2){
		return false;
	}

	for (freq = chandef->center_freq2 - width / 2 + 10; freq <= chandef->center_freq2 + width / 2 - 10; freq += 20) {
		if (chan->center_freq == freq){
			return true;
		}
	}

	return false;
}

static struct rhlist_head *sta_info_hash_lookup(struct ieee80211_local *local, u8 *addr){
	static const struct rhashtable_params sta_rht_params = {
		.nelem_hint = 3, /* start small */
		.automatic_shrinking = true,
		.head_offset = offsetof(struct sta_info, hash_node),
		.key_offset = offsetof(struct sta_info, addr),
		.key_len = ETH_ALEN,
		.max_size = CONFIG_MAC80211_STA_HASH_MAX_SIZE,
	};

	return rhltable_lookup(&local->sta_hash, addr, sta_rht_params);
}

static void sta_info_stop(struct ieee80211_local *local){
	rhltable_destroy(&local->sta_hash);
	rhltable_destroy(&local->link_sta_hash);
}

/*
 * Get sta info either from the specified interface
 * or from one of its vlans
 */
static struct sta_info *sta_info_get_bss(struct ieee80211_sub_if_data *sdata, u8 *addr){
	struct ieee80211_local *local = director.ieee80211_local;
	struct rhlist_head *tmp = NULL;
	struct sta_info *sta = NULL;

	for_each_sta_info(local, addr, sta, tmp) {
		if (sdata == NULL || &sta->sdata == sdata || sta->vif == sdata->vif || (sta->sdata.bss && sta->sdata.bss == sdata->bss)) {
			/* this is safe as the caller must already hold
			 * another rcu read section or the mutex
			 */
			return sta;
		}
	}

	return NULL;
}

/* protected by RCU */
static struct sta_info *sta_info_get(struct ieee80211_sub_if_data *sdata, u8 *addr){
	struct ieee80211_local *local = NULL;
	struct rhlist_head *tmp = NULL;
	struct sta_info *sta = NULL;

	local = director.ieee80211_local;

	for_each_sta_info(local, addr, sta, tmp) {
		if (&sta->sdata == sdata) {
			/* this is safe as the caller must already hold
			 * another rcu read section or the mutex
			 */
			return sta;
		}
	}

	return NULL;
}

static inline struct crypto_aead *ieee80211_aes_key_setup_encrypt(const u8 key[], size_t key_len, size_t mic_len){
	return aead_key_setup_encrypt("ccm(aes)", key, key_len, mic_len);
}

static struct crypto_shash *ieee80211_aes_cmac_key_setup(const u8 key[], size_t key_len){
	int err = 0;
	struct crypto_shash *tfm = NULL;

	tfm = crypto_alloc_shash("cmac(aes)", 0, 0);
	if (!IS_ERR(tfm)) {
		err = crypto_shash_setkey(tfm, key, key_len);

		if (err) {
			crypto_free_shash(tfm);
			return ERR_PTR(err);
		}
	}
	return tfm;
}

static struct crypto_aead *ieee80211_aes_gmac_key_setup(const u8 key[], size_t key_len){
	int ret = 0;
	struct crypto_aead *tfm = NULL;

	tfm = crypto_alloc_aead("gcm(aes)", 0, CRYPTO_ALG_ASYNC);
	if (IS_ERR(tfm)){
		return tfm;
	}

	ret = crypto_aead_setkey(tfm, key, key_len);
	if (ret == 0){
		ret = crypto_aead_setauthsize(tfm, IEEE80211_GMAC_MIC_LEN);
	}
	if (ret == 0){
		return tfm;
	}

	crypto_free_aead(tfm);
	return ERR_PTR(ret);
}

static inline struct crypto_aead *ieee80211_aes_gcm_key_setup_encrypt(const u8 key[], size_t key_len){
	return aead_key_setup_encrypt("gcm(aes)", key, key_len, IEEE80211_GCMP_MIC_LEN);
}

static void ieee80211_key_iter(struct ieee80211_hw *hw,
		   struct ieee80211_vif *vif,
		   struct ieee80211_key *key,
		   void (*iter)(struct ieee80211_hw *hw,
				struct ieee80211_vif *vif,
				struct ieee80211_sta *sta,
				struct ieee80211_key_conf *key,
				void *data),
		   void *iter_data){
	/* skip keys of station in removal process */
	if (key->sta && key->sta->removed){
		goto end;
	}

	if (!(key->flags & KEY_FLAG_UPLOADED_TO_HARDWARE)){
		goto end;
	}

	iter(hw, vif, key->sta ? &key->sta->sta : NULL, &key->conf, iter_data);
	end:
}

static void _ieee80211_iter_keys_rcu(struct ieee80211_hw *hw,  struct ieee80211_sub_if_data *sdata,
			 void (*iter)(struct ieee80211_hw *hw,
				      struct ieee80211_vif *vif,
				      struct ieee80211_sta *sta,
				      struct ieee80211_key_conf *key,
				      void *data),
			 void *iter_data){
	struct ieee80211_key *key = NULL;
	struct ieee80211_vif *vif = NULL;

	vif = sdata->vif;
	list_for_each_entry_rcu(key, &sdata->key_list, list){
		ieee80211_key_iter(hw, vif, key, iter, iter_data);
	}
}

static void ieee80211_wep_init(struct ieee80211_local *local){
	/* start WEP IV from a random value */
	get_random_bytes(&local->wep_iv, IEEE80211_WEP_IV_LEN);
}

static inline pthread_mutex_t *wiphy_work_queue_lock(void){
	struct ieee80211_local *local = director.ieee80211_local;

	if(local != NULL){
		return &local->wiphy_work_queue_mutex;
	}
	return &director.wiphy_work_queue_mutex;
}

static void cfg80211_process_wiphy_works(struct cfg80211_registered_device *rdev, struct wiphy_work *end){
	unsigned int runaway_limit = 100;
	struct wiphy_work *wk = NULL;
	pthread_mutex_t *lock = wiphy_work_queue_lock();

	if (rdev == NULL || rdev->wiphy == NULL || lock == NULL){
		goto out;
	}

	for (;;) {
		wk = NULL;

		pthread_mutex_lock(lock);
		if (list_empty(&rdev->wiphy_work_list)){
			pthread_mutex_unlock(lock);
			break;
		}
		wk = list_first_entry(&rdev->wiphy_work_list, struct wiphy_work, entry);
		list_del_init(&wk->entry);
		pthread_mutex_unlock(lock);
		if (wk == NULL || wk->func == NULL || (uintptr_t)wk->func <= 4096){
			if (wk == end){
				break;
			}
			continue;
		}
		wk->func(rdev->wiphy, wk);

		if (wk == end){
			break;
		}
		if (--runaway_limit == 0){
			pthread_mutex_lock(lock);
			INIT_LIST_HEAD(&rdev->wiphy_work_list);
			pthread_mutex_unlock(lock);
		}
	}
	out:
}

static void cfg80211_wiphy_worker(struct work_struct *work){
	cfg80211_process_wiphy_works(director.rdev, NULL);
}

static int ieee80211_init_cipher_suites(struct ieee80211_local *local){
	int ret = 0;
	bool have_mfp = ieee80211_hw_check(&local->hw, MFP_CAPABLE);
	static const u32 cipher_suites[] = {
		/* keep WEP and TKIP first, they may be removed below */
		WLAN_CIPHER_SUITE_WEP40,
		WLAN_CIPHER_SUITE_WEP104,
		WLAN_CIPHER_SUITE_TKIP,
		WLAN_CIPHER_SUITE_CCMP,
		WLAN_CIPHER_SUITE_CCMP_256,
		WLAN_CIPHER_SUITE_GCMP,
		WLAN_CIPHER_SUITE_GCMP_256,

		/* keep last -- depends on hw flags! */
		WLAN_CIPHER_SUITE_AES_CMAC,
		WLAN_CIPHER_SUITE_BIP_CMAC_256,
		WLAN_CIPHER_SUITE_BIP_GMAC_128,
		WLAN_CIPHER_SUITE_BIP_GMAC_256,
	};

	struct wiphy *wiphy = NULL;

	wiphy = local->wiphy;
	if(wiphy == NULL){
		ret = -EINVAL;
		return ret;
	}

	if (ieee80211_hw_check(&local->hw, SW_CRYPTO_CONTROL) && wiphy->cipher_suites){
		ret = -EINVAL;
		return ret;
	}

	if (!wiphy->cipher_suites) {
		/* assign the (software supported and perhaps offloaded)
		 * cipher suites
		 */
		wiphy->cipher_suites = cipher_suites;
		wiphy->n_cipher_suites = ARRAY_SIZE(cipher_suites);
	}

	return ret;
}

static struct ieee80211_key *ieee80211_key_alloc(u32 cipher, int idx, size_t key_len, u8 *key_data, size_t seq_len, u8 *seq){
	int err = 0;
	int i = 0;
	int j = 0;
	struct ieee80211_key *key = NULL;

	if (idx < 0 || idx >= NUM_DEFAULT_KEYS + NUM_DEFAULT_MGMT_KEYS + NUM_DEFAULT_BEACON_KEYS){
		return ERR_PTR(-EINVAL);
	}

	key = malloc(sizeof(struct ieee80211_key) + key_len);
	if (key == NULL){
		return ERR_PTR(-ENOMEM);
	}

	/*
	 * Default to software encryption; we'll later upload the
	 * key to the hardware if possible.
	 */
	key->conf.flags = 0;
	key->flags = 0;

	key->conf.link_id = -1;
	key->conf.cipher = cipher;
	key->conf.keyidx = idx;
	key->conf.keylen = key_len;
	switch (cipher) {
	case WLAN_CIPHER_SUITE_WEP40:
	case WLAN_CIPHER_SUITE_WEP104:
		key->conf.iv_len = IEEE80211_WEP_IV_LEN;
		key->conf.icv_len = IEEE80211_WEP_ICV_LEN;
		break;
	case WLAN_CIPHER_SUITE_TKIP:
		key->conf.iv_len = IEEE80211_TKIP_IV_LEN;
		key->conf.icv_len = IEEE80211_TKIP_ICV_LEN;
		if (seq) {
			for (i = 0; i < IEEE80211_NUM_TIDS; i++) {
				key->u.tkip.rx[i].iv32 = get_unaligned_le32(&seq[2]);
				key->u.tkip.rx[i].iv16 = get_unaligned_le16(seq);
			}
		}
		break;
	case WLAN_CIPHER_SUITE_CCMP:
		key->conf.iv_len = IEEE80211_CCMP_HDR_LEN;
		key->conf.icv_len = IEEE80211_CCMP_MIC_LEN;
		if (seq) {
			for (i = 0; i < IEEE80211_NUM_TIDS + 1; i++){
				for (j = 0; j < IEEE80211_CCMP_PN_LEN; j++){
					key->u.ccmp.rx_pn[i][j] = seq[IEEE80211_CCMP_PN_LEN - j - 1];
				}
			}
		}
		/*
		 * Initialize AES key state here as an optimization so that
		 * it does not need to be initialized for every packet.
		 */
		key->u.ccmp.tfm = ieee80211_aes_key_setup_encrypt(key_data, key_len, IEEE80211_CCMP_MIC_LEN);
		if (IS_ERR(key->u.ccmp.tfm)) {
			err = PTR_ERR(key->u.ccmp.tfm);
			kfree(key);
			return ERR_PTR(err);
		}
		break;
	case WLAN_CIPHER_SUITE_CCMP_256:
		key->conf.iv_len = IEEE80211_CCMP_256_HDR_LEN;
		key->conf.icv_len = IEEE80211_CCMP_256_MIC_LEN;
		for (i = 0; seq && i < IEEE80211_NUM_TIDS + 1; i++){
			for (j = 0; j < IEEE80211_CCMP_256_PN_LEN; j++){
				key->u.ccmp.rx_pn[i][j] = seq[IEEE80211_CCMP_256_PN_LEN - j - 1];
			}
		}
		/* Initialize AES key state here as an optimization so that
		 * it does not need to be initialized for every packet.
		 */
		key->u.ccmp.tfm = ieee80211_aes_key_setup_encrypt(key_data, key_len, IEEE80211_CCMP_256_MIC_LEN);
		if (IS_ERR(key->u.ccmp.tfm)) {
			err = PTR_ERR(key->u.ccmp.tfm);
			kfree(key);
			return ERR_PTR(err);
		}
		break;
	case WLAN_CIPHER_SUITE_AES_CMAC:
	case WLAN_CIPHER_SUITE_BIP_CMAC_256:
		key->conf.iv_len = 0;
		if (cipher == WLAN_CIPHER_SUITE_AES_CMAC){
			key->conf.icv_len = sizeof(struct ieee80211_mmie);
		}else{
			key->conf.icv_len = sizeof(struct ieee80211_mmie_16);
		}
		if (seq){
			for (j = 0; j < IEEE80211_CMAC_PN_LEN; j++){
				key->u.aes_cmac.rx_pn[j] = seq[IEEE80211_CMAC_PN_LEN - j - 1];
			}
		}
		/*
		 * Initialize AES key state here as an optimization so that
		 * it does not need to be initialized for every packet.
		 */
		key->u.aes_cmac.tfm = ieee80211_aes_cmac_key_setup(key_data, key_len);
		if (IS_ERR(key->u.aes_cmac.tfm)) {
			err = PTR_ERR(key->u.aes_cmac.tfm);
			kfree(key);
			return ERR_PTR(err);
		}
		break;
	case WLAN_CIPHER_SUITE_BIP_GMAC_128:
	case WLAN_CIPHER_SUITE_BIP_GMAC_256:
		key->conf.iv_len = 0;
		key->conf.icv_len = sizeof(struct ieee80211_mmie_16);
		if (seq){
			for (j = 0; j < IEEE80211_GMAC_PN_LEN; j++){
				key->u.aes_gmac.rx_pn[j] = seq[IEEE80211_GMAC_PN_LEN - j - 1];
			}
		}
		/* Initialize AES key state here as an optimization so that
		 * it does not need to be initialized for every packet.
		 */
		key->u.aes_gmac.tfm = ieee80211_aes_gmac_key_setup(key_data, key_len);
		if (IS_ERR(key->u.aes_gmac.tfm)) {
			err = PTR_ERR(key->u.aes_gmac.tfm);
			kfree(key);
			return ERR_PTR(err);
		}
		break;
	case WLAN_CIPHER_SUITE_GCMP:
	case WLAN_CIPHER_SUITE_GCMP_256:
		key->conf.iv_len = IEEE80211_GCMP_HDR_LEN;
		key->conf.icv_len = IEEE80211_GCMP_MIC_LEN;
		for (i = 0; seq && i < IEEE80211_NUM_TIDS + 1; i++){
			for (j = 0; j < IEEE80211_GCMP_PN_LEN; j++){
				key->u.gcmp.rx_pn[i][j] = seq[IEEE80211_GCMP_PN_LEN - j - 1];
			}
		}
		/* Initialize AES key state here as an optimization so that
		 * it does not need to be initialized for every packet.
		 */
		key->u.gcmp.tfm = ieee80211_aes_gcm_key_setup_encrypt(key_data, key_len);
		if (IS_ERR(key->u.gcmp.tfm)) {
			err = PTR_ERR(key->u.gcmp.tfm);
			kfree(key);
			return ERR_PTR(err);
		}
		break;
	}
	memcpy(key->conf.key, key_data, key_len);
	INIT_LIST_HEAD(&key->list);

	return key;
}

static void ieee80211_report_used_skb(struct ieee80211_local *local, struct sk_buff *skb, bool dropped, int ack_hwtstamp){
	bool acked = false;
	u16 tx_time_est = 0;
	struct ieee80211_hdr *hdr = NULL;
	struct ieee80211_tx_info *info = NULL;
	struct ieee80211_sub_if_data *sdata = NULL;

	tx_time_est = 0;
	hdr = (void *)skb->data;
	info = skb->cb;
	acked = info != NULL && (info->flags & IEEE80211_TX_STAT_ACK);

	tx_time_est = ieee80211_info_get_tx_time_est(info);
	if (dropped){
		acked = false;
	}
	#if 0
	if (tx_time_est) {
		struct sta_info *sta;

		rcu_read_lock();

		sta = sta_info_get_by_addrs(local, hdr->addr1, hdr->addr2);
		ieee80211_sta_update_pending_airtime(local, sta,
						     skb_get_queue_mapping(skb),
						     tx_time_est,
						     true);
		rcu_read_unlock();
	}

	if (info->flags & IEEE80211_TX_INTFL_MLME_CONN_TX) {
		sdata = NULL;
		rcu_read_lock();

		sdata = ieee80211_sdata_from_skb(local, skb);

		if (!sdata) {
			skb->dev = NULL;
		} else if (!dropped) {
			/* Check to see if packet is a TDLS teardown packet */
			if (ieee80211_is_data(hdr->frame_control) && (ieee80211_get_tdls_action(skb) == WLAN_TDLS_TEARDOWN)) {
				ieee80211_tdls_td_tx_handle(local, sdata, skb, info->flags);
			} else if (ieee80211_s1g_is_twt_setup(skb)) {
				if (!acked) {
					struct sk_buff *qskb;

					qskb = skb_clone(skb, GFP_ATOMIC);
					if (qskb) {
						skb_queue_tail(&sdata->status_queue,  qskb);
						wiphy_work_queue(local->hw.wiphy, &sdata->work);
					}
				}
			} else {
				ieee80211_mgd_conn_tx_status(sdata, hdr->frame_control, acked);
			}
		}

		rcu_read_unlock();
	} else if (info->status_data_idr) {
		ieee80211_report_ack_skb(local, skb, acked, dropped,
					 ack_hwtstamp);
	} else if (info->status_data) {
		struct ieee80211_sub_if_data *sdata;

		rcu_read_lock();

		sdata = ieee80211_sdata_from_skb(local, skb);

		switch (u16_get_bits(info->status_data,
				     IEEE80211_STATUS_TYPE_MASK)) {
		case IEEE80211_STATUS_TYPE_SMPS:
			ieee80211_handle_smps_status(sdata, acked,
						     info->status_data);
			break;
		case IEEE80211_STATUS_TYPE_NEG_TTLM:
			ieee80211_handle_teardown_ttlm_status(sdata, acked);
			break;
		}
		rcu_read_unlock();
	}

	if (!dropped && skb->destructor) {
		skb->wifi_acked_valid = 1;
		skb->wifi_acked = acked;
	}

	ieee80211_led_tx(local);

	if (skb_has_frag_list(skb)) {
		kfree_skb_list(skb_shinfo(skb)->frag_list);
		skb_shinfo(skb)->frag_list = NULL;
	}
	#endif
}

static void __iterate_interfaces(struct ieee80211_local *local, u32 iter_flags, void (*iterator)(void *data, u8 *mac, struct ieee80211_vif *vif), void *data){
	struct ieee80211_vif *ieee80211_vif = NULL;

	ieee80211_vif = local->ieee80211_vif;
	if (ieee80211_vif == NULL){
		goto end;
	}

	if (ieee80211_vif->type == NL80211_IFTYPE_AP_VLAN){
		goto end;
	}

	iterator(data, ieee80211_vif->addr, ieee80211_vif);
	end:
}

static void _ieee80211_wake_txqs(struct ieee80211_local *local){
	tasklet_schedule(&local->wake_txqs_tasklet);
}

static void __ieee80211_stop_queue(struct ieee80211_hw *hw, int queue, enum queue_stop_reason reason, bool refcounted){
	struct ieee80211_local *local = NULL;

	local = director.ieee80211_local;
	if (queue >= hw->queues){
		goto end;
	}

	if (!refcounted){
		local->q_stop_reasons[queue][reason] = 1;
	}else{
		local->q_stop_reasons[queue][reason]++;
	}
	set_bit(reason, &local->queue_stop_reasons[queue]);
	end:
}

static void __ieee80211_wake_queue(struct ieee80211_hw *hw, int queue, enum queue_stop_reason reason, bool refcounted){
	struct ieee80211_local *local = NULL;

	local = director.ieee80211_local;
	if (queue >= hw->queues){
		goto end;
	}

	if (!test_bit(reason, &local->queue_stop_reasons[queue])){
		goto end;
	}

	if (!refcounted) {
		local->q_stop_reasons[queue][reason] = 0;
	} else {
		local->q_stop_reasons[queue][reason]--;
		if (local->q_stop_reasons[queue][reason] < 0){
			local->q_stop_reasons[queue][reason] = 0;
		}
	}

	if (local->q_stop_reasons[queue][reason] == 0){
		clear_bit(reason, &local->queue_stop_reasons[queue]);
	}

	if (local->queue_stop_reasons[queue] != 0){
		/* someone still has this queue stopped */
		goto end;
	}

	if (!skb_queue_empty(&local->pending[queue])){
		tasklet_schedule(&local->tx_pending_tasklet);
	}

	/*
	 * Calling _ieee80211_wake_txqs here can be a problem because it may
	 * release queue_stop_reason_lock which has been taken by
	 * __ieee80211_wake_queue's caller. It is certainly not very nice to
	 * release someone's lock, but it is fine because all the callers of
	 * __ieee80211_wake_queue call it right before releasing the lock.
	 */
	if (reason == IEEE80211_QUEUE_STOP_REASON_DRIVER){
		tasklet_schedule(&local->wake_txqs_tasklet);
	}else{
		_ieee80211_wake_txqs(local);
	}
	end:
}

static void ieee80211_stop_queues_by_reason(struct ieee80211_hw *hw, unsigned long queues, enum queue_stop_reason reason, bool refcounted){
	int i = 0;
	struct ieee80211_local *local = director.ieee80211_local;

	for_each_set_bit(i, &queues, hw->queues)
		__ieee80211_stop_queue(hw, i, reason, refcounted);
}

static void ieee80211_wake_queues_by_reason(struct ieee80211_hw *hw, unsigned long queues, enum queue_stop_reason reason, bool refcounted){
	int i = 0;
	struct ieee80211_local *local = NULL;

	local = director.ieee80211_local;
	for_each_set_bit(i, &queues, hw->queues)
		__ieee80211_wake_queue(hw, i, reason, refcounted);
}

static void *ieee80211_priv(struct ieee80211_hw *hw){
	return hw->priv;
}

/* add/remove interface */
static int ieee80211_if_add(struct ieee80211_local *local, const char *name, unsigned char name_assign_type, struct wireless_dev **new_wdev, enum nl80211_iftype type, struct vif_params *params){
	int ret = 0;
	struct txq_info *txqi = NULL;
	struct ieee80211_vif *vif = NULL;
	struct net_device *netdev = NULL;
	struct ieee80211_vif **vif_slot = NULL;
	struct ieee80211_hw *ieee80211_hw = NULL;
	struct ieee80211_sub_if_data *sdata = NULL;
	struct ieee80211_bss_conf *bss_conf = NULL;

	if (local == NULL){
		ret = -EINVAL;
		return ret;
	}

	switch (type) {
	case NL80211_IFTYPE_AP:
	case NL80211_IFTYPE_P2P_GO:
		vif_slot = &local->ieee80211_ap_vif;
		break;
	case NL80211_IFTYPE_MONITOR:
		vif_slot = &local->ieee80211_mon_vif;
		break;
	default:
		vif_slot = &local->ieee80211_vif;
		break;
	}
	ieee80211_hw = local->ieee80211_hw;

	if (*vif_slot == NULL) {
		if (director.struct_sizes.ieee80211_vif == 0){
			ret = -EINVAL;
		}
		*vif_slot = calloc(1, director.struct_sizes.ieee80211_vif + ieee80211_hw->vif_data_size);
		if (*vif_slot == NULL){
			ret = -ENOMEM;
		}
	}

	sdata = calloc(1, sizeof(*sdata));
	if (sdata == NULL){
		ret = -ENOMEM;
	}

	vif = *vif_slot;
	vif->type = type;
	if (params != NULL && !is_zero_ether_addr(params->macaddr)){
		memcpy(vif->addr, params->macaddr, ETH_ALEN);
	}

	memset(&vif->bss_conf, 0, director.struct_sizes.ieee80211_bss_conf);
	bss_conf = &vif->bss_conf;
	bss_conf->vif = vif;
	bss_conf->link_id = 0;
	ether_addr_copy(bss_conf->addr, vif->addr);
	rcu_assign_pointer(vif->link_conf[0], bss_conf);
	vif->valid_links |= BIT(0);
	vif->active_links |= BIT(0);

	INIT_LIST_HEAD(&sdata->list);
	INIT_LIST_HEAD(&sdata->key_list);
	skb_queue_head_init(&sdata->skb_queue);
	skb_queue_head_init(&sdata->status_queue);
	sdata->local = local;
	sdata->vif = vif;
	snprintf(sdata->name, sizeof(sdata->name), "%s", name ? name : "wlan0");
	set_bit(SDATA_STATE_RUNNING, &sdata->state);

	sdata->deflink.sdata = sdata;
	sdata->deflink.link_id = 0;
	sdata->deflink.conf = bss_conf;
	rcu_assign_pointer(sdata->link[0], &sdata->deflink);

	pthread_mutex_lock(&local->iflist_mtx);
	list_add_tail_rcu(&sdata->list, &local->interfaces);
	pthread_mutex_unlock(&local->iflist_mtx);

	if (new_wdev != NULL){
		*new_wdev = &vif->wdev;
	}

	return ret;
}

static void ieee80211_if_remove(struct ieee80211_sub_if_data *sdata){
	struct ieee80211_local *local = NULL;

	local = director.ieee80211_local;
	pthread_mutex_lock(&local->iflist_mtx);
	list_del_rcu(&sdata->list);
	pthread_mutex_unlock(&local->iflist_mtx);

	if (sdata->dev == NULL) {
		kfree(sdata);
	}
}

static inline void drv_vif_cfg_changed(struct ieee80211_local *local, struct ieee80211_sub_if_data *sdata, u64 changed){
	struct ieee80211_ops *ieee80211_ops = NULL;

	ieee80211_ops = local->ieee80211_ops;
	if(ieee80211_ops == NULL){
		goto end;
	}

	if (ieee80211_ops->vif_cfg_changed){
		// ops->vif_cfg_changed(&local->hw, &sdata->vif, changed);
	}else if (ieee80211_ops->bss_info_changed){
		// ops->bss_info_changed(&local->hw, &sdata->vif, &sdata->vif.bss_conf, changed);
	}
	end:
}

static void ieee80211_vif_cfg_change_notify(struct ieee80211_sub_if_data *sdata, u64 changed){
	struct ieee80211_local *local = NULL;
	struct ieee80211_vif *ieee80211_vif = NULL;

	local = director.ieee80211_local;
	ieee80211_vif = sdata->vif;

	if (!changed || ieee80211_vif->type == NL80211_IFTYPE_AP_VLAN){
		goto end;
	}

	drv_vif_cfg_changed(local, sdata, changed);
	end:
}

static void ieee80211_remove_interfaces(struct ieee80211_local *local){
	struct ieee80211_vif_cfg *cfg = NULL;
	struct ieee80211_sub_if_data *tmp = NULL;
	struct ieee80211_vif *ieee80211_vif = NULL;
	struct ieee80211_sub_if_data *sdata = NULL;
	LIST_HEAD(unreg_list);

	/* Before destroying the interfaces, make sure they're all stopped so
	 * that the hardware is stopped. Otherwise, the driver might still be
	 * iterating the interfaces during the shutdown, e.g. from a worker
	 * or from RX processing or similar, and if it does so (using atomic
	 * iteration) while we're manipulating the list, the iteration will
	 * crash.
	 *
	 * After this, the hardware should be stopped and the driver should
	 * have stopped all of its activities, so that we can do RCU-unaware
	 * manipulations of the interface list below.
	 */

	ieee80211_vif = sdata->vif;
	pthread_mutex_lock(&local->iflist_mtx);
	list_splice_init(&local->interfaces, &unreg_list);
	pthread_mutex_unlock(&local->iflist_mtx);

	list_for_each_entry_safe(sdata, tmp, &unreg_list, list) {
		bool netdev = sdata->dev;
		cfg = &ieee80211_vif->cfg;
		/*
		 * Remove IP addresses explicitly, since the notifier will
		 * skip the callbacks if wdev->registered is false, since
		 * we can't acquire the wiphy_lock() again there if already
		 * inside this locked section.
		 */
		cfg->arp_addr_cnt = 0;
		if (ieee80211_vif->type == NL80211_IFTYPE_STATION && sdata->u.mgd.associated){
			ieee80211_vif_cfg_change_notify(sdata, BSS_CHANGED_ARP_FILTER);
		}
		list_del(&sdata->list);
		//cfg80211_unregister_wdev(&sdata->wdev);

		if (netdev == false){
			kfree(sdata);
		}
	}
}

static int sta_info_init(struct ieee80211_local *local){
	int ret = 0;

	static const struct rhashtable_params sta_rht_params = {
		.nelem_hint = 3, /* start small */
		.automatic_shrinking = true,
		.head_offset = offsetof(struct sta_info, hash_node),
		.key_offset = offsetof(struct sta_info, addr),
		.key_len = ETH_ALEN,
		.max_size = CONFIG_MAC80211_STA_HASH_MAX_SIZE,
	};

	static const struct rhashtable_params link_sta_rht_params = {
		.nelem_hint = 3, /* start small */
		.automatic_shrinking = true,
		.head_offset = offsetof(struct link_sta_info, link_hash_node),
		.key_offset = offsetof(struct link_sta_info, addr),
		.key_len = ETH_ALEN,
		.max_size = CONFIG_MAC80211_STA_HASH_MAX_SIZE,
	};

	ret = rhltable_init(&local->sta_hash, &sta_rht_params);
	if (ret){
		return ret;
	}

	ret = rhltable_init(&local->link_sta_hash, &link_sta_rht_params);
	if (ret) {
		rhltable_destroy(&local->sta_hash);
		return ret;
	}

	INIT_LIST_HEAD(&local->sta_list);

	return ret;
}

/* ieee80211_tpt_led_trig_rx: empty function */
static inline void ieee80211_tpt_led_trig_rx(struct ieee80211_local *local, int bytes){

}

/*
 * monitor mode reception
 *
 * This function cleans up the SKB, i.e. it removes all the stuff
 * only useful for monitoring.
 */
static struct sk_buff *ieee80211_clean_skb(struct sk_buff *skb, unsigned int present_fcs_len, unsigned int rtap_space){
	__le16 fc = 0;
	unsigned int hdrlen = 0;
	struct ieee80211_hdr *hdr = NULL;
	struct ieee80211_rx_status *status = NULL;

	status = IEEE80211_SKB_RXCB(skb);
	if (present_fcs_len){
		__pskb_trim(skb, skb->len - present_fcs_len);
	}
	pskb_pull(skb, rtap_space);

	/* After pulling radiotap header, clear all flags that indicate
	 * info in skb->data.
	 */
	status->flag &= ~(RX_FLAG_RADIOTAP_TLV_AT_END |
			  RX_FLAG_RADIOTAP_LSIG |
			  RX_FLAG_RADIOTAP_HE_MU |
			  RX_FLAG_RADIOTAP_HE |
			  RX_FLAG_RADIOTAP_VHT);

	hdr = (void *)skb->data;
	fc = hdr->frame_control;

	/*
	 * Remove the HT-Control field (if present) on management
	 * frames after we've sent the frame to monitoring. We
	 * (currently) don't need it, and don't properly parse
	 * frames with it present, due to the assumption of a
	 * fixed management header length.
	 */
	if (likely(!ieee80211_is_mgmt(fc) || !ieee80211_has_order(fc))){
		return skb;
	}
	hdrlen = ieee80211_hdrlen(fc);
	hdr->frame_control &= ~cpu_to_le16(IEEE80211_FCTL_ORDER);

	if (!pskb_may_pull(skb, hdrlen)) {
		dev_kfree_skb(skb);
		return NULL;
	}

	memmove(skb->data + IEEE80211_HT_CTL_LEN, skb->data, hdrlen - IEEE80211_HT_CTL_LEN);
	pskb_pull(skb, IEEE80211_HT_CTL_LEN);

	return skb;
}

/*
 * This function copies a received frame to all monitor interfaces and
 * returns a cleaned-up SKB that no longer includes the FCS nor the
 * radiotap header the driver might have added.
 */
static struct sk_buff *ieee80211_rx_monitor(struct ieee80211_local *local, struct sk_buff *origskb, struct ieee80211_rate *rate){
	int rtap_space = 0;
	int min_head_len = 0;
	int present_fcs_len = 0;
	bool only_monitor = false;
	struct sk_buff *skb = NULL;
	struct sk_buff *monskb = NULL;
	struct ieee80211_rx_status *status = NULL;
	struct ieee80211_sub_if_data *sdata = NULL;
	struct ieee80211_sub_if_data *prev_sdata = NULL;

	status = IEEE80211_SKB_RXCB(origskb);
	if (status->flag & RX_FLAG_RADIOTAP_TLV_AT_END && !skb_mac_header_was_set(origskb)) {
		/* with this skb no way to know where frame payload starts */
		dev_kfree_skb(origskb);
		return NULL;
	}

	if (status->flag & RX_FLAG_RADIOTAP_VHT){
		rtap_space += sizeof(struct ieee80211_radiotap_vht);
	}
	if (status->flag & RX_FLAG_RADIOTAP_HE){
		rtap_space += sizeof(struct ieee80211_radiotap_he);
	}
	if (status->flag & RX_FLAG_RADIOTAP_HE_MU){
		rtap_space += sizeof(struct ieee80211_radiotap_he_mu);
	}
	if (status->flag & RX_FLAG_RADIOTAP_LSIG){
		rtap_space += sizeof(struct ieee80211_radiotap_lsig);
	}
	if (status->flag & RX_FLAG_RADIOTAP_TLV_AT_END){
		//	rtap_space += skb_mac_header(origskb) - (&origskb->data + rtap_space);
	}
	min_head_len = rtap_space;

	/*
	 * First, we may need to make a copy of the skb because
	 *  (1) we need to modify it for radiotap (if not present), and
	 *  (2) the other RX handlers will modify the skb we got.
	 *
	 * We don't need to, of course, if we aren't going to return
	 * the SKB because it has a bad FCS/PLCP checksum.
	 */

	if (!(status->flag & RX_FLAG_NO_PSDU)) {
		if (ieee80211_hw_check(&local->hw, RX_INCLUDES_FCS)) {
			if (unlikely(origskb->len <= FCS_LEN + rtap_space)) {
				/* driver bug */
				dev_kfree_skb(origskb);
				return NULL;
			}
			present_fcs_len = FCS_LEN;
		}

		/* also consider the hdr->frame_control */
		min_head_len += 2;
	}
	return ieee80211_clean_skb(origskb, present_fcs_len, rtap_space);
}

static void ieee80211_parse_qos(struct ieee80211_rx_data *rx){
	int tid = 0;
	int seqno_idx = 0;
	int security_idx = 0;
	struct sk_buff *skb = NULL;
	struct ieee80211_hdr *hdr = NULL;
	struct ieee80211_rx_status *status = NULL;

	skb = rx->skb;
	hdr = skb->data;
	status = (struct ieee80211_rx_status *)skb->cb;

	/* does the frame have a qos control field? */
	if (ieee80211_is_data_qos(hdr->frame_control)) {
		u8 *qc = ieee80211_get_qos_ctl(hdr);
		/* frame has qos control */
		tid = *qc & IEEE80211_QOS_CTL_TID_MASK;
		if (*qc & IEEE80211_QOS_CTL_A_MSDU_PRESENT){
			status->rx_flags |= IEEE80211_RX_AMSDU;
		}
		seqno_idx = tid;
		security_idx = tid;
	} else {
		/*
		 * IEEE 802.11-2007, 7.1.3.4.1 ("Sequence Number field"):
		 *
		 *	Sequence numbers for management frames, QoS data
		 *	frames with a broadcast/multicast address in the
		 *	Address 1 field, and all non-QoS data frames sent
		 *	by QoS STAs are assigned using an additional single
		 *	modulo-4096 counter, [...]
		 *
		 * We also use that counter for non-QoS STAs.
		 */
		seqno_idx = IEEE80211_NUM_TIDS;
		security_idx = 0;
		if (ieee80211_is_mgmt(hdr->frame_control)){
			security_idx = IEEE80211_NUM_TIDS;
		}
		tid = 0;
	}

	rx->seqno_idx = seqno_idx;
	rx->security_idx = security_idx;
	/* Set skb->priority to 1d tag if highest order bit of TID is not set.
	 * For now, set skb->priority to 0 for other cases. */
	skb->priority = (tid > 7) ? 0 : tid;
}

static void ieee80211_scan_rx(struct ieee80211_local *local, struct sk_buff *skb){
	int min_hdr_len = 0;
	struct wiphy *wiphy = NULL;
	struct ieee80211_bss *bss = NULL;
	struct ieee80211_ext *ext = NULL;
	struct ieee80211_mgmt *mgmt = NULL;
	struct ieee80211_channel *channel = NULL;
	struct ieee80211_rx_status *rx_status = NULL;

	if (local == NULL || skb == NULL || skb->data == NULL || skb->len < IEEE80211_MIN_HDR_LEN){
		goto end;
	}

	wiphy = local->wiphy;
	mgmt = (void *)skb->data;
	rx_status = (struct ieee80211_rx_status *)skb->cb;
	min_hdr_len = offsetof(struct ieee80211_mgmt, u.probe_resp.variable);

	if (!ieee80211_is_probe_resp(mgmt->frame_control) && !ieee80211_is_beacon(mgmt->frame_control) && !ieee80211_is_s1g_beacon(mgmt->frame_control)){
		goto end;
	}

	if (ieee80211_is_s1g_beacon(mgmt->frame_control)) {
		ext = (struct ieee80211_ext *)mgmt;
		min_hdr_len = offsetof(struct ieee80211_ext, u.s1g_beacon.variable) + ieee80211_s1g_optional_len(ext->frame_control);
	}

	if (skb->data == NULL || skb->len < min_hdr_len || skb->len > 4096){
		goto end;
	}

	if (test_and_clear_bit(SCAN_BEACON_WAIT, &local->scanning)) {
		/*
		 * we were passive scanning because of radar/no-IR, but
		 * the beacon/proberesp rx gives us an opportunity to upgrade
		 * to active scan
		 */
		set_bit(SCAN_BEACON_DONE, &local->scanning);
		/* wiphy_delayed_work_queue(wiphy, &local->scan_work, 0); */
	}

	if (rx_status == NULL || rx_status->freq == 0 || local->hw.wiphy == NULL){
		goto end;
	}

	channel = ieee80211_get_channel_khz(local->hw.wiphy, ieee80211_rx_status_to_khz(rx_status));

	if (!channel || channel->flags & IEEE80211_CHAN_DISABLED){
		goto end;
	}
	if (!ieee80211_is_s1g_beacon(mgmt->frame_control)){
		scan_cache_observe(local, skb, channel, min_hdr_len);
	}

	end:
}

/*
 * This is the actual Rx frames handler. as it belongs to Rx path it must
 * be called with rcu_read_lock protection.
 */
static void __ieee80211_rx_handle_packet(struct ieee80211_hw *hw, struct ieee80211_sta *pubsta, struct sk_buff *skb, struct list_head *list){
	int ret = 0;
	int link_id = -1;
	__le16 fc = 0;
	struct sta_info *sta = NULL;
	struct list_head *pos = NULL;
	struct rhlist_head *tmp = NULL;
	struct sta_info *prev_sta = NULL;
	struct ieee80211_hdr *hdr = NULL;
	struct ieee80211_local *local = NULL;
	struct ieee80211_sub_if_data *prev = NULL;
	struct ieee80211_sub_if_data *sdata = NULL;
	struct ieee80211_rx_status *status = NULL;
	struct ieee80211_vif *ieee80211_vif = NULL;
	struct ieee80211_rx_data rx = {};

	if (skb == NULL){
		goto end;
	}
	if (skb->data == NULL || skb->len < IEEE80211_MIN_HDR_LEN){
		dev_kfree_skb(skb);
		goto end;
	}

	local = director.ieee80211_local;
	if (local == NULL){
		dev_kfree_skb(skb);
		goto end;
	}

	status = IEEE80211_SKB_RXCB(skb);
	fc = ((struct ieee80211_hdr *)skb->data)->frame_control;
	memset(&rx, 0, sizeof(rx));
	rx.skb = skb;
	rx.local = local;
	rx.list = list;
	rx.link_id = -1;

	if (ieee80211_is_mgmt(fc)) {
		if (skb->len < ieee80211_hdrlen(fc)){
			ret = -ENOBUFS;
		}else{
			ret = skb_linearize(skb);
		}
	}else{
		ret = !pskb_may_pull(skb, ieee80211_hdrlen(fc));
	}

	if (ret) {
		dev_kfree_skb(skb);
		goto end;
	}

	hdr = (struct ieee80211_hdr *)skb->data;
	ieee80211_parse_qos(&rx);

	if (unlikely(ieee80211_is_probe_resp(hdr->frame_control) ||
		     ieee80211_is_beacon(hdr->frame_control) ||
		     ieee80211_is_s1g_beacon(hdr->frame_control))){
		ieee80211_scan_rx(local, skb);
	}

	if (status != NULL && status->link_valid){
		link_id = status->link_id;
	}

	if (ieee80211_is_data(fc)) {
		sta = NULL;
		prev_sta = NULL;

		if (pubsta != NULL) {
			for (tmp = sta_info_hash_lookup(local, hdr->addr2); tmp != NULL; tmp = tmp->next) {
				sta = (struct sta_info *)((char *)tmp - offsetof(struct sta_info, hash_node));
				if (sta->sta == pubsta){
					break;
				}
				sta = NULL;
			}

			if (sta != NULL) {
				if (!ieee80211_rx_data_set_sta(&rx, sta, link_id)){
					goto out;
				}
				if (status != NULL && !status->link_valid && sta->sta != NULL) {
					struct link_sta_info *link_sta = link_sta_info_get_bss(rx.sdata, hdr->addr2);
					if (link_sta != NULL){
						ieee80211_rx_data_set_link(&rx, link_sta->link_id);
					}
				}
				if (ieee80211_prepare_and_rx_handle(&rx, skb, true)){
					skb = NULL;
					goto end;
				}
				goto out;
			}
		}
		for (tmp = sta_info_hash_lookup(local, hdr->addr2);
		     tmp != NULL && (sta = (struct sta_info *)((char *)tmp - offsetof(struct sta_info, hash_node)), 1);
		     tmp = tmp->next) {
			int sta_link_id = link_id;

			if (prev_sta == NULL) {
				prev_sta = sta;
				continue;
			}

			rx.sdata = &prev_sta->sdata;
			if (status != NULL && !status->link_valid && prev_sta->sta != NULL) {
				struct link_sta_info *link_sta = link_sta_info_get_bss(rx.sdata, hdr->addr2);
				if (link_sta != NULL){
					sta_link_id = link_sta->link_id;
				}
			}
			if (!ieee80211_rx_data_set_sta(&rx, prev_sta, sta_link_id)){
				goto out;
			}

			ieee80211_prepare_and_rx_handle(&rx, skb, false);
			prev_sta = sta;
		}

		if (prev_sta != NULL) {
			int sta_link_id = link_id;

			rx.sdata = &prev_sta->sdata;
			if (status != NULL && !status->link_valid && prev_sta->sta != NULL) {
				struct link_sta_info *link_sta = link_sta_info_get_bss(rx.sdata, hdr->addr2);
				if (link_sta != NULL){
					sta_link_id = link_sta->link_id;
				}
			}
			if (!ieee80211_rx_data_set_sta(&rx, prev_sta, sta_link_id)){
				goto out;
			}

			if (ieee80211_prepare_and_rx_handle(&rx, skb, true)){
				skb = NULL;
				goto end;
			}
			goto out;
		}
	}

	prev = NULL;
	for (pos = local->interfaces.next; pos != &local->interfaces; pos = pos->next) {
		sdata = (struct ieee80211_sub_if_data *)pos;
		if (sdata == NULL || sdata->vif == NULL){
			continue;
		}
		ieee80211_vif = sdata->vif;
		if (!ieee80211_sdata_running(sdata)){
			continue;
		}
		if (ieee80211_vif->type == NL80211_IFTYPE_MONITOR || ieee80211_vif->type == NL80211_IFTYPE_AP_VLAN){
			continue;
		}

		if (prev == NULL){
			prev = sdata;
			continue;
		}

		rx.sdata = prev;
		ieee80211_rx_for_interface(&rx, skb, false);
		prev = sdata;
	}

	if (prev != NULL) {
		rx.sdata = prev;
		if (ieee80211_rx_for_interface(&rx, skb, true)){
			skb = NULL;
			goto end;
		}
	}
	out:
	end:
	if(skb != NULL){
		dev_kfree_skb(skb);
	}
}

static void __ieee80211_rx_handle_8023(struct ieee80211_hw *hw, struct ieee80211_sta *pubsta, struct sk_buff *skb, struct list_head *list){
	int link_id = -1;
	struct sta_info *sta = NULL;
	struct rhlist_head *tmp = NULL;
	struct ieee80211_local *local = NULL;
	struct ieee80211_rx_status *status = NULL;
	struct ieee80211_rx_data rx = {};

	status = IEEE80211_SKB_RXCB(skb);
	local = director.ieee80211_local;
	memset(&rx, 0, sizeof(rx));
	rx.skb = skb;
	rx.local = local;
	rx.list = list;
	rx.link_id = -1;

	if (skb->len < sizeof(struct ethhdr)){
		goto drop;
	}
	if (pubsta == NULL){
		goto drop;
	}
	if (status->link_valid){
		link_id = status->link_id;
	}
	/*
	 * TODO: Should the frame be dropped if the right link_id is not
	 * available? Or may be it is fine in the current form to proceed with
	 * the frame processing because with frame being in 802.3 format,
	 * link_id is used only for stats purpose and updating the stats on
	 * the deflink is fine?
	 */
	for (tmp = sta_info_hash_lookup(local, ((struct ethhdr *)skb->data)->h_source);
	     tmp; tmp = tmp->next) {
		sta = (struct sta_info *)((char *)tmp - offsetof(struct sta_info, hash_node));
		if (sta->sta == pubsta)
			break;
		sta = NULL;
	}
	if (!sta){
		goto drop;
	}
	if (!ieee80211_rx_data_set_sta(&rx, sta, link_id)){
		goto drop;
	}
	ieee80211_rx_8023(&rx, NULL, skb->len);
	goto end;
	drop:
	if(skb != NULL){
		dev_kfree_skb(skb);
	}
	end:
}

/* nl80211 based function wrappers (must be working for both Linux and Windows) */
static void nl80211_gtk_rekey_notify(struct cfg80211_registered_device *rdev, struct net_device *netdev, u8 *bssid, u8 *replay_ctr, int gfp){

}

/* functions for dumping ieee80211 skb */
static inline void rx_dump_skb(struct sk_buff *skb, struct ieee80211_rx_status *status){
	u16 fc = 0;
	u32 sequence = 0;
	u32 dump_len = 0;
	struct ethhdr *eth = NULL;
	struct ieee80211_hdr *hdr = NULL;

	/* check if the 'ops rx on' command is enabled or not */
	if(!atomic_load(&director.rx_dump_enabled) || skb == NULL || skb->data == NULL || skb->len == 0){
		goto end;
	}

	dump_len = skb_headlen(skb);
	sequence = atomic_fetch_add(&director.rx_dump_sequence, 1) + 1;
	if(status != NULL && (status->flag & RX_FLAG_8023)){
		eth = NULL;
		printf("\n[RX %06u] format=802.3 len=%u freq=%uMHz signal=%ddBm band=%u rate_idx=%u encoding=%u\n",
		       sequence, skb->len, status->freq, status->signal, status->band, status->rate_idx, status->encoding);
		if(dump_len >= sizeof(*eth)){
			eth = (struct ethhdr *)skb->data;
			printf("  dst=%02x:%02x:%02x:%02x:%02x:%02x src=%02x:%02x:%02x:%02x:%02x:%02x proto=0x%04x\n",
			       sequence,
			       eth->h_dest[0], eth->h_dest[1], eth->h_dest[2],
			       eth->h_dest[3], eth->h_dest[4], eth->h_dest[5],
			       eth->h_source[0], eth->h_source[1], eth->h_source[2],
			       eth->h_source[3], eth->h_source[4], eth->h_source[5],
			       le16_to_cpu(eth->h_proto));
		}
		print_hex_dump("", "  ", DUMP_PREFIX_OFFSET, 16, 1, skb->data, dump_len, true);
		goto end;
	}

	if(dump_len < IEEE80211_MIN_HDR_LEN){
		printf("\n  format=802.11 truncated len=%u\n", sequence, skb->len);
		print_hex_dump("", "  ", DUMP_PREFIX_OFFSET, 16, 1, skb->data, dump_len, true);
		goto end;
	}

	hdr = (struct ieee80211_hdr *)skb->data;
	fc = le16_to_cpu(hdr->frame_control);
	printf("\n[RX %06u] format=802.11 subtype=0x%02x fc=0x%04x len=%u", sequence, fc & IEEE80211_FCTL_STYPE, fc, skb->len);
	if(status != NULL){
		printf(" freq=%uMHz signal=%d dBm band=%u bw=%u", status->freq, status->signal, status->band, status->bw);
	}
	printf("\n  addr1=%02x:%02x:%02x:%02x:%02x:%02x", sequence, hdr->addr1[0], hdr->addr1[1], hdr->addr1[2],
		hdr->addr1[3], hdr->addr1[4], hdr->addr1[5]);
	if(dump_len >= 16){
		printf(" addr2=%02x:%02x:%02x:%02x:%02x:%02x", hdr->addr2[0], hdr->addr2[1], hdr->addr2[2],
		       hdr->addr2[3], hdr->addr2[4], hdr->addr2[5]);
	}
	if(dump_len >= 22){
		printf(" addr3=%02x:%02x:%02x:%02x:%02x:%02x",
		       hdr->addr3[0], hdr->addr3[1], hdr->addr3[2],
		       hdr->addr3[3], hdr->addr3[4], hdr->addr3[5]);
	}
	printf("\n");
	print_hex_dump("", "  ", DUMP_PREFIX_OFFSET, 16, 1, skb->data, dump_len, true);
	end:
}

/* bss_conf_init: fill 'struct ieee80211_bss_conf' */
static int bss_conf_init(struct ieee80211_hw *ieee80211_hw, struct ieee80211_vif *ieee80211_vif){
	int ret = 0;
	struct ieee80211_local *local = NULL;
	struct ieee80211_bss_conf *bss_conf = NULL;

	local = &director.ieee80211_local;
	bss_conf = local->bss_conf;
	if(bss_conf != NULL){
		/* quit, bss_conf is already allocated! */
		ret = -EINVAL;
		goto end;
	}
	bss_conf = calloc(1, director.struct_sizes.ieee80211_bss_conf);
	bss_conf->vif      = ieee80211_vif;
	bss_conf->link_id  = 0;
	memcpy(bss_conf->addr, ieee80211_vif->addr, ETH_ALEN);

	bss_conf->beacon_int      = 100;
	bss_conf->dtim_period     = 1;
	bss_conf->sync_tsf        = 0;
	bss_conf->sync_device_ts  = 0;
	bss_conf->sync_dtim_count = 0;

	bss_conf->assoc_capability = WLAN_CAPABILITY_ESS | WLAN_CAPABILITY_SHORT_SLOT_TIME | WLAN_CAPABILITY_SHORT_PREAMBLE;
	bss_conf->use_cts_prot      = false;
	bss_conf->use_short_preamble= true;
	bss_conf->use_short_slot    = true;

	bss_conf->txpower     	= 20;
	bss_conf->txpower_type	= NL80211_TX_POWER_LIMITED;

	bss_conf->basic_rates = BIT(0) | BIT(1) | BIT(2) | BIT(3);
	bss_conf->beacon_rate = NULL;
	bss_conf->ht_operation_mode = IEEE80211_HT_OP_MODE_PROTECTION_NONE;
	bss_conf->he_support  = true;
	bss_conf->eht_support = false;

	bss_conf->arp_addr_cnt     = 0;

	/* disable the power saving feature */
	bss_conf->ps = false;
	bss_conf->idle = false;
	bss_conf->qos = false;

	bss_conf->cqm_rssi_thold = -70;
	bss_conf->cqm_rssi_hyst  = 4;
	bss_conf->cqm_rssi_low   = -80;
	bss_conf->cqm_rssi_high  = -60;

	bss_conf->mcast_rate[NL80211_BAND_2GHZ] = 0;
	bss_conf->mcast_rate[NL80211_BAND_5GHZ] = 0;
	bss_conf->mcast_rate[NL80211_BAND_6GHZ] = 0;

	bss_conf->twt_requester = false;
	bss_conf->twt_responder = false;
	bss_conf->twt_protected = false;
	bss_conf->twt_broadcast = false;

	bss_conf->ftm_responder        = false;
	bss_conf->ftm_responder_params = NULL;

	bss_conf->frame_time_rts_th = 0;
	bss_conf->multicast_to_unicast = false;
	bss_conf->enable_beacon = false;

	end:
	return ret;
}

/* wifi_scan_thread_sleep: timeout function used for sleeping before invoking a new Wi-Fi scan */
static void wifi_scan_thread_sleep(void){
        for (u8 i = 0; i < 50; i++){
                if (atomic_load_explicit(&director.wifi_scan_thread_stop, memory_order_acquire)){
                        break;
                }
                usleep(WIFI_SCAN_THREAD_INTERVAL / 50);
        }
}

/* wifi_scan_thread: main Wi-Fi scan thread, invoke a Wi-Fi scan every 5 seconds (default value, can be changed) */
static void *wifi_scan_thread(void *arg){
        while (!atomic_load_explicit(&director.wifi_scan_thread_stop, memory_order_acquire)){
                wifi_scan_thread_sleep();
                if (atomic_load_explicit(&director.wifi_scan_thread_stop, memory_order_acquire)){
                        break;
                }
                if (atomic_load(&director.scan_in_progress)){
                        continue;
                }
                /* decide if use cfg80211 or mac80211 scan */
                if (director.ieee80211_ops != NULL && director.ieee80211_local != NULL){
                        fast_run_ieee80211_cb(IEEE80211_HW_SCAN);
                }else if (director.cfg80211_ops != NULL && director.wiphy != NULL){
                        fast_run_cfg80211_cb(CFG80211_SCAN);
                }
        }
        return NULL;
}

/* run_wifi_scan_thread: start the Wi-Fi scan thread */
static int run_wifi_scan_thread(void){
        int ret = 0;
        bool expected = false;

        if (!atomic_compare_exchange_strong(&director.wifi_scan_thread_started, &expected, true)){
                return ret;
        }

        atomic_store_explicit(&director.wifi_scan_thread_stop, false, memory_order_release);
        ret = pthread_create(&director.wifi_scan_thread, NULL, wifi_scan_thread, NULL);
        if (ret != 0){
                atomic_store(&director.wifi_scan_thread_started, false);
                director.wifi_scan_thread = 0;
        }
        return ret;
}

static void stop_wifi_scan_thread(void){
	atomic_store_explicit(&director.wifi_scan_thread_stop, true, memory_order_release);
	if (director.wifi_scan_thread != 0 && atomic_load(&director.wifi_scan_thread_started)) {
		pthread_join(director.wifi_scan_thread, NULL);
		director.wifi_scan_thread = 0;
	}
	atomic_store(&director.wifi_scan_thread_started, false);
}

static int fill_cfg80211_start_scan_req(struct cfg80211_scan_request *cfg80211_scan_req){
	int ret = 0;
	int n_channels = 0;
	struct wiphy *wiphy = NULL;
	struct wireless_dev *wdev = NULL;
	struct net_device *net_device = NULL;
	struct ieee80211_local *local = NULL;
	struct ieee80211_scan_ies *ies = NULL;
	struct ieee80211_vif *ieee80211_vif = NULL;
	struct ieee80211_supported_band *sband = NULL;
	static struct cfg80211_ssid wildcard_ssid = {
		.ssid_len = 0,
	};

	/*
	 * Keep probe requests conservative and keep them in static storage: the
	 * fullmac cfg80211 path can retain the scan request asynchronously.
	 */
	static const u8 scan_ies_combined[] = {
		WLAN_EID_SUPP_RATES, 8,
		0x82, 0x84, 0x8b, 0x96, 0x0c, 0x12, 0x18, 0x24,
		WLAN_EID_EXT_SUPP_RATES, 4,
		0x30, 0x48, 0x60, 0x6c,
		WLAN_EID_SUPP_RATES, 8,
		0x8c, 0x12, 0x98, 0x24, 0xb0, 0x48, 0x60, 0x6c,
	};

	if(director.wiphy == NULL){
		ret = -EINVAL;
		return ret;
	}
	wiphy = director.wiphy;
	if(director.net_device == NULL){
		ret = -EINVAL;
		return ret;
	}
	net_device = director.net_device;
	if(net_device->ieee80211_ptr == NULL){
		if(director.wdev == NULL){
			ret = -EINVAL;
			return ret;
		}
		wdev = director.wdev;
	}else{
		wdev = net_device->ieee80211_ptr;
	}

	cfg80211_scan_req->ie = scan_ies_combined;
	cfg80211_scan_req->ie_len = sizeof(scan_ies_combined);
	cfg80211_scan_req->ssids = &wildcard_ssid;
	cfg80211_scan_req->n_ssids = 1;
	cfg80211_scan_req->n_channels = 0;
	for (enum nl80211_band band = 0; band < NUM_NL80211_BANDS; band++) {
		sband = wiphy->bands[band];
		if (sband == NULL || band == NL80211_BAND_6GHZ){
			continue;
		}
		for (int i = 0; i < sband->n_channels; i++) {
			if (sband->channels[i].flags & IEEE80211_CHAN_DISABLED){
				continue;
			}
			n_channels = cfg80211_scan_req->n_channels++;
			cfg80211_scan_req->channels[n_channels] = &sband->channels[i];
		}
	}

	cfg80211_scan_req->duration = 0;
	cfg80211_scan_req->duration_mandatory = false;
	cfg80211_scan_req->scan_start = 0;
	cfg80211_scan_req->flags = 0;

	for (enum nl80211_band band = 0; band < NUM_NL80211_BANDS; band++){
		cfg80211_scan_req->rates[band] = wiphy->bands[band] ? (u32)-1 : 0;
	}

	memset(cfg80211_scan_req->mac_addr, 0, ETH_ALEN);
	memset(cfg80211_scan_req->mac_addr_mask, 0, ETH_ALEN);
	eth_broadcast_addr(cfg80211_scan_req->bssid);

	cfg80211_scan_req->no_cck = false;
	cfg80211_scan_req->scan_6ghz = false;
	cfg80211_scan_req->first_part = true;
	cfg80211_scan_req->n_6ghz_params = 0;
	cfg80211_scan_req->scan_6ghz_params = NULL;
	cfg80211_scan_req->tsf_report_link_id = -1;

	cfg80211_scan_req->wiphy = wiphy;
	cfg80211_scan_req->wdev = wdev;

	return ret;
}

static int fill_cfg80211_start_ap_req(struct cfg80211_ap_settings *ap_settings){
	int ret = 0;
	u8 ch = 1;
	static u8 tail[6];
	static u8 head[83];
	u8 *head_ptr = NULL;
	struct wiphy *wiphy = NULL;
	struct ieee80211_channel *chan = NULL;
	struct cfg80211_beacon_data *beacon_data = NULL;

	u8 xrates[4]         = {0x30,0x48,0x60,0x6c};
	u8 brd[ETH_ALEN]     = {0xff,0xff,0xff,0xff,0xff,0xff};
	u8 ap_addr[ETH_ALEN] = {0x02,0x00,0x00,0x52,0xc0,0x01};
	u8 rates[8]          = {0x82,0x84,0x8b,0x96,0x0c,0x12,0x18,0x24};

	head_ptr = head;
	chan = ieee80211_get_channel(director.wiphy, ieee80211_channel_to_frequency(ch, NL80211_BAND_2GHZ));
	/* generate the beacon header */
	put_unaligned_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_BEACON, head_ptr);
	head_ptr += 2;
	put_unaligned_le16(0, head_ptr);
	head_ptr += 2;
	ether_addr_copy(head_ptr, brd);
	head_ptr += ETH_ALEN;
	ether_addr_copy(head_ptr, ap_addr);
	head_ptr += ETH_ALEN;
	ether_addr_copy(head_ptr, ap_addr);
	head_ptr += ETH_ALEN;
	put_unaligned_le16(0, head_ptr);
	head_ptr += 2;

	memset(head_ptr, 0, 8);
	head_ptr += 8;
	put_unaligned_le16(100, head_ptr);
	head_ptr += 2;
	put_unaligned_le16(WLAN_CAPABILITY_ESS | WLAN_CAPABILITY_SHORT_PREAMBLE, head_ptr);
	head_ptr += 2;

	/* apply the ssid */
	*head_ptr++ = WLAN_EID_SSID;
	*head_ptr++ = director.ap_ssid_len;
	memcpy(head_ptr, director.ap_ssid, director.ap_ssid_len);
	head_ptr += director.ap_ssid_len;

	/* insert the supported rates */
	*head_ptr++ = WLAN_EID_SUPP_RATES;
	*head_ptr++ = sizeof(rates);
	memcpy(head_ptr, rates, sizeof(rates));
	head_ptr += sizeof(rates);

	*head_ptr++ = WLAN_EID_DS_PARAMS;
	*head_ptr++ = 1;
	*head_ptr++ = ch;

	tail[0] = WLAN_EID_EXT_SUPP_RATES;
	tail[1] = sizeof(xrates);
	memcpy(&tail[2], xrates, sizeof(xrates));

	beacon_data = &ap_settings->beacon;
	beacon_data->head = head;
	beacon_data->head_len = head_ptr - head;
	beacon_data->tail = tail;
	beacon_data->tail_len = sizeof(tail);

	ap_settings->ssid = director.ap_ssid;
	ap_settings->ssid_len = director.ap_ssid_len;

	ap_settings->hidden_ssid = NL80211_HIDDEN_SSID_NOT_IN_USE;
	ap_settings->auth_type = NL80211_AUTHTYPE_OPEN_SYSTEM;
	ap_settings->beacon_interval = 100;
	ap_settings->dtim_period = 2;
	ap_settings->privacy = false;

	ap_settings->chandef.chan = chan;
	ap_settings->chandef.width = NL80211_CHAN_WIDTH_20;
	ap_settings->chandef.center_freq1 = chan->center_freq;

	return ret;
}

/* fill_start_ap_req: fill the 'start_ap' callback parameters */
static int fill_start_ap_req(struct ieee80211_bss_conf *link_conf){
	int ret = 0;
	struct wiphy *wiphy = NULL;
	struct ieee80211_local *local = NULL;
	struct ieee80211_channel *chan = NULL;

	if(link_conf == NULL){
		ret = -EINVAL;
		return ret;
	}

	local = director.ieee80211_local;

	link_conf->vif = local->ieee80211_vif;
	link_conf->link_id = 0;

	link_conf->beacon_int    = 100;
	link_conf->dtim_period   = 2;
	link_conf->enable_beacon = true;
	link_conf->hidden_ssid   = NL80211_HIDDEN_SSID_NOT_IN_USE;
	wiphy = local->wiphy;
	for(int i = 0; i < NUM_NL80211_BANDS; i++){
		if(wiphy->bands[i] && wiphy->bands[i]->n_channels > 0){
			chan = &wiphy->bands[i]->channels[0];
			break;
		}
	}

	if(chan != NULL){
		link_conf->chanreq.oper.chan         = chan;
		link_conf->chanreq.oper.width        = NL80211_CHAN_WIDTH_20;
		link_conf->chanreq.oper.center_freq1 = chan->center_freq;
		link_conf->chanreq.oper.center_freq2 = 0;
	}

	link_conf->basic_rates       = BIT(0) | BIT(1) | BIT(2) | BIT(3);
	link_conf->ht_operation_mode = 0;
	link_conf->qos               = false;
	link_conf->txpower           = 0;
	link_conf->txpower_type      = NL80211_TX_POWER_AUTOMATIC;
	link_conf->allow_p2p_go_ps   = false;
	return ret;
}

static u32 hw_scan_channel_count(struct wiphy *wiphy){
	int ret = 0;
	u32 count = 0;
	struct ieee80211_supported_band *sband = NULL;

	if (wiphy == NULL){
		return ret;
	}

	for (enum nl80211_band band = 0; band < NUM_NL80211_BANDS; band++) {
		sband = wiphy->bands[band];

		if (sband == NULL || band == NL80211_BAND_6GHZ){
			continue;
		}
		for (int i = 0; i < sband->n_channels; i++) {
			if (!(sband->channels[i].flags & IEEE80211_CHAN_DISABLED)){
				count++;
			}
		}
	}
	return count;
}

/* fill_hw_scan_req: fill the 'hw_scan' request for the ieee80211 layer */
static int fill_hw_scan_req(struct ieee80211_scan_request *scan_req){
	int ret = 0;
	int n_channels = 0;
	struct wiphy *wiphy = NULL;
	struct wireless_dev *wdev = NULL;
	struct ieee80211_local *local = NULL;
	struct ieee80211_scan_ies *ies = NULL;
	struct ieee80211_vif *ieee80211_vif = NULL;
	struct ieee80211_supported_band *sband = NULL;
	struct cfg80211_scan_request *cfg80211_scan_req = NULL;
	static struct cfg80211_ssid wildcard_ssid = {
		.ssid_len = 0,
	};

	/*
	 * Keep probe requests conservative and keep them in static storage: the
	 * hw_scan request is retained asynchronously by drivers such as rtw89.
	 */
	static const u8 scan_ies_2ghz[] = {
		WLAN_EID_SUPP_RATES, 8,
		0x82, 0x84, 0x8b, 0x96, 0x0c, 0x12, 0x18, 0x24,
		WLAN_EID_EXT_SUPP_RATES, 4,
		0x30, 0x48, 0x60, 0x6c,
	};
	static const u8 scan_ies_5ghz[] = {
		WLAN_EID_SUPP_RATES, 8,
		0x8c, 0x12, 0x98, 0x24, 0xb0, 0x48, 0x60, 0x6c,
	};
	static const u8 scan_ies_combined[] = {
		WLAN_EID_SUPP_RATES, 8,
		0x82, 0x84, 0x8b, 0x96, 0x0c, 0x12, 0x18, 0x24,
		WLAN_EID_EXT_SUPP_RATES, 4,
		0x30, 0x48, 0x60, 0x6c,
		WLAN_EID_SUPP_RATES, 8,
		0x8c, 0x12, 0x98, 0x24, 0xb0, 0x48, 0x60, 0x6c,
	};

	if (scan_req == NULL){
		ret = -EINVAL;
		return ret;
	}
	local = director.ieee80211_local;
	if (local == NULL || local->wiphy == NULL){
		ret = -EINVAL;
		return ret;
	}
	wiphy = local->wiphy;

	memset(scan_req, 0, director.struct_sizes.ieee80211_scan_request);

	ies = &scan_req->ies;
	ies->ies[NL80211_BAND_2GHZ] = scan_ies_combined;
	ies->len[NL80211_BAND_2GHZ] = sizeof(scan_ies_2ghz);

	ies->ies[NL80211_BAND_5GHZ] = scan_ies_combined + sizeof(scan_ies_2ghz);
	ies->len[NL80211_BAND_5GHZ] = sizeof(scan_ies_5ghz);

	ies->common_ies = NULL;
	ies->common_ie_len = 0;

	cfg80211_scan_req = &scan_req->req;
	cfg80211_scan_req->ie = scan_ies_combined;
	cfg80211_scan_req->ie_len = sizeof(scan_ies_combined);

	cfg80211_scan_req->ssids = &wildcard_ssid;
	cfg80211_scan_req->n_ssids = 1;

	cfg80211_scan_req->n_channels = 0;
	for (enum nl80211_band band = 0; band < NUM_NL80211_BANDS; band++) {
		sband = wiphy->bands[band];
		if (sband == NULL || band == NL80211_BAND_6GHZ){
			continue;
		}
		for (int i = 0; i < sband->n_channels; i++) {
			if (sband->channels[i].flags & IEEE80211_CHAN_DISABLED){
				continue;
			}
			n_channels = cfg80211_scan_req->n_channels++;
			cfg80211_scan_req->channels[n_channels] = &sband->channels[i];
		}
	}

	cfg80211_scan_req->duration = 0;
	cfg80211_scan_req->duration_mandatory = false;
	cfg80211_scan_req->scan_start = 0;
	cfg80211_scan_req->flags = 0;

	for (enum nl80211_band band = 0; band < NUM_NL80211_BANDS; band++){
		cfg80211_scan_req->rates[band] = wiphy->bands[band] ? (u32)-1 : 0;
	}

	memset(cfg80211_scan_req->mac_addr, 0, ETH_ALEN);
	memset(cfg80211_scan_req->mac_addr_mask, 0, ETH_ALEN);
	eth_broadcast_addr(cfg80211_scan_req->bssid);

	cfg80211_scan_req->no_cck = false;
	cfg80211_scan_req->scan_6ghz = false;
	cfg80211_scan_req->first_part = true;
	cfg80211_scan_req->n_6ghz_params = 0;
	cfg80211_scan_req->scan_6ghz_params = NULL;
	cfg80211_scan_req->tsf_report_link_id = -1;

	cfg80211_scan_req->wiphy = wiphy;

	if (local->ieee80211_vif) {
		ieee80211_vif = local->ieee80211_vif;
		wdev = &ieee80211_vif->wdev;
	}

	cfg80211_scan_req->wdev = wdev;
	return ret;
}

/* start_hw_scan: fill the parameters and invoke the 'hw_scan' callback from 'ieee80211_ops' */
static int start_hw_scan(struct ieee80211_local *local,
			 int (*hw_scan)(struct ieee80211_hw *hw, struct ieee80211_vif *vif, struct ieee80211_scan_request *req),
			 void (*cancel_hw_scan)(struct ieee80211_hw *hw, struct ieee80211_vif *vif)){
	int ret = 0;
	u32 n_channels = 0;
	struct ieee80211_scan_request *scan_req = NULL;

	if (local == NULL || local->wiphy == NULL || local->ieee80211_hw == NULL || local->ieee80211_vif == NULL || hw_scan == NULL){
		ret = -EINVAL;
		return ret;
	}
	if (atomic_load(&director.scan_in_progress)){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] hw_scan is already in progress\n");
		#endif
		ret = -EBUSY;
		return ret;
	}

	n_channels = hw_scan_channel_count(local->wiphy);
	if (n_channels == 0){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] hw_scan: no enabled channels are registered\n");
		#endif
		ret = -EINVAL;
		return ret;
	}

	free(director.active_scan_req);
	director.active_scan_req = calloc(1, director.struct_sizes.ieee80211_scan_request + n_channels * sizeof(struct ieee80211_channel));
	if (director.active_scan_req == NULL){
		ret = -ENOMEM;
		return ret;
	}

	scan_req = director.active_scan_req;
	ret = fill_hw_scan_req(scan_req);
	if (ret != 0){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] hw_scan request initialization failed: %d\n", ret);
		#endif
		free(director.active_scan_req);
		director.active_scan_req = NULL;
		return ret;
	}

	clear_bit(SCAN_COMPLETED, &local->scanning);
	clear_bit(SCAN_ABORTED, &local->scanning);
	clear_bit(SCAN_HW_CANCELLED, &local->scanning);

	scan_cache_reset();
	atomic_store(&director.scan_in_progress, true);

	ret = hw_scan(local->ieee80211_hw, local->ieee80211_vif, scan_req);
	if (ret == 0){
		scan_watchdog_start(local, cancel_hw_scan);
	}
	if (ret != 0){
		atomic_store(&director.scan_in_progress, false);
		#ifdef ENABLE_DBG_PRINT
		printf("[!] hw_scan could not start!\n");
		#endif
	}
	return ret;
}

/* start_cfg80211_scan: invoke directly the cfg80211 scan callback */
static int start_cfg80211_scan(struct wiphy *wiphy, struct cfg80211_ops *cfg80211_ops){
        int ret = 0;
        u32 n_channels = 0;
        size_t scan_req_size = 0;
        struct cfg80211_scan_request *scan_req = NULL;
        int (*scan)(struct wiphy *wiphy, struct cfg80211_scan_request *request) = NULL;

        if (wiphy == NULL || cfg80211_ops == NULL || cfg80211_ops->scan == NULL){
                ret = -EINVAL;
                return ret;
        }
        if (atomic_load(&director.scan_in_progress)){
                ret = -EBUSY;
                return ret;
        }

        n_channels = hw_scan_channel_count(wiphy);
        if (n_channels == 0){
                #ifdef ENABLE_DBG_PRINT
                printf("[!] cfg80211 scan: no enabled channels are registered\n");
                #endif
                ret = -EINVAL;
                return ret;
        }

        scan_req_size = director.struct_sizes.cfg80211_scan_request + n_channels * sizeof(struct ieee80211_channel *);
        free(director.active_scan_req);
        director.active_scan_req = calloc(1, scan_req_size);
        if (director.active_scan_req == NULL){
                ret = -ENOMEM;
                return ret;
        }

        scan_req = director.active_scan_req;
        ret = fill_cfg80211_start_scan_req(scan_req);
        if (ret < 0){
                #ifdef ENABLE_DBG_PRINT
                printf("[!] failed to create the cfg80211 scan request!\n");
                #endif
                free(director.active_scan_req);
                director.active_scan_req = NULL;
                return ret;
        }

        scan_cache_reset();
        atomic_store(&director.scan_in_progress, true);
        scan = cfg80211_ops->scan;
        ret = scan(wiphy, scan_req);
        if (ret == 0){
                run_wifi_scan_thread();
        }else{
                atomic_store(&director.scan_in_progress, false);
                free(director.active_scan_req);
                director.active_scan_req = NULL;
        }
        return ret;
}

static struct wireless_dev *cfg80211_add_virtual_intf(struct wiphy *wiphy, struct cfg80211_ops *cfg_ops){
	struct wireless_dev *wdev = NULL;
	struct net_device *net_device = NULL;
	struct wireless_dev *(*add_virtual_intf)(struct wiphy *wiphy, const char *name, unsigned char name_assign_type, enum nl80211_iftype type, struct vif_params *params) = NULL;
	struct vif_params params = {};

	add_virtual_intf = cfg_ops->add_virtual_intf;
	if(add_virtual_intf != NULL){
		params.flags = 0;
		params.use_4addr = 0;
		net_device = director.net_device;
		if(net_device != NULL){
			memcpy(params.macaddr, director.net_device, ETH_ALEN);
		}else{
			memset(params.macaddr, 0xFF, ETH_ALEN);
		}
		wdev = add_virtual_intf(wiphy, "wlan", 0, NL80211_IFTYPE_STATION, &params);
		return wdev;
	}else{
		return NULL;
	}
}
#endif
