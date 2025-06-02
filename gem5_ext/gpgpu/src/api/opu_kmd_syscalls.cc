
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <string>


#include "api/opu_kmd_syscalls.hh"
#include "api/gpu_syscall_helper.hh"
#include "cpu/thread_context.hh"
#include "debug/OPUKMDSyscalls.hh"
// #include "gpgpu-sim/gpu-sim.h"
// #include "gpgpusim_entrypoint.h"
// #include "gpu/gpgpu-sim/cuda_gpu.hh"
// #include "cmdio.h"
enum cmdioError
{
    cmdioSuccess                           =      0,   // No errors
};
typedef enum cmdioError cmdioError_t;

#define MAX_STRING_LEN 1000

// cmdioError_t g_last_cmdioError = cmdioSuccess;
cmdioError_t g_last_cmdioError = cmdioSuccess;

extern ThreadContext* create_thread_context();

extern "C" void app_direct_call_opu_kmd(uint64_t gpusysno, uint64_t call_params)
{
    ThreadContext *tc = create_thread_context();
    opu_kmd_funcs[gpusysno](tc, (gpusyscall_t*)call_params);
}

void opu_open(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);

    uint32_t device_num = *((uint32_t*)helper.getParam(0, true));


    DPRINTF(OPUKMDSyscalls, "gem5 OPU Kmd Syscall: open(device_num = %d)\n", device_num);

    g_last_cmdioError = cmdioSuccess;

    //int64_t handle = (int64_t)cmd_open(device_num);
    //ioctl_open_args args;
    //helper.setReturn((uint8_t*)&(args.handle), sizeof(int64_t));
    // Addr addr = cudaGPU->allocateGPUMemory(sim_size);

    //helper.setReturn((uint8_t*)&handle, sizeof(int64_t));
}


void create_queue(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);

    Addr arg_in = *((Addr*)helper.getParam(0, true));

    size_t arg_size = 0; // sizeof(ioctl_create_queue_args);

    const void* arg = new uint8_t[arg_size];
    helper.readBlob(arg_in, (uint8_t*)(arg), arg_size);

    DPRINTF(OPUKMDSyscalls, "gem5 OPU CmdIO Syscall: create_queue(ioctl_create_queue_args = %x)\n", arg);

    g_last_cmdioError = cmdioSuccess;

    int ret; //  = cmd_create_queue((ioctl_create_queue_args*)arg);

    helper.writeBlob(arg_in, (uint8_t*)(arg), arg_size);

    // delete arg[];

    helper.setReturn((uint8_t*)&ret, sizeof(cmdioError_t));
}

void alloc_memory(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);

    Addr arg_in = *((Addr*)helper.getParam(0, true));

    size_t arg_size = 0; // sizeof(ioctl_alloc_memory_args);

    const void* arg = new uint8_t[arg_size];
    helper.readBlob(arg_in, (uint8_t*)(arg), arg_size);

    DPRINTF(OPUKMDSyscalls, "gem5 OPU CmdIO Syscall: alloc_memory(ioctl_alloc_memory_args = %x)\n", arg);

    g_last_cmdioError = cmdioSuccess;

    int ret; //  = cmd_alloc_memory((ioctl_alloc_memory_args*)arg);

    helper.writeBlob(arg_in, (uint8_t*)(arg), arg_size);

    // delete arg;

    helper.setReturn((uint8_t*)&ret, sizeof(cmdioError_t));
}

void read_register(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);

    uint32_t arg1_in = *((uint32_t*)helper.getParam(0));
    Addr arg2_in = *((Addr*)helper.getParam(1, true));


    DPRINTF(OPUKMDSyscalls, "gem5 OPU CmdIO Syscall: read_register(index=%d, value= %x)\n", arg1_in, arg2_in);

    g_last_cmdioError = cmdioSuccess;

    uint32_t value;

    int ret; //  = cmd_read_register(arg1_in, &value);

    helper.writeBlob(arg2_in, (uint8_t*)(&value), 1, true);

    helper.setReturn((uint8_t*)&ret, sizeof(cmdioError_t));
}

void write_register(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);

    uint32_t arg1_in = *((uint32_t*)helper.getParam(0));
    uint32_t arg2_in = *((uint32_t*)helper.getParam(1));


    DPRINTF(OPUKMDSyscalls, "gem5 OPU CmdIO Syscall: write_register(index=%d, value= %d)\n", arg1_in, arg2_in);

    g_last_cmdioError = cmdioSuccess;

    int ret; //  = cmd_write_register(arg1_in, arg2_in);

    helper.setReturn((uint8_t*)&ret, sizeof(cmdioError_t));
}

