#define _GNU_SOURCE
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/random.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include "wifi_profile.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uint16_t __le16;
#define ETH_ALEN 6
#define IEEE80211_MAX_SSID_LEN 32
#define GFP_ATOMIC 0
#define min(a,b) ((a) < (b) ? (a) : (b))
#define cpu_to_le16(v) ((uint16_t)(v))
#define le16_to_cpu(v) ((uint16_t)(v))
#define IEEE80211_FCTL_FTYPE 0x000c
#define IEEE80211_FCTL_STYPE 0x00f0
#define IEEE80211_FCTL_TODS 0x0100
#define IEEE80211_FCTL_FROMDS 0x0200
#define IEEE80211_FCTL_PROTECTED 0x4000
#define IEEE80211_FTYPE_MGMT 0
#define IEEE80211_FTYPE_DATA 8
#define IEEE80211_STYPE_ASSOC_REQ 0
#define IEEE80211_STYPE_ASSOC_RESP 0x10
#define IEEE80211_STYPE_AUTH 0xb0
#define IEEE80211_STYPE_DEAUTH 0xc0
#define IEEE80211_STYPE_DISASSOC 0xa0
#define IEEE80211_STYPE_DATA 0
#define WLAN_STATUS_SUCCESS 0
#define WLAN_CAPABILITY_ESS 1
#define WLAN_CAPABILITY_PRIVACY 0x10
#define WLAN_CAPABILITY_SHORT_PREAMBLE 0x20
#define WLAN_CAPABILITY_SHORT_SLOT_TIME 0x400
#define WLAN_EID_SSID 0
#define WLAN_EID_SUPP_RATES 1
#define WLAN_EID_EXT_SUPP_RATES 50
#define WLAN_EID_VENDOR_SPECIFIC 221
#define IEEE80211_NUM_ACS 4
#define IEEE80211_INVAL_HW_QUEUE 255
#define IEEE80211_TX_CTL_FIRST_FRAGMENT 1
#define IEEE80211_TX_INTFL_DONT_ENCRYPT 2
#define IEEE80211_TX_CTL_USE_MINRATE 4
#define IEEE80211_TX_CTL_REQ_TX_STATUS 8
#define IEEE80211_TX_INTFL_MLME_CONN_TX 16
#define IEEE80211_TX_STAT_ACK 32
#define IEEE80211_TX_CTRL_PORT_CTRL_PROTO 1
#define IEEE80211_TX_CTRL_MLO_LINK_UNSPEC 0xf0000000U
#define RX_FLAG_8023 (1U << 30)
#define RX_FLAG_FAILED_FCS_CRC (1U << 5)
#define RX_FLAG_FAILED_PLCP_CRC (1U << 6)
#define RX_FLAG_DECRYPTED (1U << 1)
#define RX_FLAG_IV_STRIPPED (1U << 4)
enum nl80211_band { NL80211_BAND_2GHZ, NL80211_BAND_5GHZ };
enum ieee80211_ac_numbers { IEEE80211_AC_VO, IEEE80211_AC_VI, IEEE80211_AC_BE, IEEE80211_AC_BK };
struct ieee80211_channel { enum nl80211_band band; };
struct ieee80211_sta { int unused; };
enum ieee80211_sta_state { IEEE80211_STA_NOTEXIST, IEEE80211_STA_NONE, IEEE80211_STA_AUTH, IEEE80211_STA_ASSOC, IEEE80211_STA_AUTHORIZED };
enum set_key_cmd { SET_KEY, DISABLE_KEY };
#define BSS_CHANGED_ASSOC 1
struct list_head { int unused; };
struct rhlist_head { int unused; };
struct rhltable { int unused; };
struct rhashtable_params { size_t head_offset, key_offset, key_len; };
struct sta_info { enum ieee80211_sta_state sta_state; struct list_head list; u8 addr[6]; struct rhlist_head hash_node; };
struct link_sta_info { u8 addr[6]; struct rhlist_head link_hash_node; };
struct ieee80211_chanctx_conf { int unused; };
struct ieee80211_key_conf { int unused; };
struct ieee80211_bss_conf { struct ieee80211_chanctx_conf *chanctx_conf; const u8 *bssid; };
struct ieee80211_vif {
	u8 addr[6], hw_queue[4], cab_queue;
	struct { bool assoc; u16 aid; u8 ap_addr[6], ssid_len; } cfg;
	struct ieee80211_bss_conf bss_conf;
};
struct ieee80211_hw { int unused; };
struct wiphy { int unused; };
struct ieee80211_local {
	struct ieee80211_vif *ieee80211_vif;
	struct ieee80211_hw *ieee80211_hw;
	struct wiphy *wiphy;
	struct { int extra_tx_headroom; } hw;
	struct rhltable sta_hash, link_sta_hash;
	unsigned int num_sta;
};
struct ieee80211_hdr_3addr {
	__le16 frame_control, duration;
	u8 addr1[6], addr2[6], addr3[6];
	__le16 seq_ctrl;
} __attribute__((packed));
#define ieee80211_hdr ieee80211_hdr_3addr
struct ieee80211_mgmt {
	__le16 frame_control, duration;
	u8 da[6], sa[6], bssid[6];
	__le16 seq_ctrl;
	union {
		struct { __le16 auth_alg, auth_transaction, status_code; u8 variable[0]; } auth;
		struct { __le16 capab_info, listen_interval; u8 variable[0]; } assoc_req;
		struct { __le16 capab_info, status_code, aid; u8 variable[0]; } assoc_resp;
	} u;
} __attribute__((packed));
struct ethhdr { u8 h_dest[6], h_source[6]; u16 h_proto; } __attribute__((packed));
struct ieee80211_rx_status { u32 flag; };
struct ieee80211_tx_info {
	u32 flags, status_data, band, hw_queue;
	struct {
		u32 flags;
		struct ieee80211_vif *vif;
		struct { int idx, count; } rates[2];
	} control;
};
struct sk_buff { u8 *head, *data; unsigned int len; int priority; u8 cb[128]; };
struct ieee80211_tx_control { struct ieee80211_sta *sta; };
struct ieee80211_ops {
	void (*tx)(struct ieee80211_hw *, struct ieee80211_tx_control *, struct sk_buff *);
	void (*flush)(struct ieee80211_hw *, struct ieee80211_vif *, u32, bool);
	int (*set_key)(struct ieee80211_hw *, enum set_key_cmd, struct ieee80211_vif *, struct ieee80211_sta *, struct ieee80211_key_conf *);
	int (*sta_state)(struct ieee80211_hw *, struct ieee80211_vif *, struct ieee80211_sta *, enum ieee80211_sta_state, enum ieee80211_sta_state);
	void (*vif_cfg_changed)(struct ieee80211_hw *, struct ieee80211_vif *, u64);
	void (*bss_info_changed)(struct ieee80211_hw *, struct ieee80211_vif *, struct ieee80211_bss_conf *, u64);
	void (*unassign_vif_chanctx)(struct ieee80211_hw *, struct ieee80211_vif *, struct ieee80211_bss_conf *, struct ieee80211_chanctx_conf *);
	void (*remove_chanctx)(struct ieee80211_hw *, struct ieee80211_chanctx_conf *);
};
static struct {
	void *connection_fsm;
	void *connection_retired;
	pthread_mutex_t connection_lock, connection_control_lock;
	pthread_cond_t connection_wake;
	u16 connection_tx_cookie;
	bool connection_runtime_initialized, connection_tx_trace;
	struct ieee80211_ops *ieee80211_ops;
} director;
#define IEEE80211_SKB_CB(s) ((struct ieee80211_tx_info *)(s)->cb)
#define IEEE80211_SKB_RXCB(s) ((struct ieee80211_rx_status *)(s)->cb)
static void setup_percpu(void) {}
static unsigned int removed_hashes, removed_lists, removed_keys, state_transitions;
static int rhltable_remove_simple(struct rhltable *table, struct rhlist_head *node, struct rhashtable_params params) {
	(void)table; (void)node; (void)params; removed_hashes++; return 0;
}
static void list_del_rcu(struct list_head *head) { (void)head; removed_lists++; }
static struct sk_buff *dev_alloc_skb(unsigned int len) {
	struct sk_buff *s = calloc(1, sizeof(*s));
	assert(s);
	s->head = s->data = calloc(1, len);
	assert(s->head);
	return s;
}
static void kfree_skb(struct sk_buff *s) { free(s->head); free(s); }
static void skb_reserve(struct sk_buff *s, unsigned int n) { s->data += n; }
static void *skb_put(struct sk_buff *s, unsigned int n) { void *p = s->data + s->len; s->len += n; return p; }
static void *skb_put_zero(struct sk_buff *s, unsigned int n) { void *p = skb_put(s, n); memset(p, 0, n); return p; }
static void skb_set_queue_mapping(struct sk_buff *s, int ac) { (void)s; (void)ac; }
static unsigned int skb_headlen(struct sk_buff *s) { return s->len; }
static struct sk_buff *skb_copy(struct sk_buff *s, int flags) {
	(void)flags;
	struct sk_buff *copy = dev_alloc_skb(s->len);
	memcpy(skb_put(copy, s->len), s->data, s->len);
	memcpy(copy->cb, s->cb, sizeof(s->cb));
	return copy;
}
static int ieee80211_hdrlen(u16 fc) { return 24 + (((fc & 0x8c) == 0x88) ? 2 : 0); }
static bool ieee80211_is_mgmt(u16 fc) { return (fc & 12) == 0; }
static bool ieee80211_is_data(u16 fc) { return (fc & 12) == 8; }
static bool ieee80211_is_auth(u16 fc) { return (fc & 0xfc) == 0xb0; }
static bool ieee80211_is_assoc_resp(u16 fc) { return (fc & 0xfc) == 0x10; }
static struct ieee80211_channel test_channel;
static struct ieee80211_channel *connection_find_channel(struct wiphy *w, u32 freq) { (void)w; (void)freq; return &test_channel; }
static int connection_prepare_station(struct ieee80211_local *local, struct ieee80211_channel *channel) { (void)local; (void)channel; return 0; }
static int connection_mark_authenticated(struct ieee80211_local *local) { (void)local; return 0; }
static int connection_notify_assoc(struct ieee80211_local *local, u16 aid);
static int connection_authorize(struct ieee80211_local *local);
static int connection_install_keys(struct ieee80211_local *local);
static inline int connection_rx_process(struct ieee80211_local *local, struct sk_buff *skb);
static inline int connection_send_eapol(struct ieee80211_local *local, const u8 *eapol, int len);

