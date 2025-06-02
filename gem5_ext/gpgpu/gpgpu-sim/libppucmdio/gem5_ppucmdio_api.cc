
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdarg.h>

#include <unistd.h>
#include <stdint.h>
#include "cmdio.h"
#include "gem5_ppucmdio_api.h"


#ifndef __MEM_DEBUG__
// Wrap m5op in a namespace so calls to m5_gpu can be intercepted and
// pre-processed out if debugging or running tests on hardware
namespace m5op {
    extern "C" {
        #include "m5op.h"
    }
}
#endif

inline void m5_ppu(uint64_t __gpusysno, uint64_t call_params) {
#ifndef __MEM_DEBUG__
    m5op::m5_ppu(__gpusysno, (void*)call_params);
#endif
}

//cudaError_t g_last_cudaError = cudaSuccess;
//extern cudaError_t g_last_cudaError;

extern "C" {

void* cmd_open(uint32_t device_num)
{
    gpusyscall_t call_params;
    call_params.num_args = 1;
    call_params.arg_lengths = new int[call_params.num_args];
    call_params.arg_lengths[0] = sizeof(uint32_t);
    call_params.total_bytes = call_params.arg_lengths[0];

    call_params.args = new char[call_params.total_bytes];
    call_params.ret = new char[sizeof(void*)];

    int bytes_off = 0;
    int lengths_off = 0;
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&device_num, call_params.arg_lengths[0]);

    // cmd_open
    m5_ppu(0, (uint64_t)&call_params);
    void* handle = *((void**)call_params.ret);

    delete[] call_params.args;
    delete[] call_params.arg_lengths;
    delete[] call_params.ret;
    return handle;
}


int cmd_create_queue(ioctl_create_queue_args *args)
{
    gpusyscall_t call_params;
    call_params.num_args = 1;
    call_params.arg_lengths = new int[call_params.num_args];
    call_params.arg_lengths[0] = sizeof(ioctl_create_queue_args*);
    call_params.total_bytes = call_params.arg_lengths[0];

    call_params.args = new char[call_params.total_bytes];
    call_params.ret = new char[sizeof(int)];

    int bytes_off = 0;
    int lengths_off = 0;
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&args, call_params.arg_lengths[0]);

    // cmd_create_queue
    m5_ppu(1, (uint64_t)&call_params);
    int ret = *((int*)call_params.ret);

    delete[] call_params.args;
    delete[] call_params.arg_lengths;
    delete[] call_params.ret;

    return ret;
}

int cmd_alloc_memory(ioctl_alloc_memory_args *args)
{
    gpusyscall_t call_params;
    call_params.num_args = 1;
    call_params.arg_lengths = new int[call_params.num_args];
    call_params.arg_lengths[0] = sizeof(ioctl_alloc_memory_args*);
    call_params.total_bytes = call_params.arg_lengths[0];

    call_params.args = new char[call_params.total_bytes];
    call_params.ret = new char[sizeof(int)];

    int bytes_off = 0;
    int lengths_off = 0;
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&args, call_params.arg_lengths[0]);

    // cmd_alloc_memory
    m5_ppu(2, (uint64_t)&call_params);
    int ret = *((int*)call_params.ret);

    delete[] call_params.args;
    delete[] call_params.arg_lengths;
    delete[] call_params.ret;

    return ret;
}


int cmd_read_register(uint32_t index, uint32_t *value)
{
    gpusyscall_t call_params;
    call_params.num_args = 2;
    call_params.arg_lengths = new int[call_params.num_args];
    call_params.arg_lengths[0] = sizeof(index);
    call_params.arg_lengths[1] = sizeof(&value);
    call_params.total_bytes = call_params.arg_lengths[0] + call_params.arg_lengths[1];

    call_params.args = new char[call_params.total_bytes];
    call_params.ret = new char[sizeof(int)];

    int bytes_off = 0;
    int lengths_off = 0;
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&index, call_params.arg_lengths[0]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&value, call_params.arg_lengths[1]);

    // cmd_read_register
    m5_ppu(3, (uint64_t)&call_params);
    int ret = *((int*)call_params.ret);

    delete[] call_params.args;
    delete[] call_params.arg_lengths;
    delete[] call_params.ret;

    return ret;
}


int cmd_write_register(uint32_t index, uint32_t value)
{
    gpusyscall_t call_params;
    call_params.num_args = 2;
    call_params.arg_lengths = new int[call_params.num_args];
    call_params.arg_lengths[0] = sizeof(index);
    call_params.arg_lengths[1] = sizeof(value);
    call_params.total_bytes = call_params.arg_lengths[0] + call_params.arg_lengths[1];

    call_params.args = new char[call_params.total_bytes];
    call_params.ret = new char[sizeof(int)];

    int bytes_off = 0;
    int lengths_off = 0;
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&index, call_params.arg_lengths[0]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&value, call_params.arg_lengths[1]);

    // cmd_write_register
    m5_ppu(4, (uint64_t)&call_params);
    int ret = *((int*)call_params.ret);

    delete[] call_params.args;
    delete[] call_params.arg_lengths;
    delete[] call_params.ret;

    return ret;
}

}
