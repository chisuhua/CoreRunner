set remote Z-packet on
#set riscv use_compressed_breakpoint off
set debug serial 1

#handle SIGPIPE nostop print
#handle SIGIO nostop noprint
#handle SIGTRAP nostop noprint

#file tests/test-progs/bare_metal/bin/riscv/bare_metal
#file /mnt/d/yunpan/source/github/sim/zephyreproject/zephyr/build/zephyr/zephyr.elf
file gpgpu/gpgpu-sim/cuda_samples/0_Simple/vectorAdd/vectorAdd

target remote localhost:7000
#break vectorAdd.cu:30
#break vectorAdd.cu:53
#break cuda_runtime_api.cc:2153
#break cuda_runtime_api.cc:2166
#break cuda_runtime_api.cc:2295
#break extract_code_using_cuobjdump
#break get_app_cuda_version
#break /mnt/d/source/github/sim/mygem5gpu/gem5-ppu/design/gpgpu/gpgpu-sim/libcuda/cuda_runtime_api.cc:2309



