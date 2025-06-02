Thare many mode to run application, the build step is dependent the mode the app run:

> git clone https://github.com/chisuhua/gem5.git
> cd gem5; 
> git sumodule udpate --init

. setup env
> cd design
> source setup_env.sh

. build gem5
> cd design
> build.sh

. build libgem5
> cd design
> ./build_libgem5.sh

. build cosim
> cd design/cosim
> scons

if only run isasim, it can only build libcuda
. build libcuda
> cd design
> source setup_env.sh
> cd libcuda; make



## run howto

it require gpgpusim.config in run directory

### choosing run with ptx or opu isa

- ptx
`
-gpgpu_ptx_sim_mode 1
`

- opu isa
`
-gpgpu_ptx_sim_mode 2
`

### setting ptx umd mode

- run ptx with libcuda
`
-gpgpu_umd_mode 0
`
run with libcuda ptx

- run ptx with gem5 libcuda
`
-gpgpu_umd_mode >0
`

### setting opuisa umd mode

1. isasim or libcuda ptx simulation mode
`
-gpgpu_umd_mode 0
`
  - only libcuda code is used
  - libcuda will spawn thread to process cuda operation

2. run app with gem5

if run inside gem5, nee to update design/gpgpu/gpgpu-sim/gem5.env
below is assume run outside gem5, when any gem5xxx api called in cuda_runtime_api will call m5_gpu , which invoke gem5

`
-gpgpu_umd_mode 1
`
  - using umd platform libcuda, which call libcuda code
    - gpgpu-sim/src/gpgpusim_entrypoint.cc start_sim_thread will spawn processing thread
  - NOTE: looks broken

`
-gpgpu_umd_mode 2
`
  - using umd platform libgem5cuda, which invoke cuda_gpu cycles to process cuda command


at begining, run design/opu/oputest/cuda_samples/smoke test to verify it works
currently there are tests in smoke:
- vectorCopy
- vectorAdd
- vectorSmem
- matrixMul
- simpleTensorCoreGemm
- shmemTensorCoreGemm

`
-gpgpu_umd_mode 3
`
run opu umd

`
-gpgpu_umd_mode 4
`
run opu kmd

## debug howto

- the app run in gem5 is debugged by remote gdb

1. use -w option with gem5.debug
2. run command in remote gdb
> set remote z-packet on
> set debug serial 1
> #handle SIGPIPE nostop print
> #handle SIGIO nostop noprint
> #handle SIGTRAP nostop noprint
> file ....
> target remote localhost:7000

note: you can use design/.gdbini, which run gdb at design directory

- debug ptx
modify gpgpusim.config for below:
`
-trace_enabled 1
-gpgpu_ptx_inst_debug_to_file 1
-gpgpu_ptx_inst_debug_thread_uid 1
`



## m5 cpu/gpu api syscall

the m5 op is define in util/m5/m5op_x85.S, which define the  x86 macro instruction.

代码中插入, m5op::m5_gpu(__gpusysno, call_params)
来执行

插入后的代码，会被src/arch/x86/isa/decoder/two_byte_opcodes.isa
解析后会调用相应的PseudoInst

src/sim/PseudoInst.cc 定义了通过PseduoInst调用的系统调用，
比如m5_gpu的系统调用
PseudoInst::gpu

其中调用的函数gpgpu_funcs
gpugpu_funcs定义在design/gpgpu/src/api/libcuda_syscalls.cc

## mmu

since app run outside gem5, it use AppThreadContext as faked context to run gem5 cuda api 
we use setting
- manageGPUMemory is true for libgem5cuda
- accessHostPage is false
	we use hdp to read/write app allocated memory data
	and hack ShaderTLB::beginTranslating to use paddr same as vaddr

## config.ini setup procedure

### se_fusion.py
  - GPUConfig.craeteGPU (design/cosim/configs/GPUConfigs.py) to setup gpu
  - Ruby.create_system (
		config/Ruby
		design/cosim/configs/gpu_protocal/VI_hammer_fusion.py
		design/cosim/configs/gpu_protocal/VI_hammer.py
	  )
  - GPUConfig.connectGPUPorts

