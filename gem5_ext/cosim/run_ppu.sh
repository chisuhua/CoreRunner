
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

#OPTION="--debug-flags=ProtocolTrace,RubyTest"

# used by gpgpu_sim/cuda-sim/ptx_parser.cc
export PTX_SIM_KERNELFILE=1
# used by gpgpu_sim/cuda-sim/ptx_parser.cc
export PTX_SIM_DEBUG=30
#export PTX_SIM_DEBUG=1

OPTION="--debug-flags="
#OPTION="--debug-flags=DebugPrintf"
OPTION="--debug-flags=Printf"
OPTION="${OPTION},MMU"
OPTION="${OPTION},VtoPhys"
OPTION="${OPTION},Interrupt"
OPTION="${OPTION},DMA"
OPTION="${OPTION},DMACopyEngine"
OPTION="${OPTION},GPUCopyEngine"
OPTION="${OPTION},ShaderMMU"
OPTION="${OPTION},ShaderTLB"
OPTION="${OPTION},CudaCore"
OPTION="${OPTION},CudaCoreAccess"
OPTION="${OPTION},CudaCoreFetch"
OPTION="${OPTION},CudaGPU"
OPTION="${OPTION},CudaGPUAccess"
OPTION="${OPTION},CudaGPUPageTable"
OPTION="${OPTION},CudaGPUTick"
#OPTION="${OPTION}RubyQueue"
OPTION="${OPTION},SimpleCPU"
#OPTION="${OPTION}Faults,"
#OPTION="${OPTION}X86,"
#OPTION="${OPTION}Decoder"
#OPTION="${OPTION},Stats"
#OPTION="${OPTION},StatEvents"
#OPTION="${OPTION},RubyCache"
#OPTION="${OPTION},RubyCacheTrace"
#OPTION="${OPTION},RubyDma"
#OPTION="${OPTION},RubyGenerated"
#OPTION="${OPTION},RubyPort"
#OPTION="${OPTION},RubyPrefetcher"
#OPTION="${OPTION},RubyQueue"
OPTION="${OPTION},RubySequencer"
OPTION="${OPTION},RubySystem"
#OPTION="${OPTION},RubyResourceStalls"
OPTION="${OPTION},AddrRanges"
#OPTION="${OPTION},BaseXBar"
#OPTION="${OPTION},CommMonitor"
#OPTION="${OPTION},MemoryAccess"
#OPTION="${OPTION},RubyStats"
#OPTION="${OPTION},RubyNetwork"
OPTION="${OPTION} $1"
OPTION="${OPTION} $2"


#OPTION="${OPTION} --list-sim-objects"


OPTION="${OPTION} --debug-file=run_debug.log"

OPTION=" --debug-file=run_debug.log"

#OPTION="--debug-flags=SyscallVerbose,GPUSyscalls,CudaGPU,CudaGPUPageTable"
#OPTION="--debug-flags=CudaCore,CudaCoreAccess,CudaCoreFetch,CudaGPU,CudaGPUAccess,CudaGPUPageTable,CudaGPUTick,GPUCopyEngine,ShaderTLB,ShaderLSQ,ShaderMMU"
OPTION="${OPTION} --debug-flags=Printf"
#OPTION="${OPTION},RubySlicc"
#OPTION="${OPTION},ProtocolTrace"
#OPTION="${OPTION},RubyTest"
OPTION="${OPTION},AddrRanges"

#OPTION="${OPTION} --debug-file=run_debug.log"
#OPTION="${OPTION},BaseXBar"
#OPTION="${OPTION},Decode"
#OPTION="${OPTION},MemoryAccess"
#OPTION="${OPTION},Decode"
#OPTION="${OPTION},Interrupt"
#OPTION="${OPTION},RiscvMisc"
#OPTION="${OPTION},Stack"
#OPTION="${OPTION},RiscvTLB"
#OPTION="${OPTION},Fetch"

OPTION="${OPTION},Uart"
OPTION="${OPTION},Timer"
#OPTION="${OPTION},GDBRecv"
#OPTION="${OPTION},GDBMisc"
#OPTION="${OPTION},GDBAcc"
#OPTION="${OPTION},GDBSend"
#OPTION="${OPTION},GDBWrite"

#OPTION="${OPTION},MinorMem"



CMD="tests/test-progs/hello/bin/x86/linux/hello"
CMD="tests/test-progs/threads/bin/X86/linux/threads"

CONF="configs/example/ppu/fs_simple_ppu.py"
#CMD="-b tests/test-progs/bare_metal/bin/riscv/bare_metal"
CMD="-b /mnt/d/source/github/sim/zephyrproject/zephyr/build_ppu32/zephyr/zephyr.elf"

#CMD="${CMD} -w"

if [ ! -e "m5out/config.ini" ]; then
GEM5=$DIR/../../build/X86_VI_hammer/gem5.debug
echo "Running $GEM5 $OPTION $CONF $CMD to generate m5out/config.ini"
$GEM5 $OPTION $CONF $CMD
fi

CO_OPTION=""
CO_OPTION="$CO_OPTION -v"
CO_OPTION="$CO_OPTION -d Uart"
CO_OPTION="$CO_OPTION -d Timer"
CO_OPTION="$CO_OPTION -d AddrRanges"
CO_OPTION="$CO_OPTION -d Bridge"
CO_OPTION="$CO_OPTION -d MemoryAccess"
CO_OPTION="$CO_OPTION -e 1000000000"

#CO_RUN="./build/output/example_master/simple_ppu/gem5.sc "
#CO_RUN="./build/output/example_master/axi/gem5.sc "
CO_RUN="./build/output/$CO_DESIGN/gem5.sc "
CO_RUN="$CO_RUN m5out/config.ini $CO_OPTION"


echo "Running cosim"
echo $CO_RUN
$CO_RUN

#$RUN $OPTION design/gpgpu/configs/my_fusion.py -c /mnt/d/source/github/sim/gem5-gpu/benchmarks/rodinia/vectoradd/gem5_fusion_vectorAdd
#$RUN $OPTION design/gpgpu/configs/ruby_random_test_fusion.py

#build/X86_VI_hammer_GPU/gem5.opt ../gem5-gpu/configs/se_fusion.py -c /path/to/your/benchmarks/rodinia/backprop/gem5_fusion_backprop -o 16
#build/X86_VI_hammer_GPU/gem5.opt ../gem5-gpu/configs/se_fusion.py -c /mnt/d/source/github/sim/gem5-gpu/benchmarks/rodinia/backprop/gem5_fusion_backprop -o 16
#build/X86_VI_hammer_GPU/gem5.debug $OPTION ../gem5-gpu/configs/se_fusion.py -c /mnt/d/source/github/sim/gem5-gpu/benchmarks/rodinia/backprop/gem5_fusion_backprop -o 16

#build/X86_VI_hammer_GPU/gem5.opt ../gem5-gpu/configs/se_fusion.py -c /mnt/d/source/github/sim/gem5-gpu/benchmarks/rodinia/vectoradd/simple

#build/X86_VI_hammer_GPU/gem5.opt ../gem5-gpu/configs/se_fusion.py --helon

