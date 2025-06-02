rm build/variables.global

cd $GEM5_ROOT
# cd gem5
#scons build/X86_VI_hammer_GPU/gem5.opt --default=../../../gem5-gpu/build_opts/X86_VI_hammer_GPU EXTRAS=../gem5-gpu/src:../gpgpu-sim/

#scons build/X86_VI_hammer_GPU/gem5.opt --verbose


#scons build/PPU/gem5.debug --verbose -j6

#scons build/RISCV/gem5.debug --verbose -j6
#scons build/RISCV/gem5.debug

#scons build/X86/gem5.opt --verbose BUILD_GPU=yes  PROTOCOL=GPU_VIPER_Baseline -j6
#scons build/X86/gem5.debug --verbose -j6
#scons build/X86_GPU/gem5.debug --verbose BUILD_GPU=yes PROTOCOL=GPU_VIPER_Baseline -j6

#scons build/HSAIL_X86/gem5.opt --verbose BUILD_GPU=yes  PROTOCOL=GPU_VIPER_Baseline -j6
#scons build/HSAIL_X86/gem5.opt --verbose -j6

#scons build/NULL/gem5.debug --verbose -j6 PROTOCOL=Garnet_standalone
#scons build/NULL_MOESI_hammer/gem5.debug --verbose -j8

#scons build/MSI/gem5.debug --verbose -j6 PROTOCOL=MSI

#scons build/X86_MSI/gem5.debug -j6 --verbose PROTOCOL=MSI
#scons -j6 --with-cxx-config --without-python --without-tcmalloc build/X86_MSI/libgem5_debug.so --verbose

#scons build/MSI/gem5.opt --verbose -j6


#scons build/X86_VI_hammer/gem5.debug --verbose -j8 BUILD_PPU=yes --default=../design/gpgpu/build_opts/X86_VI_hammer EXTRAS=design/gpgpu

#scons -j6 --with-cxx-config --without-python --without-tcmalloc build/X86_VI_hammer/libgem5_$GEM5_VARIANT --verbose BUILD_PPU=yes USE_ZEPHYR=yes --default=../design/gpgpu/build_opts/X86_VI_hammer EXTRAS=design/gpgpu
#scons -j6 --with-cxx-config --without-python --without-tcmalloc build/X86_VI_hammer/libgem5_$GEM5_VARIANT.so --verbose --default=../design/gpgpu/build_opts/X86_VI_hammer EXTRAS=design/gpgpu
scons -j$GEM5_CPU_CORES --with-cxx-config --without-tcmalloc build/X86_VI_hammer/libgem5_$GEM5_VARIANT.so --verbose --default=../design/gpgpu/build_opts/X86_VI_hammer EXTRAS=design/gpgpu:design/opu/opusim/src/gem5
