#!/bin/sh
set -eu
cd "$(dirname "$0")/../.."
wifi_test_dir=$(mktemp -d /tmp/exsul-wifi-tests.XXXXXX)
awk -f tools/tests/extract_wifi_connection.awk inc/ieee80211.h > "$wifi_test_dir/wifi_connection_under_test.inc"
cc=${CC:-gcc}
"$cc" -std=gnu11 -Wall -Wextra -Werror -g -fsanitize=address,undefined \
    -Iinc tools/tests/wifi_profile_test.c -o "$wifi_test_dir/wifi_profile_test"
"$cc" -std=gnu11 -Wall -Wextra -Werror -Wno-unused-function -Wno-sign-compare -g \
    -fsanitize=address,undefined -pthread -Iinc -I"$wifi_test_dir" \
    tools/tests/wifi_connection_test.c -lcrypto -o "$wifi_test_dir/wifi_connection_test"
"$wifi_test_dir/wifi_profile_test"
"$wifi_test_dir/wifi_connection_test"
printf 'Test artifacts: %s\n' "$wifi_test_dir"
