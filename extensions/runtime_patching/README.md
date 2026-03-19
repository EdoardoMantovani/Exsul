Mixing libtcc, the ghidra c++ decompiler library and the gcc flag -fpatchable-function-entry=N[,M]
could lead to a runtime jit patching framework which would permit to extend/edit the loaded lkm in memory by
using the high level C code instead of interacting with the raw assembly

The idea lies on the fact that if we add some NOPs at the start of every function (or even in different parts, it may
still be possible to exploit the 4 byte alignment for getting some extra space) we could use the internal sym_table
for getting the function address, overwriting the NOPs with a jump on a function written in C at runtime, this could be
extended also by using the ghidra decompilation framework library, even if it would require some additional work,
it should be possible to have a patching + decompilation of a usb kernel driver which is running in userspace in real time.


Useful links:

[1] gcc -finstrument: https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html
[2] ghidra decompiler library: https://fp32.org/ghidra_decompiler_cli_guide.html
[3] libtcc main page: https://bellard.org/tcc/tcc-doc.html
