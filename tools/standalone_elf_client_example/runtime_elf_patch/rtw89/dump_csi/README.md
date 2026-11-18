# rtw89 CSI dump

Runtime patch for enabling RTL8852CU channel-information reports in the
standalone rtw89 ELF. It prints report metadata and raw CSI bytes directly;
it does not use netlink.

Build Exsul normally, start the standalone ELF, then run:

```sh
../../script_launcher rtw89_script.c
```

The patch leaves normal receive traffic active and captures for 300 ms by
default. Set the variables on the standalone ELF process before launching the
runtime patch. Available controls are:

```sh
RTW89_CSI_DURATION_MS=60000 RTW89_CSI_MACID=0 \
  ./rtw89_8852cu_git_7.0.0.elf

../../script_launcher rtw89_script.c
```

- `RTW89_CSI_DURATION_MS`: capture duration; `0` runs without a time limit.
- `RTW89_CSI_MAX_REPORTS`: stop after this many complete reports; `0` disables
  the report limit.
- `RTW89_CSI_MACID`: hardware MACID selected by the rich-mode receive filter.
- `RTW89_CSI_LEGACY`: use a legacy 1x1 profile and include management/control
  frames instead of the default 2x2 MIMO data-frame profile.

The USB observer decodes RTL8852C AX RX descriptors (16/32-byte descriptors,
8-byte USB aggregation alignment). RX type 2 is decoded as the segmented
Realtek channel-info format. The script validates sequence numbers and lengths,
reassembles the matrix, and prints it only when the final segment is complete.
RX type 11 is not used by the RTL8852CU vendor channel-info path and is ignored;
ordinary Wi-Fi frames are never printed as CSI.

For useful samples, keep scan or other receive traffic active. After creating
an associated station, set `RTW89_CSI_MACID` to that station's hardware MACID.
If no type-2 report arrives, the final diagnostics print the observed Wi-Fi RX
MACIDs, descriptor-type counters, register readback, and BB-report error flags.
Restart the standalone ELF with `RTW89_CSI_MACID` set to the MACID receiving
ordinary Wi-Fi frames when it differs from the default value `0`.

If the default MIMO window contains no data frames, the script automatically
retries once with the legacy 1x1 profile. If all received management frames
lack a valid address-CAM match, the retry uses MACID `255`, matching the value
used by the RTL8852CU vendor RX parser for such frames.
