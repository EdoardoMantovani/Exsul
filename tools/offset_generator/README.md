Made by Edoardo Mantovani, 2026
-------------------------------

This tool generates a C archive of struct field offsets for Linux kernels.

Workload:

* the struct parser in Build.pl detects the fields of the structs which have changed a lot during the kernel development (module, iee80211_hw ecc..)
* `main_launcher.pl` takes `required_offsets.json` as input by default
* it discovers every Linux tag since `v6.0` from the Linux Git repository
* for every selected tag, it downloads the GitHub archive, appends the hardcoded debug-info config lines to `arch/x86/configs/x86_64_defconfig`, runs `make ARCH=x86 x86_64_defconfig`, and builds `vmlinux`
* it runs `pahole -C <struct> vmlinux`, extracts the requested field offsets, and stores them in `offset_backup_solution.h`
* completed kernels are recorded in `.offset_generator_state.json`, so the tool can be resumed after interruption

TODO:
- implement also the support for different distro, like Ubuntu or Gentoo, take only the defconfig and apply them to the specific version of the kernel used.

Usage:

```
perl main_launcher.pl
perl main_launcher.pl --from-tag v6.0 --to-tag v6.8 --jobs 16
perl main_launcher.pl --tags v6.0,v6.1 --stop-on-error
```

Useful options:

* `--output FILE`: generated C header path
* `--workdir DIR`: downloads, extracted sources, and logs
* `--force`: rebuild selected tags already present in the state file
* `--keep-builds`: keep extracted Linux trees after processing
* `--include-rc`: include release-candidate tags
