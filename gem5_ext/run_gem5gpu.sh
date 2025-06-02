
# cd gem5; and run this script

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
RUN=$GEM5_ROOT/build/X86_VI_hammer/gem5.$GEM5_VARIANT



#OPTION="--debug-flags=SyscallVerbose,GPUSyscalls,CudaGPU,CudaGPUPageTable"
#OPTION="--debug-flags=CudaCore,CudaCoreAccess,CudaCoreFetch,CudaGPU,CudaGPUAccess,CudaGPUPageTable,CudaGPUTick,GPUCopyEngine,ShaderTLB,ShaderLSQ,ShaderMMU"
OPTION="--debug-flags=RubySlicc"
OPTION="${OPTION},ProtocolTrace"
OPTION="${OPTION},RubyTest"

OPTION="--debug-flags=SyscallBase"

#OPTION="--debug-flags=SyscallVerbose"

OPTION="--debug-flags=CudaCoreFetch"
OPTION="${OPTION},CudaCoreAccess"
OPTION="${OPTION},CudaGPUPageTable"
OPTION="${OPTION},MemoryAccess"
OPTION="${OPTION},AddrRanges"
#OPTION="${OPTION},ShaderTLB"
#OPTION="${OPTION},ShaderMMU"
#OPTION="${OPTION},ShaderLSQ"
OPTION="${OPTION} --debug-file=run_debug.log"

if [ ! "$1" = "" ]; then
    CMD=$1
    if [ ! "$2" = "" ]; then
        CMD="$CMD $2"
    fi
else
    CMD="$DIR/../tests/test-progs/hello/bin/x86/linux/hello"
    CMD="$DIR/../tests/test-progs/threads/bin/X86/linux/threads"
    CMD="$DIR/../benchmarks/rodinia/vectoradd/gem5_fusion_vectorAdd"
    CMD="$DIR/../design/gpgpu/gpgpu-sim/cuda_samples/0_Simple/vectorAdd/vectorAdd"
    CMD="$GEM5_ROOT/tests/test-progs/hello/src/hello_cpu"
    #CMD="design/gpgpu/gpgpu-sim/cuda_samples/0_Simple/hello/hello"
    #CMD="/mnt/d/source/github/sim/gem5-gpu/benchmarks/rodinia/backprop/gem5_fusion_backprop -o 16"
    #RUN=./build/X86_VI_hammer_GPU/gem5.debug
    #CMD="tests/test-progs/hello/bin/x86/linux/hello"
fi

#GDB="-w"
GDB=""
#ENV="-e $DIR/../design/gpgpu/gpgpu-sim/gem5.env"
#CMD="$CMD $ENV"
#CMD="$CMD --split"
#CMD="$CMD --system-config=cpu_only"
CMD="$CMD --system-config=opu_libgem5"
#CMD="$CMD --ppu"
#CMD="$CMD --cp_firmware=/work_source/github/sim/zephyrproject/zephyr/build_posix_gem5/zephyr/zephyr.exe"
CMD="$CMD --cp_firmware=$GEM5_ROOT/tests/test-progs/hello/src/hello_opu"

#CMD="$CMD --list-mem-type"
CMD="$CMD --mem-type=SimpleMemory"

CONF="$GEM5_ROOT/design/cosim/configs/se_fusion.py"
#CONF="design/gpgpu/configs/se_fusion.py"

echo $RUN $OPTION $CONF $GDB -c $CMD |tee run_gem5gpu.cmd
$RUN $OPTION $CONF $GDB -c $CMD

#$RUN $OPTION design/gpgpu/configs/my_fusion.py -c /mnt/d/source/github/sim/gem5-gpu/benchmarks/rodinia/vectoradd/gem5_fusion_vectorAdd
#$RUN $OPTION design/gpgpu/configs/ruby_random_test_fusion.py

#build/X86_VI_hammer_GPU/gem5.opt ../gem5-gpu/configs/se_fusion.py -c /path/to/your/benchmarks/rodinia/backprop/gem5_fusion_backprop -o 16
#build/X86_VI_hammer_GPU/gem5.opt ../gem5-gpu/configs/se_fusion.py -c /mnt/d/source/github/sim/gem5-gpu/benchmarks/rodinia/backprop/gem5_fusion_backprop -o 16
#build/X86_VI_hammer_GPU/gem5.debug $OPTION ../gem5-gpu/configs/se_fusion.py -c /mnt/d/source/github/sim/gem5-gpu/benchmarks/rodinia/backprop/gem5_fusion_backprop -o 16

#build/X86_VI_hammer_GPU/gem5.opt ../gem5-gpu/configs/se_fusion.py -c /mnt/d/source/github/sim/gem5-gpu/benchmarks/rodinia/vectoradd/simple

#build/X86_VI_hammer_GPU/gem5.opt ../gem5-gpu/configs/se_fusion.py --helon

