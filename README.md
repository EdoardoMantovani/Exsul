# Exsul

Exsul is a complete framework used for running USB Linux kernel drivers *completely* in userspace, this open the door to hundreds of different applications:
from JIT runtime code patching to converting the kernel driver into a standalone ELF/PE file.

The current focus is USB Wi-Fi hardware, especially Realtek `rtw89` Wi-Fi 6/6E/7 drivers and MediaTek `mt76` drivers,
the support for the HCI layer is partially implemented and it is expected to fully support also USB Bluetooth dongles in the next releases.

This tool is at an experimental stage: it is useful for driver research, debugging/function tracing, fuzzing and firmware reverse engineering,
but it is (still) not a drop-in replacement for the Linux kernel, the tun/tap network registration process needs to be examined before practically implementing it.

## Exsul Under-The-Hood

The general idea for running a kernel module in userspace is based upon several assumption:
- you can reproduce/run the (Linux) kernel module linking process
- you have the correct struct-offsets which have been used for compiling the target kernel module
- you have the full Linux kernel stack and the exported functions needed by the target kernel module

Regarding the first point, Linux implements the system call `init_module` for loading and running a given kernel object,
this is essentially what gets called from tools like `insmod` and `modprobe`,
`init_module` itself is not too difficult to implement in the userspace, most of its operations are quite straightforward:
check if the ELF header and section table are corrupt, search and resolve the relocations, allocate the
memory layout for the loaded module and place the sections of the module in its memory, most of the steps can be reproduced easily,
the only *complex* part relies on the relocation resolutions and symbol lookup, the kernel has a symbol table which is used for connecting an exported function
by its name, Exsul implements a similar architecture by mantaining an huge array called *sym_table* (see this later). 
The second point was by far the most challenging of this project, suppose we have the following kernel C code: 
```c
data = skb_push(skb, IEEE80211_HT_CTL_LEN);
memmove(data, data + IEEE80211_HT_CTL_LEN, hdr_len);
```
both `skb_push` and `memmove` are exported functions, this means that their code isn't inside the kernel object (.ko) but instead it is present
in the host kernel and at runtime, the code of `skb_push` is:
```c
void *skb_push(struct sk_buff *skb, unsigned int len)
{
	skb->data -= len;
	skb->len  += len;
	if (unlikely(skb->data < skb->head))
		skb_under_panic(skb, len, __builtin_return_address(0));
	return skb->data;
}
```
the offsets for the `data`, `head` and `len` fields changes across kernel versions, so the `data` field on Linux 7.0.0-28 is at 208, while the same offset under Linux
5.15.167.4 is at 200, you can understand that this creates an huge problem because the kernel module has hardcoded only one specific version of those offsets.

For the last point, - the Linux exported functions - under the folder `/stubs/functions` and `/stub/variables`, you will find a big number of C files, each contains the definition of one
specific function/global variable exported by the kernel (with `readelf -sW <target_module.ko> | grep UND`)
When Perl runs `Build.pl`, every C stubs will be merged into a single big file and it will be compiled inside Exsul, this means that at the end, Exsul reminds the Linux Kernel Library
project, with the difference that most functions have been reimplemented manually specifically for the userspace.

## Build Requirements

- Perl
- GCC or Clang (use the --compiler flag for switching the compiler)
- standard Linux utilities: `readelf`, `sort`, `awk` and `xxd`
- libusb-1.0-dev
- libdwarf (soon will be replaced)
- TinyCC development headers/library
- libunwind-dev

## Build

Build the default Linux executable with the following command:

