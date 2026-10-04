#ifndef EXSUL_WIFI_PROFILE_H
#define EXSUL_WIFI_PROFILE_H

/* Wire-format parsing shared with the hardware-independent connection tests. */
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

struct wifi_ap_profile {
	uint8_t rates[32];
	unsigned int rates_len;
	bool wmm;
	bool rsn;
	bool ccmp_group;
	bool ccmp_pairwise;
	bool psk;
	bool pmf_required;
	bool legacy_forbidden;
};

static inline unsigned int wifi_wire_le16(const uint8_t *p)
{
	return p[0] | ((unsigned int)p[1] << 8);
}

static inline bool wifi_rsn_suite(const uint8_t *p, uint8_t type)
{
	return p[0] == 0 && p[1] == 0x0f && p[2] == 0xac && p[3] == type;
}

static inline int wifi_parse_rsn(const uint8_t *p, size_t len,
				struct wifi_ap_profile *out)
{
	unsigned int count;
	if (len < 8 || wifi_wire_le16(p) != 1)
		return -EINVAL;
	out->rsn = true;
	out->ccmp_group = wifi_rsn_suite(p + 2, 4);
	p += 6;
	len -= 6;
	count = wifi_wire_le16(p);
	p += 2;
	len -= 2;
	if (!count || count > len / 4)
		return -EINVAL;
	while (count--){
		out->ccmp_pairwise |= wifi_rsn_suite(p, 4);
		p += 4;
		len -= 4;
	}
	if (len < 2)
		return -EINVAL;
	count = wifi_wire_le16(p);
	p += 2;
	len -= 2;
	if (!count || count > len / 4)
		return -EINVAL;
	while (count--){
		out->psk |= wifi_rsn_suite(p, 2);
		p += 4;
		len -= 4;
	}
	if (!len)
		return 0; /* Optional RSN capabilities default to zero. */
	if (len < 2)
		return -EINVAL;
	out->pmf_required = !!(wifi_wire_le16(p) & (1U << 6));
	p += 2;
	len -= 2;
	if (!len)
		return 0;
	if (len < 2)
		return -EINVAL;
	count = wifi_wire_le16(p);
	p += 2;
	len -= 2;
	if (count > len / 16)
		return -EINVAL;
	len -= count * 16;
	return len == 0 || len == 4 ? 0 : -EINVAL;
}

static inline int wifi_parse_ap_ies(const uint8_t *ies, size_t len,
				   struct wifi_ap_profile *out)
{
	memset(out, 0, sizeof(*out));
	while (len){
		unsigned int id, n;
		if (len < 2 || ies[1] > len - 2)
			return -EINVAL;
		id = ies[0];
		n = ies[1];
		ies += 2;
		len -= n + 2;
		if (id == 1 || id == 50){
			if (!n || (id == 1 && n > 8) || n > sizeof(out->rates) - out->rates_len)
				return -EINVAL;
			for (unsigned int i = 0; i < n; i++){
				unsigned int rate = ies[i] & 0x7f;
				/* Basic membership selectors require a non-legacy PHY. */
				if ((ies[i] & 0x80) && rate >= 121)
					out->legacy_forbidden = true;
				out->rates[out->rates_len++] = ies[i];
			}
		}else if (id == 48){
			if (out->rsn || wifi_parse_rsn(ies, n, out))
				return -EINVAL;
		}else if (id == 221 && n >= 7 &&
			  !memcmp(ies, "\x00\x50\xf2\x02", 4) && ies[5] == 1 &&
			  (ies[4] == 0 || ies[4] == 1)){
			out->wmm = true;
		}
		ies += n;
	}
	return out->rates_len ? 0 : -EINVAL;
}

static inline int wifi_profile_supported(const struct wifi_ap_profile *ap, bool privacy)
{
	if (ap->legacy_forbidden)
		return -EOPNOTSUPP;
	if (!privacy)
		return ap->rsn ? -EOPNOTSUPP : 0;
	return ap->rsn && ap->ccmp_group && ap->ccmp_pairwise && ap->psk &&
	       !ap->pmf_required ? 0 : -EOPNOTSUPP;
}

/* Rates use the 802.11 wire unit of 500 kbit/s. Preserve AP basic-rate bits. */
static inline int wifi_select_rates(const struct wifi_ap_profile *ap,
		const unsigned int *local_rates, unsigned int local_count,
		uint8_t *rates, uint8_t *rates_len, uint32_t *supported, uint32_t *basic)
{
	*rates_len = 0;
	*supported = *basic = 0;
	if (local_count > 32)
		return -EINVAL;
	for (unsigned int i = 0; i < ap->rates_len; i++){
		unsigned int j;
		for (j = 0; j < local_count; j++)
			if ((ap->rates[i] & 0x7f) == local_rates[j])
				break;
		if (j == local_count){
			if (ap->rates[i] & 0x80)
				return -EOPNOTSUPP;
			continue;
		}
		if (ap->rates[i] & 0x80)
			*basic |= UINT32_C(1) << j;
		if (*supported & (UINT32_C(1) << j)){
			for (unsigned int k = 0; k < *rates_len; k++)
				if ((rates[k] & 0x7f) == local_rates[j])
					rates[k] |= ap->rates[i] & 0x80;
			continue;
		}
		*supported |= UINT32_C(1) << j;
		rates[(*rates_len)++] = ap->rates[i];
	}
	return *rates_len && *basic ? 0 : -EOPNOTSUPP;
}

#endif
