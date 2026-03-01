Made by Edoardo Mantovani, 2026
-------------------------------

This tool permits to generate the offset for a specific kernel version or for a range of kernel versions.

Workload:

* the struct parser in Build.pl detects the fields of the structs which have changed a lot during the kernel development (module, iee80211_hw ecc..)
* the "offset_generator" tool takes as input a list of kernel versions (can be a single one or a range, es: 5.2.X-6.17.Y)
* for every kernel version, it downloads the branch from the Linux Kernel github repository, set the x86_64 defconfig and add 'CONFIG_DEBUG_INFO_BTF=y' to the defconfig
* run make and then through pahole, parse the field offset extracted in the phase 1
* put them in a big header and then delete the current kernel folder, create a new one with the version increased

TODO:
- implement also the support for different distro, like Ubuntu or Gentoo, take only the defconfig and apply them to the specific version of the kernel used.
