savedcmd_lkm_hello_world.mod := printf '%s\n'   lkm_hello_world.o | awk '!x[$$0]++ { print("./"$$0) }' > lkm_hello_world.mod