#include "wifi_connection_under_test.inc"

static struct ieee80211_vif test_vif = { .addr = {2, 0, 0, 0, 0, 1} };
static struct ieee80211_local test_local = { .ieee80211_vif = &test_vif };
static _Atomic unsigned int management_count, eapol_count, authorized, key_installs;
static bool immediate_responses;
static int connection_notify_assoc(struct ieee80211_local *local, u16 aid) {
	(void)local;
	assert(aid == 7);
	assert(!connection_get()->deadline_ms && !connection_get()->waiting_tx_status);
	/* The driver is allowed to block here; the RX producer must remain free. */
	usleep(10000);
	return 0;
}
static int connection_authorize(struct ieee80211_local *local) {
	(void)local;
	atomic_fetch_add(&authorized, 1);
	connection_set_state(connection_get(), CONNECTION_CONNECTED);
	return 0;
}
static int connection_install_keys(struct ieee80211_local *local) {
	(void)local;
	atomic_fetch_add(&key_installs, 1);
	assert(connection_get()->gtk_len == 16);
	return 0;
}
static inline int connection_send_eapol(struct ieee80211_local *local, const u8 *eapol, int len) {
	(void)local;
	assert(len >= 99 && eapol[1] == 3);
	atomic_fetch_add(&eapol_count, 1);
	return 0;
}
static struct sk_buff *response(u16 subtype) {
	struct connection_fsm *fsm = connection_get();
	struct sk_buff *s = dev_alloc_skb(30);
	struct ieee80211_mgmt *m = skb_put_zero(s, 30);
	m->frame_control = subtype;
	memcpy(m->da, test_vif.addr, 6);
	memcpy(m->sa, fsm->bssid, 6);
	memcpy(m->bssid, fsm->bssid, 6);
	if (subtype == IEEE80211_STYPE_AUTH)
		m->u.auth.auth_transaction = 2;
	else
		m->u.assoc_resp.aid = 0xc007;
	return s;
}
static void mock_tx(struct ieee80211_hw *hw, struct ieee80211_tx_control *ctl, struct sk_buff *s) {
	(void)hw; (void)ctl;
	u16 subtype = wifi_wire_le16(s->data) & 0xf0;
	struct connection_fsm *fsm = connection_get();
	assert(fsm->state == (subtype == IEEE80211_STYPE_AUTH ? CONNECTION_AUTH_SENT : CONNECTION_ASSOC_SENT));
	atomic_fetch_add(&management_count, 1);
	if (immediate_responses){
		/* Deliver RX before tx() returns and before its TX acknowledgement. */
		struct sk_buff *rx = response(subtype == IEEE80211_STYPE_AUTH ? IEEE80211_STYPE_AUTH : IEEE80211_STYPE_ASSOC_RESP);
		assert(connection_rx_observe(&test_local, rx));
		kfree_skb(rx);
		IEEE80211_SKB_CB(s)->flags |= IEEE80211_TX_STAT_ACK;
		connection_tx_status_observe(s, false);
	}
	kfree_skb(s);
}
static struct ieee80211_ops test_ops = { .tx = mock_tx };
static int mock_disable_key(struct ieee80211_hw *hw, enum set_key_cmd cmd,
		struct ieee80211_vif *vif, struct ieee80211_sta *sta, struct ieee80211_key_conf *key) {
	(void)hw; (void)vif; (void)sta; (void)key;
	assert(cmd == DISABLE_KEY);
	removed_keys++;
	return 0;
}
static int mock_sta_state(struct ieee80211_hw *hw, struct ieee80211_vif *vif,
		struct ieee80211_sta *sta, enum ieee80211_sta_state old, enum ieee80211_sta_state new_state) {
	(void)hw; (void)vif; (void)sta;
	assert(new_state == old - 1);
	state_transitions++;
	return 0;
}
static struct connection_fsm *new_connection(bool secure) {
	struct connection_fsm *fsm = calloc(1, sizeof(*fsm));
	assert(fsm);
	atomic_init(&fsm->state, CONNECTION_IDLE);
	fsm->protected_network = secure;
	memcpy(fsm->bssid, "\x02\x00\x00\x00\x00\x02", 6);
	memcpy(fsm->ssid, "IEEE", 4); fsm->ssid_len = 4;
	memcpy(fsm->passphrase, "password", 8); fsm->passphrase_len = 8;
	memcpy(fsm->rates, "\x82\x84\x8b\x96", 4); fsm->rates_len = 4;
	director.ieee80211_ops = &test_ops;
	director.connection_fsm = fsm;
	management_count = eapol_count = authorized = key_installs = 0;
	return fsm;
}
static void wait_state(struct connection_fsm *fsm, enum connection_state state) {
	u64 until = connection_now_ms() + 2000;
	while (fsm->state != state && connection_now_ms() < until)
		usleep(1000);
	assert(fsm->state == state);
}
static void dispose(struct connection_fsm *fsm) {
	pthread_mutex_lock(&director.connection_lock);
	fsm->stopping = true;
	pthread_cond_signal(&director.connection_wake);
	pthread_mutex_unlock(&director.connection_lock);
	if (fsm->retry_worker_started)
		pthread_join(fsm->worker, NULL);
	director.connection_fsm = NULL;
	while (fsm->events_head){
		struct connection_event *e = fsm->events_head;
		fsm->events_head = e->next;
		if (e->skb) kfree_skb(e->skb);
		free(e);
	}
	free(fsm);
}
static void deliver_eapol(const u8 *payload, unsigned int len, bool ethernet) {
	struct sk_buff *s = dev_alloc_skb(len + 32);
	if (ethernet){
		u8 *p = skb_put_zero(s, 14);
		memcpy(p, test_vif.addr, 6); memcpy(p + 6, connection_get()->bssid, 6);
		memcpy(p + 12, "\x88\x8e", 2);
		IEEE80211_SKB_RXCB(s)->flag = RX_FLAG_8023;
	}else{
		struct ieee80211_hdr *h = skb_put_zero(s, 24);
		h->frame_control = IEEE80211_FTYPE_DATA | IEEE80211_FCTL_FROMDS;
		memcpy(h->addr1, test_vif.addr, 6); memcpy(h->addr2, connection_get()->bssid, 6);
		memcpy(skb_put(s, 8), "\xaa\xaa\x03\x00\x00\x00\x88\x8e", 8);
	}
	memcpy(skb_put(s, len), payload, len);
	assert(connection_rx_observe(&test_local, s));
	kfree_skb(s);
}

