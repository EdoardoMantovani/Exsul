#include <assert.h>
#include <stdio.h>
#include "wifi_profile.h"

static const uint8_t wpa2_ies[] = {
	1, 8, 0x82, 0x84, 0x8b, 0x96, 12, 18, 24, 36,
	50, 4, 48, 72, 96, 108,
	48, 20, 1, 0, 0, 15, 172, 4, 1, 0, 0, 15, 172, 4,
	1, 0, 0, 15, 172, 2, 0, 0,
	221, 7, 0, 80, 242, 2, 0, 1, 0,
};

int main(void)
{
	struct wifi_ap_profile ap;
	uint8_t changed[sizeof(wpa2_ies)], rates[32], count;
	uint32_t supported, basic;
	const unsigned int local[] = {2, 4, 11, 22, 12, 18, 24, 36, 48, 72, 96, 108};
	assert(!wifi_parse_ap_ies(wpa2_ies, sizeof(wpa2_ies), &ap));
	assert(!wifi_profile_supported(&ap, true));
	assert(ap.wmm && ap.rates_len == 12);
	assert(!wifi_select_rates(&ap, local, 12, rates, &count, &supported, &basic));
	assert(count == 12 && supported == 0xfff && basic == 15);
	assert(!memcmp(rates, wpa2_ies + 2, 8));
	assert(!memcmp(rates + 8, wpa2_ies + 12, 4));

	/* Open networks, optional PMF, and required PMF are distinct policies. */
	assert(!wifi_parse_ap_ies(wpa2_ies, 16, &ap));
	assert(!wifi_profile_supported(&ap, false));
	assert(wifi_profile_supported(&ap, true) == -EOPNOTSUPP);
	memcpy(changed, wpa2_ies, sizeof(changed));
	changed[36] = 0x80;
	assert(!wifi_parse_ap_ies(changed, sizeof(changed), &ap));
	assert(!wifi_profile_supported(&ap, true));
	changed[36] = 0xc0;
	assert(!wifi_parse_ap_ies(changed, sizeof(changed), &ap));
	assert(wifi_profile_supported(&ap, true) == -EOPNOTSUPP);
	changed[36] = 0;
	changed[35] = 8; /* SAE only */
	assert(!wifi_parse_ap_ies(changed, sizeof(changed), &ap));
	assert(wifi_profile_supported(&ap, true) == -EOPNOTSUPP);
	changed[35] = 2;
	changed[23] = 2; /* TKIP group cipher */
	assert(!wifi_parse_ap_ies(changed, sizeof(changed), &ap));
	assert(wifi_profile_supported(&ap, true) == -EOPNOTSUPP);

	/* Mandatory PHY selectors and unsupported basic rates cannot be ignored. */
	memcpy(changed, wpa2_ies, sizeof(changed));
	changed[2] = 0xff;
	assert(!wifi_parse_ap_ies(changed, sizeof(changed), &ap));
	assert(wifi_profile_supported(&ap, true) == -EOPNOTSUPP);
	changed[2] = 0x83;
	assert(!wifi_parse_ap_ies(changed, sizeof(changed), &ap));
	assert(wifi_select_rates(&ap, local, 12, rates, &count, &supported, &basic) == -EOPNOTSUPP);
	changed[2] = 3; /* An unsupported optional rate may be omitted. */
	assert(!wifi_parse_ap_ies(changed, sizeof(changed), &ap));
	assert(!wifi_select_rates(&ap, local, 12, rates, &count, &supported, &basic));
	assert(count == 11 && basic == 14);

	/* Malformed advertised lengths/counts fail before dereferencing any suite. */
	for (size_t n = 17; n < 38; n++)
		assert(wifi_parse_ap_ies(wpa2_ies, n, &ap) == -EINVAL);
	memcpy(changed, wpa2_ies, sizeof(changed));
	changed[24] = 255;
	changed[25] = 255;
	assert(wifi_parse_ap_ies(changed, sizeof(changed), &ap) == -EINVAL);
	assert(wifi_parse_ap_ies(NULL, 0, &ap) == -EINVAL);
	puts("wifi profile tests passed");
	return 0;
}