```sh
perl Build.pl
```
In case you want to switch compiler, pass the `--compiler` flag (tcc isn't supported), for example:

```sh
perl Build.pl --compiler=clang
```

I suggest to read the various flags supported with `--help`, they permits to build Exsul for Windows, strip the final executable automatically,
enabling debugging output and much more..

```sh
perl Build.pl --help
Build script for Exsul
Made by Edoardo Mantovani, 2026
-> options:
--compiler=[gcc|clang]
--enable-fw -> enable static firmware embedding in the main application, this permits to not use the Linux standard path for loading a firmware
--generate_offset -> generate a special json file which can be used for creating the offset backup plan
--windows -> enable windows compilation and require static Windows libraries for a single executable
--verbose -> show compilation command line
--dbg -> enable debugging strings in Exsul
--strip -> strip the final executable
--trace -> trace every exported function, used mainly for debugging!
--clean -> clear every compilation file
--help -> display this help text
```

## Loading and Executing Linux Kernel Modules

Basic form for loading a module under Exsul:

```sh
./bin/exsul <module.ko>
```

nothing will be executed, you will have a kernel module loaded into the Exsul's process memory, if you need to call a specific function you can pass the
function name in the command line or use the command `run [function_name]` in the Exsul shell.

Example with the mainline Realtek `rtw89` drivers:

```sh
./bin/exsul rtw89_core_git.ko rtw89_usb_git.ko rtw89_8852c_git.ko rtw89_8852cu_git.ko
```

This will load multiple kernel modules at the same times, like the kernel does, remember that the module load order matters:
always invoke first the `core` and `usb` modules and then the chipset specific one.

## Interactive Shell

After startup, Exsul starts an interactive command prompt, common commands supported follows:

```text
start				-> initialize the loaded driver by locating and calling its probe function
load <module.ko>		-> load another module in Exsul's memory
unload <module.ko>		-> unload a module
run <function_name>		-> call an exported function from a loaded module (lookup from the Exsul internal function table)
run <module.ko> <function>	-> call a function from a specific module
ops <command>			-> invoke the mac80211/cfg80211 operation registered by the driver
get sym				-> print resolved symbols (for debugging)
get elf/pe			-> use the current process memory for creating a standalone ELF/PE file
get conf			-> write offsets.conf for the current run
exit				-> terminate Exsul, unload every module
```

The `ops` command invokes callbacks exported by the loaded kernel module through mac80211 or cfg80211 layer.

Common mac80211-oriented commands:

```text
ops start
ops stop
ops add_interface sta
ops remove_interface
ops config
ops scan
ops hw_scan
ops sw_scan
ops cancel_scan
ops connect <ssid> [password]
ops rx [on|off]
ops start_ap [ssid]
ops stop_ap
```

Common cfg80211/netdev-oriented commands:

```text
ops open # invokes the 'ndo_open' callback in 'net_device_ops'
ops stop
ops init
ops uninit
ops add_interface sta
ops remove_interface
ops change_interface
ops scan
ops hw_scan
ops start_ap <ssid>
ops stop_ap
ops mac <aa:bb:cc:dd:ee:ff>
ops rx on
ops rx off
```

notice that there are some special commands like `ops rx` and `ops connect`, even if they aren't hardcoded in any mac80211/cfg80211 `_ops` struct, they
are used for enabling/disabling the rx debugging patch and for connecting to a Wi-Fi network.

## Offsets

Exsul needs structure sizes and field offsets for properly running loaded kernel modules.
There are three offset sources which can be used for properly running Exsul:

1. `inc/struct_offset.h`, generated during the build - still experimental, this would work fine on standard distro like Ubuntu, Fedora ecc.. -
2. `offsets.conf`, loaded at runtime if supplied on the command line, can be obtained after the first module loading with the command `get offset`
3. DWARF/pahole based discovery from the loaded module or kernel debug data, this is the default mechanism and is used when no other offset source is given,
essentially, it works like pahole but at runtime, as a result, the initial loading is slow.

The distro offset generator consumes `required_offsets.json` and can produce a header with known offsets for distro kernels:

```sh
cd tools/offset_generator
perl distro_offset_generator.pl required_offsets.json distro_struct_offsets.h
```

The current script is Ubuntu-oriented and expects official debug packages. It uses `pahole` to extract offsets from unpacked debug images.

## Standalone Executable


## Debugging

Valgrind is useful because Exsul runs the driver inside a userspace process:

```sh
valgrind --leak-check=full ./bin/exsul <module.ko> [more modules...] [offset.conf]
```

When Exsul catches a crash, it prints the stacktrace and writes crash report file which can be used for further debugging, the file name usually is:

```text
crashreport-YYYY-MM-DD_HH:MM:SS.bin
sigabort_crash.bin # for sigabort events where memory cannot be allocated
```

## Current Limitations

- Correct structure offsets are critical; wrong offsets usually become memory corruption or invalid callback calls.
- USB, firmware loading, workqueues, timers, cfg80211, mac80211, netdev, and allocation semantics are approximations of kernel behavior.

## FAQ

### Which drivers are tested right now?

The currently tested driver families are `rtw89`, `mt76`, notice that also the development kernel modules written by Realtek like `rtw8852cu`, `rtl8852bu`, and `rtw8851bu` are fully working.

### Can Exsul create a real network interface?

Not as a complete kernel replacement for now, the driver-side mac80211/cfg80211/netdev callbacks can be invoked and tested, but for a full practical network integration it still needs the tun/tap
registration path and related net stack behavior to be completed, in the next releases this aspect will be examined.

### Which architectures are supported ?

For now only x86_64, in the future Exsul will be able to work also under arm32/arm64.