int main(void) {
	connection_runtime_init();
	immediate_responses = true;
	struct connection_fsm *fsm = new_connection(false);
	assert(!wifi_connection_send_auth(&test_local, &test_channel));
	assert(!connection_start_retry_worker(&test_local, fsm));
	wait_state(fsm, CONNECTION_CONNECTED);
	assert(management_count == 2 && authorized == 1);
	dispose(fsm);

	/* A queued association response wins over an already-expired timer. */
	immediate_responses = false;
	fsm = new_connection(false);
	fsm->state = CONNECTION_ASSOC_SENT;
	fsm->assoc_attempts = 4;
	fsm->deadline_ms = connection_now_ms() - 1;
	struct sk_buff *rx = response(IEEE80211_STYPE_ASSOC_RESP);
	assert(connection_rx_observe(&test_local, rx)); kfree_skb(rx);
	assert(!connection_start_retry_worker(&test_local, fsm));
	wait_state(fsm, CONNECTION_CONNECTED);
	assert(management_count == 0);
	dispose(fsm);

	/* TX status belongs to one request only, and arms a full response window. */
	fsm = new_connection(false);
	assert(!wifi_connection_send_auth(&test_local, &test_channel));
	u64 original_deadline = fsm->deadline_ms;
	assert(original_deadline >= connection_now_ms() + 2900);
	struct connection_event tx_event = { .cookie = fsm->pending_cookie + 1, .acked = true };
	connection_tx_status_process(fsm, &tx_event);
	assert(fsm->deadline_ms == original_deadline && fsm->waiting_tx_status);
	tx_event.cookie = fsm->pending_cookie;
	connection_tx_status_process(fsm, &tx_event);
	assert(!fsm->waiting_tx_status && fsm->deadline_ms >= connection_now_ms() + 900);
	fsm->state = CONNECTION_WAIT_EAPOL_M1;
	fsm->deadline_ms = 12345;
	connection_tx_status_process(fsm, &tx_event);
	assert(fsm->deadline_ms == 12345);
	dispose(fsm);

	/* Bounded failure when neither TX status nor an AP response arrives. */
	fsm = new_connection(false);
	fsm->state = CONNECTION_ASSOC_SENT;
	fsm->assoc_attempts = 4;
	fsm->waiting_tx_status = true;
	fsm->deadline_ms = connection_now_ms() - 1;
	assert(!connection_start_retry_worker(&test_local, fsm));
	wait_state(fsm, CONNECTION_FAILED);
	dispose(fsm);

	/* Full WPA2 M1/M3 processing, mixed Ethernet/raw delivery, EAPOL v1. */
	immediate_responses = true;
	fsm = new_connection(true);
	assert(!wifi_connection_send_auth(&test_local, &test_channel));
	assert(!connection_start_retry_worker(&test_local, fsm));
	wait_state(fsm, CONNECTION_WAIT_EAPOL_M1);
	u8 m1[99] = {1, 3, 0, 95, 2};
	connection_put_be16(m1 + 5, 0x008a);
	connection_put_be16(m1 + 7, 16);
	connection_put_be64(m1 + 9, 1);
	memset(m1 + 17, 0x55, 32);
	deliver_eapol(m1, sizeof(m1), true);
	wait_state(fsm, CONNECTION_WAIT_EAPOL_M3);
	/* Wait for M2 publication; its counter is the fixture's release point. */
	while (eapol_count < 1) usleep(1000);
	static const u8 expected_pmk[32] = {
		0xf4,0x2c,0x6f,0xc5,0x2d,0xf0,0xeb,0xef,0x9e,0xbb,0x4b,0x90,0xb3,0x8a,0x5f,0x90,
		0x2e,0x83,0xfe,0x1b,0x13,0x5a,0x70,0xe2,0x3a,0xed,0x76,0x2e,0x97,0x10,0xa1,0x2e
	};
	assert(!memcmp(fsm->pmk, expected_pmk, 32));
	u8 kde[24] = {0xdd,22,0,15,172,1,1,0};
	memset(kde + 8, 0x33, 16);
	u8 wrapped[32], m3[160]; int wrapped_len, final_len, m3_len;
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new(); assert(ctx);
	EVP_CIPHER_CTX_set_flags(ctx, EVP_CIPHER_CTX_FLAG_WRAP_ALLOW);
	assert(EVP_EncryptInit_ex(ctx, EVP_aes_128_wrap(), NULL, fsm->ptk + 16, NULL) == 1);
	assert(EVP_EncryptUpdate(ctx, wrapped, &wrapped_len, kde, sizeof(kde)) == 1);
	assert(EVP_EncryptFinal_ex(ctx, wrapped + wrapped_len, &final_len) == 1);
	EVP_CIPHER_CTX_free(ctx);
	assert(wrapped_len == 32 && final_len == 0);
	assert(!connection_build_key_eapol(m3, sizeof(m3), 0x13ca, 2, fsm->anonce,
		wrapped, wrapped_len, fsm->ptk, &m3_len));
	deliver_eapol(m3, m3_len, false);
	wait_state(fsm, CONNECTION_CONNECTED);
	assert(key_installs == 1 && authorized == 1);
	unsigned int before = eapol_count;
	deliver_eapol(m3, m3_len, true);
	u64 until = connection_now_ms() + 2000;
	while (eapol_count == before && connection_now_ms() < until) usleep(1000);
	assert(eapol_count == before + 1 && key_installs == 1);
	dispose(fsm);

	/* Corrupted AES-wrap data is rejected, not reported as successful unwrap. */
	u8 key[16] = {0}, invalid[24] = {0}, plain[32]; int plain_len = -1;
	assert(connection_aes_unwrap(key, invalid, sizeof(invalid), plain, &plain_len) < 0);
	assert(plain_len == 0);

	/* Missing EAPOL M1 has its own deadline; it cannot hang indefinitely. */
	fsm = new_connection(true);
	fsm->state = CONNECTION_WAIT_EAPOL_M1;
	fsm->handshake_deadline_ms = fsm->deadline_ms = connection_now_ms() - 1;
	assert(!connection_start_retry_worker(&test_local, fsm));
	wait_state(fsm, CONNECTION_FAILED);
	dispose(fsm);

	/* Disconnect joins the worker, unwinds all station states and unpublishes it. */
	fsm = new_connection(true);
	fsm->state = CONNECTION_WAIT_EAPOL_M1;
	fsm->handshake_deadline_ms = fsm->deadline_ms = connection_now_ms() + 15000;
	fsm->sta = calloc(1, sizeof(*fsm->sta));
	fsm->sta_info = calloc(1, sizeof(*fsm->sta_info));
	fsm->link_sta_info = calloc(1, sizeof(*fsm->link_sta_info));
	fsm->chanctx = calloc(1, sizeof(*fsm->chanctx));
	fsm->ptk_key = calloc(1, sizeof(*fsm->ptk_key));
	fsm->gtk_key = calloc(1, sizeof(*fsm->gtk_key));
	fsm->sta_info->sta_state = IEEE80211_STA_AUTHORIZED;
	fsm->sta_hashed = fsm->link_hashed = fsm->sta_listed = true;
	test_local.num_sta = 1;
	test_vif.cfg.assoc = true;
	test_vif.bss_conf.bssid = fsm->bssid;
	test_vif.bss_conf.chanctx_conf = fsm->chanctx;
	test_ops.set_key = mock_disable_key;
	test_ops.sta_state = mock_sta_state;
	assert(!connection_start_retry_worker(&test_local, fsm));
	assert(!wifi_connection_stop(&test_local));
	assert(director.connection_fsm == NULL && test_local.num_sta == 0);
	assert(!test_vif.cfg.assoc && test_vif.bss_conf.chanctx_conf == NULL);
	assert(removed_keys == 2 && state_transitions == 4 && removed_hashes == 2 && removed_lists == 1);
	assert(director.connection_retired == fsm);
	assert(!wifi_connection_stop(&test_local)); /* idempotent */
	connection_free_retired();
	assert(director.connection_retired == NULL);
	/* A new attempt is no longer blocked by the old station. */
	fsm = new_connection(false);
	assert(!wifi_connection_send_auth(&test_local, &test_channel));
	assert(!connection_start_retry_worker(&test_local, fsm));
	wait_state(fsm, CONNECTION_CONNECTED);
	dispose(fsm);
	puts("wifi connection tests passed");
	return 0;
}
