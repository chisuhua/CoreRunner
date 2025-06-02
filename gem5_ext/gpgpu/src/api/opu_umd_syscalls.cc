// This file created from opu_runtime_api.h distributed with opu 1.1
// Changes Copyright 2009,  Tor M. Aamodt, Ali Bakhoda and George L. Yuan
// University of British Columbia

/*
 * opu_syscalls.cc
 *
 * Copyright © 2009 by Tor M. Aamodt, Wilson W. L. Fung, Ali Bakhoda,
 * George L. Yuan and the University of British Columbia, Vancouver,
 * BC V6T 1Z4, All Rights Reserved.
 *
 * THIS IS A LEGAL DOCUMENT BY DOWNLOADING GPGPU-SIM, YOU ARE AGREEING TO THESE
 * TERMS AND CONDITIONS.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * NOTE: The files libopu/opu_runtime_api.c and src/opu-sim/opu-math.h
 * are derived from the opu Toolset available from http://www.nvidia.com/opu
 * (property of NVIDIA).  The files benchmarks/BlackScholes/ and
 * benchmarks/template/ are derived from the opu SDK available from
 * http://www.nvidia.com/opu (also property of NVIDIA).  The files from
 * src/intersim/ are derived from Booksim (a simulator provided with the
 * textbook "Principles and Practices of Interconnection Networks" available
 * from http://cva.stanford.edu/books/ppin/). As such, those files are bound by
 * the corresponding legal terms and conditions set forth separately (original
 * copyright notices are left in files from these sources and where we have
 * modified a file our copyright notice appears before the original copyright
 * notice).
 *
 * Using this version of GPGPU-Sim requires a complete installation of opu
 * which is distributed seperately by NVIDIA under separate terms and
 * conditions.  To use this version of GPGPU-Sim with OpenCL requires a
 * recent version of NVIDIA's drivers which support OpenCL.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the University of British Columbia nor the names of
 * its contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * 4. This version of GPGPU-SIM is distributed freely for non-commercial use only.
 *
 * 5. No nonprofit user may place any restrictions on the use of this software,
 * including as modified by the user, by any other authorized user.
 *
 * 6. GPGPU-SIM was developed primarily by Tor M. Aamodt, Wilson W. L. Fung,
 * Ali Bakhoda, George L. Yuan, at the University of British Columbia,
 * Vancouver, BC V6T 1Z4
 */

/*
 * Copyright 1993-2007 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */

/*
 * Copyright (c) 2011-2013 Mark D. Hill and David A. Wood
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <string>

#ifdef OPENGL_SUPPORT
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h> // Apple's version of GLUT is here

#else
#include <GL/gl.h>

#endif
#endif

#include "api/opu_umd_syscalls.hh"
#include "api/gpu_syscall_helper.hh"
#include "cpu/thread_context.hh"
// #include "opu-sim/opu-sim.h"
// #include "opu-sim/ptx_ir.h"
// #include "opu-sim/ptx_loader.h"
// #include "opu-sim/ptx_parser.h"
#include "debug/GPUSyscalls.hh"
// #include "gpgpu-sim/gpu-sim.h"
// #include "gpgpusim_entrypoint.h"
#include "opu_top.hh"
#include "opu_stream.hh"
// #include "../libopu_sim/gpgpu_context.h"
#include "cpu/simple_thread.hh"
#include "arch/x86/vecregs.hh"

#define MAX_STRING_LEN 1000

typedef struct CUstream_st *opuStream_t;

static unsigned g_active_device = 0; // Active opu-enabled GPU that runs the code
static cudaError_t g_last_cudaError = cudaSuccess;

using namespace gem5;
/*
class AppSystem {
public:
    AppSystem() {}
};
*/
extern ThreadContext* create_thread_context();

extern "C" void app_direct_call_opu_umd(uint64_t gpusysno, uint64_t call_params)
{
    ThreadContext *tc = create_thread_context();
    opu_umd_funcs[gpusysno](tc, (gpusyscall_t*)call_params);
}

#if 0
kernel_info_t *gpgpu_opu_ptx_sim_init_grid(gpgpu_ptx_sim_arg_list_t args,
                                            struct dim3 gridDim,
                                            struct dim3 blockDim,
                                            function_info* entry)
{
   OpuTop *opu_top = OpuTop::getOpuTop(g_active_device);
   gpgpu_t *gpu = opu_top->getTheGPU();

   kernel_info_t *result = new kernel_info_t(gridDim,blockDim,entry, gpu->getNameArrayMapping(), gpu->getNameInfoMapping());

   if (entry == NULL) {
       panic("GPGPU-Sim PTX: ERROR launching kernel -- no PTX implementation found");
   }
   unsigned argcount=args.size();
   unsigned argn=1;
   for (gpgpu_ptx_sim_arg_list_t::iterator a = args.begin(); a != args.end(); a++) {
      entry->add_param_data(argcount-argn, &(*a));
      argn++;
   }

   entry->finalize(result->get_param_memory());
   // g_ptx_kernel_count++;

   return result;
}
#endif

#if defined __APPLE__
#   define __my_func__    __PRETTY_FUNCTION__
#else
# if defined __cplusplus ? __GNUC_PREREQ (2, 6) : __GNUC_PREREQ (2, 4)
#   define __my_func__    __PRETTY_FUNCTION__
# else
#  if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#   define __my_func__    __func__
#  else
#   define __my_func__    ((__const char *) 0)
#  endif
# endif
#endif

#if 0
class kernel_config {
  public:
    kernel_config(dim3 GridDim, dim3 BlockDim, size_t sharedMem, struct CUstream_st *stream)
    {
        m_GridDim=GridDim;
        m_BlockDim=BlockDim;
        m_sharedMem=sharedMem;
        m_stream = stream;
    }
    void set_arg(const void *arg, size_t size, size_t offset)
    {
        m_args.push_front(gpgpu_ptx_sim_arg(arg,size,offset));
    }
    dim3 grid_dim() const { return m_GridDim; }
    dim3 block_dim() const { return m_BlockDim; }
    gpgpu_ptx_sim_arg_list_t get_args() { return m_args; }
    struct CUstream_st *get_stream() { return m_stream; }

  private:
    dim3 m_GridDim;
    dim3 m_BlockDim;
    size_t m_sharedMem;
    struct CUstream_st *m_stream;
    gpgpu_ptx_sim_arg_list_t m_args;
};
#endif

using namespace gem5;
#if 0
extern "C" void gem5_ptxinfo_addinfo()
{
    if (!strcmp("__opu_dummy_entry__",get_ptxinfo_kname())) {
      // this string produced by ptxas for empty ptx files (e.g., bandwidth test)
        clear_ptxinfo();
        return;
    }
    OpuTop *opu_top = OpuTop::getOpuTop(g_active_device);
    print_ptxinfo();
    opu_top->add_ptxinfo(get_ptxinfo_kname(), get_ptxinfo());
    clear_ptxinfo();
}
#endif

void opu_not_implemented(const char* func, unsigned line)
{
    fflush(stdout);
    fflush(stderr);
    printf("\n\ngem5-gpu opu: Execution error: opu API function \"%s()\" has not been implemented yet.\n"
            "                 [gem5-gpu/src/gem5/%s around line %u]\n\n\n",
    func,__FILE__, line);
    fflush(stdout);
    abort();
}

// typedef std::map<unsigned,CUevent_st*> event_tracker_t;

// int CUevent_st::m_next_event_uid;
// event_tracker_t g_timer_events;
//std::list<kernel_config> g_opu_launch_stack;

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

void
libopuMalloc(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_devPtr = *((Addr*)helper.getParam(0, true));
    size_t sim_size = *((size_t*)helper.getParam(1));
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: opuMalloc(devPtr = %x, size = %d)\n", sim_devPtr, sim_size);

    g_last_cudaError = cudaSuccess;

    OpuTop *opu_top = OpuTop::getOpuTop(g_active_device);

    if (!opu_top->isManagingGPUMemory()) {
        // Tell opu runtime to allocate memory
        cudaError_t to_return = cudaErrorApiFailureBase;
        helper.setReturn((uint8_t*)&to_return, sizeof(cudaError_t));
        return;
    } else {
        Addr addr = opu_top->allocateGPUMemory(sim_size);
        helper.writeBlob(sim_devPtr, (uint8_t*)(&addr), sizeof(Addr), true);
        if (addr) {
            g_last_cudaError = cudaSuccess;
        } else {
            g_last_cudaError = cudaErrorMemoryAllocation;
        }
        helper.setReturn((uint8_t*)&g_last_cudaError, sizeof(cudaError_t));
    }
}

void
libopuMallocHost(ThreadContext *tc, gpusyscall_t *call_params) {
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_ptr = *((Addr*)helper.getParam(0, true));
    size_t sim_size = *((size_t*)helper.getParam(1));
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: opuMallocHost(ptr = %x, size = %d)\n", sim_ptr, sim_size);

    g_last_cudaError = cudaSuccess;
    // Tell opu runtime to allocate memory
    cudaError_t to_return = cudaErrorApiFailureBase;
    helper.setReturn((uint8_t*)&to_return, sizeof(cudaError_t));
}

void
libopuRegisterDeviceMemory(ThreadContext *tc, gpusyscall_t *call_params)
{
    // This GPU syscall is used to initialize tracking of GPU memory so that
    // the GPU can do TLB lookups and if necessary, physical memory allocations
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_devicePtr = *((Addr*)helper.getParam(0, true));
    size_t sim_size = *((size_t*)helper.getParam(1));
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: opuRegisterDeviceMemory(devicePtr = %x, size = %d)\n", sim_devicePtr, sim_size);

    OpuTop *opu_top = OpuTop::getOpuTop(g_active_device);
    opu_top->registerDeviceMemory(tc, sim_devicePtr, sim_size);
}

void
opuMallocPitch(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

//__host__ cudaError_t opuRTAPI opuMallocArray(struct opuArray **array, const struct opuChannelFormatDesc *desc, size_t width, size_t height __dv(1)) {
void
opuMallocArray(ThreadContext *tc, gpusyscall_t *call_params) {
    opu_not_implemented(__my_func__,__LINE__);
}

void
libopuFree(ThreadContext *tc, gpusyscall_t *call_params) {
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_devPtr = *((Addr*)helper.getParam(0, true));
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: opuFree(devPtr = %x)\n", sim_devPtr);

    OpuTop *opu_top = OpuTop::getOpuTop(g_active_device);

    if (!opu_top->isManagingGPUMemory()) {
        g_last_cudaError = cudaSuccess;
        // Tell opu runtime to free memory
        cudaError_t to_return = cudaErrorApiFailureBase;
        helper.setReturn((uint8_t*)&to_return, sizeof(cudaError_t));
    } else {
        // TODO: Tell SPA to free this memory
        cudaError_t to_return = cudaSuccess;
        helper.setReturn((uint8_t*)&to_return, sizeof(cudaError_t));
    }
}

void
libopuFreeHost(ThreadContext *tc, gpusyscall_t *call_params) {
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_ptr = *((Addr*)helper.getParam(0, true));
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: opuFreeHost(ptr = %x)\n", sim_ptr);

    g_last_cudaError = cudaSuccess;
    // Tell opu runtime to free memory
    cudaError_t to_return = cudaErrorApiFailureBase;
    helper.setReturn((uint8_t*)&to_return, sizeof(cudaError_t));
}

//__host__ cudaError_t opuRTAPI opuFreeArray(struct opuArray *array){
void
opuFreeArray(ThreadContext *tc, gpusyscall_t *call_params) {
    opu_not_implemented(__my_func__,__LINE__);
};


/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

void
libopuMemcpy(ThreadContext *tc, gpusyscall_t *call_params) {
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_dst = *((Addr*)helper.getParam(0, true));
    Addr sim_src = *((Addr*)helper.getParam(1, true));
    size_t sim_count = *((size_t*)helper.getParam(2));
    enum cudaMemcpyKind sim_kind = *((enum cudaMemcpyKind*)helper.getParam(3));

    OpuTop *opu_top = OpuTop::getOpuTop(g_active_device);

    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: opuMemcpy(dst = %x, src = %x, count = %d, kind = %s)\n",
            sim_dst, sim_src, sim_count, opuMemcpyKindStrings[sim_kind]);

    bool suspend = false;
    if (sim_count == 0) {
        g_last_cudaError = cudaSuccess;
        helper.setReturn((uint8_t*)&suspend, sizeof(bool));
        return;
    }

    if (sim_kind == opuMemcpyHostToDevice) {
        stream_operation mem_op((const void*)sim_src, (size_t)sim_dst, sim_count, 0);
        mem_op.setThreadContext(tc);
        opu_top->getStreamManager()->push(mem_op);
    } else if (sim_kind == opuMemcpyDeviceToHost) {
        stream_operation mem_op((size_t)sim_src, (void*)sim_dst, sim_count, 0);
        mem_op.setThreadContext(tc);
        opu_top->getStreamManager()->push(mem_op);
    } else if (sim_kind == opuMemcpyDeviceToDevice) {
        stream_operation mem_op((size_t)sim_src, (size_t)sim_dst, sim_count, 0);
        mem_op.setThreadContext(tc);
        opu_top->getStreamManager()->push(mem_op);
    } else {
        panic("GPGPU-Sim PTX: opuMemcpy - ERROR : unsupported cudaMemcpyKind\n");
    }

    suspend = opu_top->needsToBlock();
    assert(suspend);
    g_last_cudaError = cudaSuccess;
    helper.setReturn((uint8_t*)&suspend, sizeof(bool));
}

//__host__ cudaError_t opuRTAPI opuMemcpyToArray(struct opuArray *dst, size_t wOffset, size_t hOffset, const void *src, size_t count, enum cudaMemcpyKind kind) {
void
opuMemcpyToArray(ThreadContext *tc, gpusyscall_t *call_params) {
    opu_not_implemented(__my_func__,__LINE__);
}

//__host__ cudaError_t opuRTAPI opuMemcpyFromArray(void *dst, const struct opuArray *src, size_t wOffset, size_t hOffset, size_t count, enum cudaMemcpyKind kind) {
void
opuMemcpyFromArray(ThreadContext *tc, gpusyscall_t *call_params) {
    opu_not_implemented(__my_func__,__LINE__);
}

//__host__ cudaError_t opuRTAPI opuMemcpyArrayToArray(struct opuArray *dst, size_t wOffsetDst, size_t hOffsetDst,
//                                                      const struct opuArray *src, size_t wOffsetSrc, size_t hOffsetSrc,
//                                                      size_t count, enum cudaMemcpyKind kind __dv(opuMemcpyDeviceToDevice)) {
void
opuMemcpyArrayToArray(ThreadContext *tc, gpusyscall_t *call_params) {
    opu_not_implemented(__my_func__,__LINE__);
}

//__host__ cudaError_t opuRTAPI opuMemcpy2D(void *dst, size_t dpitch, const void *src, size_t spitch, size_t width,
//                                            size_t height, enum cudaMemcpyKind kind) {
void
opuMemcpy2D(ThreadContext *tc, gpusyscall_t *call_params) {
    opu_not_implemented(__my_func__,__LINE__);
}

//__host__ cudaError_t opuRTAPI opuMemcpy2DToArray(struct opuArray *dst, size_t wOffset, size_t hOffset, const void *src,
//                                                  size_t spitch, size_t width, size_t height, enum cudaMemcpyKind kind) {
void
opuMemcpy2DToArray(ThreadContext *tc, gpusyscall_t *call_params) {
    opu_not_implemented(__my_func__,__LINE__);
}

//__host__ cudaError_t opuRTAPI opuMemcpy2DFromArray(void *dst, size_t dpitch, const struct opuArray *src, size_t wOffset,
//                                                  size_t hOffset, size_t width, size_t height, enum cudaMemcpyKind kind) {
void
opuMemcpy2DFromArray(ThreadContext *tc, gpusyscall_t *call_params) {
    opu_not_implemented(__my_func__,__LINE__);
}

//__host__ cudaError_t opuRTAPI opuMemcpy2DArrayToArray(struct opuArray *dst, size_t wOffsetDst, size_t hOffsetDst,
//                                                      const struct opuArray *src, size_t wOffsetSrc, size_t hOffsetSrc,
//                                                      size_t width, size_t height, enum cudaMemcpyKind kind __dv(opuMemcpyDeviceToDevice)) {
void
opuMemcpy2DArrayToArray(ThreadContext *tc, gpusyscall_t *call_params) {
    opu_not_implemented(__my_func__,__LINE__);
}

void
libopuMemcpyToSymbol(ThreadContext *tc, gpusyscall_t *call_params) {
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_symbol = *((Addr*)helper.getParam(0, true));
    Addr sim_src = *((Addr*)helper.getParam(1, true));
    size_t sim_count = *((size_t*)helper.getParam(2));
    size_t sim_offset = *((size_t*)helper.getParam(3));
    enum cudaMemcpyKind sim_kind = *((enum cudaMemcpyKind*)helper.getParam(4));

    OpuTop *opu_top = OpuTop::getOpuTop(g_active_device);

    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: opuMemcpyToSymbol(symbol = %x, src = %x, count = %d, offset = %d, kind = %s)\n",
            sim_symbol, sim_src, sim_count, sim_offset, opuMemcpyKindStrings[sim_kind]);

    assert(sim_kind == opuMemcpyHostToDevice);
    stream_operation mem_op((const void*)sim_src, (const char*)sim_symbol, sim_count, sim_offset, NULL);
    mem_op.setThreadContext(tc);
    opu_top->getStreamManager()->push(mem_op);

    bool suspend = opu_top->needsToBlock();
    assert(suspend);
    g_last_cudaError = cudaSuccess;
    helper.setReturn((uint8_t*)&suspend, sizeof(bool));
}

void
libopuMemcpyFromSymbol(ThreadContext *tc, gpusyscall_t *call_params) {
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_dst = *((Addr*)helper.getParam(0, true));
    Addr sim_symbol = *((Addr*)helper.getParam(1, true));
    size_t sim_count = *((size_t*)helper.getParam(2));
    size_t sim_offset = *((size_t*)helper.getParam(3));
    enum cudaMemcpyKind sim_kind = *((enum cudaMemcpyKind*)helper.getParam(4));

    OpuTop *opu_top = OpuTop::getOpuTop(g_active_device);

    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: opuMemcpyToSymbol(symbol = %x, src = %x, count = %d, offset = %d, kind = %s)\n",
            sim_symbol, sim_dst, sim_count, sim_offset, opuMemcpyKindStrings[sim_kind]);

    assert(sim_kind == opuMemcpyDeviceToHost);
    stream_operation mem_op((const char*)sim_symbol, (void*)sim_dst, sim_count, sim_offset, NULL);
    mem_op.setThreadContext(tc);
    opu_top->getStreamManager()->push(mem_op);

    bool suspend = opu_top->needsToBlock();
    assert(suspend);
    g_last_cudaError = cudaSuccess;
    helper.setReturn((uint8_t*)&suspend, sizeof(bool));
}

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

//	__host__ cudaError_t opuRTAPI opuMemcpyAsync(void *dst, const void *src, size_t count, enum cudaMemcpyKind kind, opuStream_t stream)
void
opuMemcpyAsync(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

//	__host__ cudaError_t opuRTAPI opuMemcpyToArrayAsync(struct opuArray *dst, size_t wOffset, size_t hOffset,
//	                                                        const void *src, size_t count, enum cudaMemcpyKind kind, opuStream_t stream)
void
opuMemcpyToArrayAsync(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

//	__host__ cudaError_t opuRTAPI opuMemcpyFromArrayAsync(void *dst, const struct opuArray *src, size_t wOffset, size_t hOffset,
//	                                                        size_t count, enum cudaMemcpyKind kind, opuStream_t stream)
void
opuMemcpyFromArrayAsync(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

//	__host__ cudaError_t opuRTAPI opuMemcpy2DAsync(void *dst, size_t dpitch, const void *src, size_t spitch, size_t width,
//	                                                size_t height, enum cudaMemcpyKind kind, opuStream_t stream)
void
opuMemcpy2DAsync(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

void
opuMemcpy2DToArrayAsync(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

void
opuMemcpy2DFromArrayAsync(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

void
opuBlockThread(ThreadContext *tc, gpusyscall_t *call_params)
{
    // Similar to futex in syscalls, except we need to track the variable to
    // be set
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);
    Addr sim_is_free_ptr = *((Addr*)helper.getParam(0, true));

    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: opuBlockThread(tc = %x, is_free_ptr = %x)\n", tc, sim_is_free_ptr);

    OpuTop *opu_top = OpuTop::getOpuTop(g_active_device);
    opu_top->blockThread(tc, sim_is_free_ptr);
}

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

void
libopuMemset(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_mem = *((Addr*)helper.getParam(0, true));
    int sim_c = *((int*)helper.getParam(1));
    size_t sim_count = *((size_t*)helper.getParam(2));
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: opuMemset(mem = %x, c = %d, count = %d)\n", sim_mem, sim_c, sim_count);

    OpuTop *opu_top = OpuTop::getOpuTop(g_active_device);

    if (!opu_top->isManagingGPUMemory() && !opu_top->isAccessingHostPagetable()) {
        // Signal to libopu that it should handle the memset. This is required
        // if the copy engine may be unable to access the CPU's pagetable to get
        // address translations (unified memory without access host pagetable)
        g_last_cudaError = cudaErrorApiFailureBase;
        helper.setReturn((uint8_t*)&g_last_cudaError, sizeof(cudaError_t));
    } else {
        stream_operation mem_op((size_t)sim_mem, sim_c, sim_count, 0);
        mem_op.setThreadContext(tc);
        opu_top->getStreamManager()->push(mem_op);
        g_last_cudaError = cudaSuccess;
        helper.setReturn((uint8_t*)&g_last_cudaError, sizeof(cudaError_t));

        bool suspend = opu_top->needsToBlock();
        assert(suspend);
    }
}

void
opuMemset2D(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

void
opuGetSymbolAddress(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

void
opuGetSymbolSize(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

void
libopuGetDeviceCount(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);
    Addr sim_count = *((Addr*)helper.getParam(0, true));

    int count = OpuTop::getNumOpuDevices();
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: opuGetDeviceCount(count* = %x) = %d\n", sim_count, count);

    helper.writeBlob(sim_count, (uint8_t*)(&count), sizeof(int));
    g_last_cudaError = cudaSuccess;
}

void
libopuGetDeviceProperties(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_prop = *((Addr*)helper.getParam(0, true));
    int sim_device = *((int*)helper.getParam(1));
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: opuGetDeviceProperties(prop* = %x, device = %d)\n", sim_prop, sim_device);
    OpuTop *opu_top = OpuTop::getOpuTop(g_active_device);
    if (sim_device <= OpuTop::getNumOpuDevices())  {
        OpuTop::OpuDeviceProperties *prop = opu_top->getDeviceProperties();
        helper.writeBlob(sim_prop, (uint8_t*)(prop), sizeof(OpuTop::OpuDeviceProperties));
        g_last_cudaError = cudaSuccess;
    } else {
        g_last_cudaError = cudaErrorInvalidDevice;
    }
    helper.setReturn((uint8_t*)&g_last_cudaError, sizeof(cudaError_t));
}

void
opuChooseDevice(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

void
libopuSetDevice(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);

    int sim_device = *((int*)helper.getParam(0));
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: opuSetDevice(device = %d)\n", sim_device);
    if (sim_device <= OpuTop::getNumOpuDevices()) {
        g_active_device = sim_device;
        g_last_cudaError = cudaSuccess;
    } else {
        g_last_cudaError = cudaErrorInvalidDevice;
    }
    helper.setReturn((uint8_t*)&g_last_cudaError, sizeof(cudaError_t));
}

void
libopuGetDevice(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_device = *((Addr*)helper.getParam(0, true));
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: opuGetDevice(device = 0x%x)\n", sim_device);
    if (g_active_device <= OpuTop::getNumOpuDevices()) {
        helper.writeBlob(sim_device, (uint8_t*)&g_active_device, sizeof(int));
        g_last_cudaError = cudaSuccess;
    } else {
        g_last_cudaError = cudaErrorInvalidDevice;
    }
    helper.setReturn((uint8_t*)&g_last_cudaError, sizeof(cudaError_t));
}

/*******************************************************************************
 *                                                                              *
 *                                                                              *
 *                                                                              *
*******************************************************************************/

// __host__ cudaError_t opuRTAPI opuBindTexture(size_t *offset, const struct textureReference *texref, const void *devPtr,
//                                                      const struct opuChannelFormatDesc *desc, size_t size __dv(UINT_MAX))
void
opuBindTexture(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    opu_not_implemented(__my_func__,__LINE__);
}

void
opuBindTextureToArray(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

void
opuUnbindTexture(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

void
opuGetTextureAlignmentOffset(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

void
opuGetTextureReference(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

/*******************************************************************************
 *                                                                              *
 *                                                                              *
 *                                                                              *
*******************************************************************************/

void
opuGetChannelDesc(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

void
opuCreateChannelDesc(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

/*******************************************************************************
 *                                                                              *
 *                                                                              *
 *                                                                              *
*******************************************************************************/

void
opuGetLastError(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: opuGetLastError()\n");
    helper.setReturn((uint8_t*)&g_last_cudaError, sizeof(cudaError_t));
}

void
opuGetErrorString(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

/*******************************************************************************
 *                                                                              *
 *                                                                              *
 *                                                                              *
*******************************************************************************/
void libgem5opuConfigureCall(ThreadContext *tc, gpusyscall_t *call_params) {
    opu_not_implemented(__my_func__,__LINE__);
}

#if 0
void libgem5opuConfigureCall(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);

    dim3 sim_gridDim = *((dim3*)helper.getParam(0));
    dim3 sim_blockDim = *((dim3*)helper.getParam(1));
    size_t sim_sharedMem = *((size_t*)helper.getParam(2));
    opuStream_t sim_stream = *((opuStream_t*)helper.getParam(3));
    if (sim_stream) {
        panic("gem5-fusion doesn't currently support opu streams");
    }
    assert(!sim_stream); // We do not currently support opu streams
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: opuConfigureCall(tc = %p, gridDim = (%u,%u,%u), blockDim = (%u,%u,%u), sharedMem = %u, stream)\n",
            tc, sim_gridDim.x, sim_gridDim.y, sim_gridDim.z, sim_blockDim.x,
            sim_blockDim.y, sim_blockDim.z, sim_sharedMem);

    g_opu_launch_stack.push_back(kernel_config(sim_gridDim, sim_blockDim, sim_sharedMem, sim_stream));
    g_last_cudaError = cudaSuccess;
}
#endif

// void gem5opuSetupArgument_(ThreadContext *tc, gpusyscall_t *call_params){
void libgem5opuSetupArgument(ThreadContext *tc, gpusyscall_t *call_params){
    opu_not_implemented(__my_func__,__LINE__);
#if 0
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_arg = *((Addr*)helper.getParam(0, true));
    size_t sim_size = *((size_t*)helper.getParam(1));
    size_t sim_offset = *((size_t*)helper.getParam(2));

    const void* arg = new uint8_t[sim_size];
    helper.readBlob(sim_arg, (uint8_t*)arg, sim_size);
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: opuSetupArgument(tc = %p, arg = %x, size = %d, offset = %d) *arg = %p\n",
            tc, sim_arg, sim_size, sim_offset, arg);

    assert(!g_opu_launch_stack.empty());
    kernel_config &config = g_opu_launch_stack.back();
    config.set_arg(arg, sim_size, sim_offset);

    g_last_cudaError = cudaSuccess;
#endif
}

/*
void gem5opuSetupArgument_(GPUSyscallHelper &helper, Addr sim_arg, size_t sim_size, size_t sim_offset)
{
    const void* arg = new uint8_t[sim_size];
    helper.readBlob(sim_arg, (uint8_t*)arg, sim_size);
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: opuSetupArgument(arg = %x, size = %d, offset = %d) *arg = %p\n",
            sim_arg, sim_size, sim_offset, arg);

    assert(!g_opu_launch_stack.empty());
    kernel_config &config = g_opu_launch_stack.back();
    config.set_arg(arg, sim_size, sim_offset);
}


void libgem5opuSetupArgument(ThreadContext *tc, gpusyscall_t *call_params){
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);

    function_info* f = *((function_info**)helper.getParam(0, true));
    Addr sim_arg = *((Addr*)helper.getParam(1, true));
    unsigned num_args = f->num_args();

    Addr* args = new Addr[num_args];
    helper.readBlob(sim_arg, (uint8_t*)args, num_args * sizeof(void*));

    for(unsigned i = 0; i < num_args; i++){
       	std::pair<size_t, unsigned> p = f->get_param_config(i);
       	gem5opuSetupArgument_(helper, (Addr)(args[i]), p.first, p.second);
    }

    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: opuSetupArguments(tc = %p, arg = %x) *arg = %p\n",
            tc, sim_arg);

    g_last_cudaError = cudaSuccess;
}
*/
/*
#define CACHE_BLOCK_SIZE_BYTES 128
#define PAGE_SIZE_BYTES 4096

unsigned char touchPages(unsigned char *ptr, size_t size)
{
    unsigned char sum = 0;
    for (unsigned i = 0; i < size; i += PAGE_SIZE_BYTES) {
        sum += ptr[i];
    }
    sum += ptr[size-1];
    return sum;
}

__inline__ void *checkedAlignedAlloc(size_t size, size_t align_gran = CACHE_BLOCK_SIZE_BYTES)
{
    void *to_return = NULL;
    int error = posix_memalign(&to_return, align_gran, size);
    if (error) {
        fprintf(stderr, "ERROR: allocation failed with code: %d, Exiting...\n", error);
        exit(-1);
    }
    return to_return;
}
*/


void libgem5opuLaunch(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_hostFun = *((Addr*)helper.getParam(0, true));
    struct DispatchInfo *disp_info= *((DispatchInfo**)helper.getParam(1, true));
    struct gem5::Stream_st *stream= *((gem5::Stream_st**)helper.getParam(2, true));

    OpuTop *opu_top = OpuTop::getOpuTop(g_active_device);
    // assert(!g_opu_launch_stack.empty());
    // kernel_config config = g_opu_launch_stack.back();
    // struct CUstream_st *stream = config.get_stream();
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: opuLaunch(tc = %p, hostFun* = %x)\n", tc, sim_hostFun);
    // FIXME
    // kernel_info_t *grid = gpgpu_opu_ptx_sim_init_grid(config.get_args(), config.grid_dim(), config.block_dim(), opu_top->get_kernel((const char*)sim_hostFun));
    // grid->set_inst_base_vaddr(opu_top->getInstBaseVaddr());
    // std::string kname = grid->name();
    stream_operation op(disp_info, 0, stream);
    op.setThreadContext(tc);
    opu_top->getStreamManager()->push(op);
    // g_opu_launch_stack.pop_back();
    g_last_cudaError = cudaSuccess;
}

size_t getMaxThreadsPerBlock(struct OpuFuncAttributes attr) {
    OpuTop *opu_top = OpuTop::getOpuTop(g_active_device);
    OpuTop::OpuDeviceProperties *prop;

    prop = opu_top->getDeviceProperties();

    size_t max = prop->maxThreadsPerBlock;

    if ((prop->regsPerBlock / attr.numRegs) < max) {
        max = prop->regsPerBlock / attr.numRegs;
    }

    return max;
}

void
libopuFuncGetAttributes(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
#if 0
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_attr = *((Addr*)helper.getParam(0, true));
    Addr sim_hostFun = *((Addr*)helper.getParam(1, true));

    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: opuFuncGetAttributes(attr* = %x, hostFun* = %x)\n", sim_attr, sim_hostFun);

    OpuTop *opu_top = OpuTop::getOpuTop(g_active_device);
    function_info *entry = opu_top->get_kernel((const char*)sim_hostFun);

    if (entry) {
        const struct gpgpu_ptx_sim_info *kinfo = entry->get_kernel_info();
        OpuFuncAttributes attr;
        attr.sharedSizeBytes = kinfo->smem;
        attr.constSizeBytes  = kinfo->cmem;
        attr.localSizeBytes  = kinfo->lmem;
        attr.numRegs         = kinfo->regs;
        attr.maxThreadsPerBlock = getMaxThreadsPerBlock(attr);
        attr.ptxVersion      = kinfo->ptx_version;
        attr.binaryVersion   = kinfo->sm_target;
        helper.writeBlob(sim_attr, (uint8_t*)&attr, sizeof(OpuFuncAttributes));
        g_last_cudaError = cudaSuccess;
    } else {
        g_last_cudaError = cudaErrorInvalidDeviceFunction;
    }

    helper.setReturn((uint8_t*)&g_last_cudaError, sizeof(cudaError_t));
#endif
}

/*******************************************************************************
 *                                                                              *
 *                                                                              *
 *                                                                              *
*******************************************************************************/

// __host__ cudaError_t opuRTAPI opuStreamCreate(opuStream_t *stream)
void
opuStreamCreate(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

// __host__ cudaError_t opuRTAPI opuStreamDestroy(opuStream_t stream)
void
opuStreamDestroy(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

// __host__ cudaError_t opuRTAPI opuStreamSynchronize(opuStream_t stream)
void
opuStreamSynchronize(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

// __host__ cudaError_t opuRTAPI opuStreamQuery(opuStream_t stream)
void
opuStreamQuery(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

/*******************************************************************************
 *                                                                              *
 *                                                                              *
 *                                                                              *
*******************************************************************************/

// __host__ cudaError_t opuRTAPI opuEventCreate(opuEvent_t *event)
void
opuEventCreate(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

void
opuEventRecord(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

void
opuEventQuery(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

void
opuEventSynchronize(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

void
opuEventDestroy(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

void
opuEventElapsedTime(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

/*******************************************************************************
 *                                                                              *
 *                                                                              *
 *                                                                              *
*******************************************************************************/

void
opuThreadExit(ThreadContext *tc, gpusyscall_t *call_params)
{
    // This function should clean-up any/all resources associated with the
    // current device in the passed thread context
    opu_not_implemented(__my_func__,__LINE__);
}

void
opuThreadSynchronize(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: opuThreadSynchronize(), tc = %x\n", tc);
    OpuTop *opu_top = OpuTop::getOpuTop(g_active_device);
    bool suspend = opu_top->needsToBlock();
    g_last_cudaError = cudaSuccess;
    helper.setReturn((uint8_t*)&suspend, sizeof(bool));
}

void
__opuSynchronizeThreads(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

// symbol_table* registering_symtab = NULL;
// unsigned registering_fat_cubin_handle = 0;
// int registering_allocation_size = -1;
// Addr registering_allocation_ptr = 0;
// Addr registering_local_alloc_ptr = 0;
#if 0
unsigned
get_global_and_constant_alloc_size(symbol_table* symtab)
{
    unsigned total_bytes = 0;
    symbol_table::iterator iter;
    for (iter = symtab->global_iterator_begin(); iter != symtab->global_iterator_end(); iter++) {
        symbol* global = *iter;
        // if ( global->has_initializer() ) {
        if ( global) {
            total_bytes += global->get_size_in_bytes();
        }
    }

    for (iter = symtab->const_iterator_begin(); iter != symtab->const_iterator_end(); iter++) {
        symbol* constant = *iter;
        total_bytes += constant->get_size_in_bytes();
    }

    return total_bytes;
}


unsigned
get_local_alloc_size(OpuTop *opu_top) {
    unsigned cores = opu_top->getDeviceProperties()->multiProcessorCount;
    unsigned threads_per_core = opu_top->getMaxThreadsPerMultiprocessor();
    // NOTE: Per technical specs Wikipedia: http://en.wikipedia.org/wiki/opu
    // For opu GPUs with compute capability 1.x, each thread should be able to
    // access up to 16kB of memory, and for compute capability 2.x+, each
    // thread should be able to access up to 512kB of local memory. Since this
    // could blow out the simulator's memory footprint, here we use 8kB per
    // thread as a more reasonable baseline. This may need to be changed if
    // benchmarks trip on the GPGPU-Sim-side panic of too much local memory
    // usage per thread.
    //
    // FIXME
    // return 0;
    unsigned max_local_mem_per_thread = 8 * 1024;
    return cores * threads_per_core * max_local_mem_per_thread;
}
#endif

#if 0
void
finalize_global_and_constant_setup(ThreadContext *tc, Addr base_addr, symbol_table* symtab)
{
    opu_not_implemented(__my_func__,__LINE__);
    OpuTop *opu_top = OpuTop::getOpuTop(g_active_device);
    Addr curr_addr = base_addr;
    Addr next_addr = 0;
    symbol_table::iterator iter;
    for (iter = symtab->global_iterator_begin(); iter != symtab->global_iterator_end(); iter++) {
        symbol* global = *iter;
        global->set_address(curr_addr);
        opu_top->registerDeviceMemory(tc, curr_addr, global->get_size_in_bytes());
        next_addr = curr_addr + global->get_size_in_bytes();
        if (next_addr - base_addr > registering_allocation_size) {
            panic("Didn't allocate enough global+const memory. Bailing!");
        } else {
            DPRINTF(GPUSyscalls, "GPGPU-Sim PTX: Updated symbol \"%s\" to address range 0x%x to 0x%x\n", global->name(), curr_addr, next_addr-1);
        }
        curr_addr = next_addr;
    }

    for (iter = symtab->const_iterator_begin(); iter != symtab->const_iterator_end(); iter++) {
        symbol* constant = *iter;
        constant->set_address(curr_addr);
        opu_top->registerDeviceMemory(tc, curr_addr, constant->get_size_in_bytes());
        next_addr = curr_addr + constant->get_size_in_bytes();
        if (next_addr - base_addr > registering_allocation_size) {
            panic("Didn't allocate enough global+const memory. Bailing!");
        } else {
            DPRINTF(GPUSyscalls, "GPGPU-Sim PTX: Updated symbol \"%s\" to address range 0x%x to 0x%x\n", constant->name(), curr_addr, next_addr-1);
        }
        curr_addr = next_addr;
    }
}

void registerFatBinaryTop(GPUSyscallHelper *helper,
        symbol_table* symtab,
        unsigned int handle
        )
{
    OpuTop *opu_top = OpuTop::getOpuTop(g_active_device);
    // gpgpu_t *gpu = opu_top->getTheGPU();

    // registering_symtab = symtab;
    // registering_fat_cubin_handle = handle;

    // Read ident member
    // opu_top->add_binary(registering_symtab, registering_fat_cubin_handle);


}
#endif


#if 0
void
sysgem5gpu_symbol_lookup(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);

    symbol_table* symtab = *((symbol_table**)helper.getParam(0, true));
    Addr name = *((Addr*)helper.getParam(1, true));
    char* symbol_name = new char[MAX_STRING_LEN];
    helper.readString(name, (uint8_t*)symbol_name, MAX_STRING_LEN);

    symbol *s = symtab->lookup(symbol_name);

    // if (!opu_top->isManagingGPUMemory()) {
    helper.setReturn((uint8_t*)&s, sizeof(void*));
}

void
sysgem5gpu_symbol_get_function(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);

    symbol_table* symtab = *((symbol_table**)helper.getParam(0, true));
    Addr name = *((Addr*)helper.getParam(1, true));
    char* symbol_name = new char[MAX_STRING_LEN];
    helper.readString(name, (uint8_t*)symbol_name, MAX_STRING_LEN);

    symbol *s = symtab->lookup(symbol_name);
    function_info *f = NULL;
    if (s != NULL) {
        f = s->get_pc();
        assert(f != NULL);
    }

    // if (!opu_top->isManagingGPUMemory()) {
    helper.setReturn((uint8_t*)&f, sizeof(void*));
}
#endif

void
libgem5opuRegisterFatBinary(ThreadContext *tc, gpusyscall_t *call_params) {
    opu_not_implemented(__my_func__,__LINE__);
}

#if 0
{

    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);

    // Get opu call simulated parameters
    // Addr sim_fatCubin = *((Addr*)helper.getParam(0, true));
    // int sim_binSize = *((int*)helper.getParam(1));
    symbol_table* symtab = *((symbol_table**)helper.getParam(0, true));
    unsigned int handle = *((int*)helper.getParam(1));

    // DPRINTF(GPUSyscalls, "gem5 GPU Syscall: __opuRegisterFatBinary(fatCubin* = %x, binSize = %d)\n", sim_fatCubin, sim_binSize);
    OpuTop *opu_top = OpuTop::getOpuTop(g_active_device);

    assert(registering_symtab == NULL);
    registering_symtab = symtab;

    assert(registering_fat_cubin_handle == 0);
    registering_fat_cubin_handle = handle;

    // registerFatBinaryTop(&helper, sim_fatCubin, sim_binSize);
    registerFatBinaryTop(&helper, registering_symtab, registering_fat_cubin_handle);

    // FIXME now opurt load global and use opuMemcpy instead
    registering_allocation_size = get_global_and_constant_alloc_size(registering_symtab);

    // FIXME register_allocation_size will used return to gem5opuRegisterFatBinary
    //  and it call libopuRegisterFatBinaryFinalize, which will call
    //  registerDeviceMemory, the size 0x4000 is hardcode now
    registering_allocation_size += 0x4000;


    // FIXME i don't think it is need opu_top->saveFatBinaryInfoTop(tc->threadId(), registering_fat_cubin_handle, sim_fatCubin, sim_binSize);

    if (!opu_top->isManagingGPUMemory()) {
        helper.setReturn((uint8_t*)&registering_allocation_size, sizeof(int));
        // FIXME hack temp for inst text
        // opu_top->registerDeviceMemory(tc, 0xF0000000, 0x4000);
    } else {
        assert(!registering_allocation_ptr);
        registering_allocation_ptr = opu_top->allocateGPUMemory(registering_allocation_size);
        // FIXME
        // g_program_memory_start = registering_allocation_ptr;
        int zero_allocation = 0;
        helper.setReturn((uint8_t*)&zero_allocation, sizeof(int));
    }
}
#endif

#if 0
void
libgem5opuRegisterPtxInfo(ThreadContext *tc, gpusyscall_t *call_params)
{

    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);

    // Get opu call simulated parameters
    char* ptxinfo_kname = ((char*)helper.getParam(0, true));
    gpgpu_ptx_sim_info ptxinfo_kinfo = *((gpgpu_ptx_sim_info*)helper.getParam(1, true));

    // DPRINTF(GPUSyscalls, "gem5 GPU Syscall: __opuRegisterFatBinary(fatCubin* = %x, binSize = %d)\n", sim_fatCubin, sim_binSize);
    OpuTop *opu_top = OpuTop::getOpuTop(g_active_device);
    // gpgpu_ptxinfo_load_from_string(ptx, source_num);
    // TODO ptx_loader.cc ptxinfo_parse know ptxinfo_kname, ptxinfo_kinfo
    opu_top->add_ptxinfo(ptxinfo_kname, ptxinfo_kinfo);
}

unsigned int registerFatBinaryBottom(GPUSyscallHelper *helper, Addr sim_alloc_ptr)
{
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: __opuRegisterFatBinaryFinalize(alloc_ptr* = 0x%x)\n", sim_alloc_ptr);

    assert(registering_symtab);
    assert(registering_fat_cubin_handle > 0);
    assert(registering_allocation_size >= 0);
    assert(sim_alloc_ptr || registering_allocation_size == 0);

    if (registering_allocation_size > 0) {
        // FIXME , i think libopu runtime have dont it, so skip in here
        finalize_global_and_constant_setup(helper->getThreadContext(), sim_alloc_ptr, registering_symtab);
    }

    // FIXME , i think libopu runtime have dont it, so skip in here
    load_static_globals(helper, registering_symtab, STATIC_ALLOC_LIMIT);
    load_constants(helper, registering_symtab, STATIC_ALLOC_LIMIT);

    unsigned int handle = registering_fat_cubin_handle;

    registering_symtab = NULL;
    registering_fat_cubin_handle = 0;
    registering_allocation_size = -1;
    registering_allocation_ptr = 0;

    return handle;
}
#endif

void
libopuRegisterFatBinaryFinalize(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
#if 0
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);
    Addr sim_alloc_ptr = *((Addr*)helper.getParam(0, true));

    OpuTop *opu_top = OpuTop::getOpuTop(g_active_device);

    // opu_top->saveFatBinaryInfoBottom(sim_alloc_ptr);

    unsigned int handle;
    if (!opu_top->isManagingGPUMemory()) {
        // opu_top->saveFatBinaryInfoBottom(sim_alloc_ptr);
        handle = registerFatBinaryBottom(&helper, sim_alloc_ptr);
        // g_program_memory_start = sim_alloc_ptr;
        opu_top->registerDeviceMemory(tc, sim_alloc_ptr, 0x4000);
    } else {
        assert(!sim_alloc_ptr);
        assert(registering_allocation_ptr || registering_allocation_size == 0);
        // opu_top->saveFatBinaryInfoBottom(registering_allocation_ptr);
        handle = registerFatBinaryBottom(&helper, registering_allocation_ptr);
    }

    // TODO: If local memory has been allocated and has been mapped by the CPU
    // thread, register the allocation with the GPU for address translation.
    if (registering_local_alloc_ptr /*FIXME && !opu_top->getAccessHostPagetable()*/) {
        opu_top->registerDeviceMemory(tc, registering_local_alloc_ptr, get_local_alloc_size(opu_top));
    }

    helper.setReturn((uint8_t*)&handle, sizeof(void**), true);
#endif
}

void
__opuCheckAllocateLocal(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);

    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: __opuCheckAllocateLocal()\n");

    OpuTop *opu_top = OpuTop::getOpuTop(g_active_device);
#if 0
    assert(registering_symtab);
    if (registering_symtab->get_local_next() > 0 && (registering_local_alloc_ptr == NULL)) {
        unsigned long long local_alloc_size = get_local_alloc_size(opu_top);
        if (!opu_top->isManagingGPUMemory()) {
            DPRINTF(GPUSyscalls, "gem5 GPU Syscall:      CPU must allocate local: %lluB\n", local_alloc_size);
            helper.setReturn((uint8_t*)&local_alloc_size, sizeof(unsigned long long), false);
        } else {
            DPRINTF(GPUSyscalls, "gem5 GPU Syscall:      GPU allocating local...\n");
            registering_local_alloc_ptr = opu_top->allocateGPUMemory(local_alloc_size);
            opu_top->setLocalBaseVaddr(registering_local_alloc_ptr);
            opu_top->registerDeviceMemory(tc, registering_local_alloc_ptr, local_alloc_size);
            unsigned long long zero_allocation = 0;
            helper.setReturn((uint8_t*)&zero_allocation, sizeof(int));
        }
    }
#endif
}

void
__opuSetLocalAllocation(ThreadContext *tc, gpusyscall_t *call_params) {
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);
    Addr sim_alloc_ptr = *((Addr*)helper.getParam(0, true));

    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: __opuSetLocalAllocation(alloc_ptr* = 0x%x)\n", sim_alloc_ptr);

    OpuTop *opu_top = OpuTop::getOpuTop(g_active_device);

    // Save the local memory base address
    assert(!opu_top->isManagingGPUMemory());
    // assert(!registering_local_alloc_ptr);
    // registering_local_alloc_ptr = sim_alloc_ptr;
    // opu_top->setLocalBaseVaddr(registering_local_alloc_ptr);

    // TODO: Need to check if using host or GPU page mappings. If the GPU is
    // not able to access the host's pagetable, then the memory pages need to
    // be mapped for the GPU to access them.
    // global: bool registering_signal_local_map = false;
    // if (!opu_top->getAccessHostPagetable()) {
    //     registering_signal_local_map = true;
    //     helper.setReturn((uint8_t*)&signal_map_memory, sizeof(bool));
    // }
}

void
libopuUnregisterFatBinary(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
#if 0
    OpuTop *opu_top = OpuTop::getOpuTop(g_active_device);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: libopuUnregisterFatBinary() Faked\n");

    registering_local_alloc_ptr = 0;
#endif
}

void
libopuRegisterFunction(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
#if 0
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_fatCubinHandle = *((Addr*)helper.getParam(0, true));
    Addr sim_hostFun = *((Addr*)helper.getParam(1, true));
    Addr sim_deviceFun = *((Addr*)helper.getParam(2, true));
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: libopuRegisterFunction(fatCubinHandle** = %x, hostFun* = %x, deviceFun* = %x)\n",
            sim_fatCubinHandle, sim_hostFun, sim_deviceFun);

    // Read device function name from simulated system memory
    char* device_fun = new char[MAX_STRING_LEN];
    OpuTop *opu_top = OpuTop::getOpuTop(g_active_device);
    helper.readString(sim_deviceFun, (uint8_t*)device_fun, MAX_STRING_LEN);

    // Register function
    unsigned fat_cubin_handle = (unsigned)(unsigned long long)sim_fatCubinHandle;
    opu_top->register_function(fat_cubin_handle, (const char*)sim_hostFun, device_fun);
    // FIXME opu_top->saveFunctionNames(fat_cubin_handle, (const char*)sim_hostFun, device_fun);
    delete[] device_fun;
#endif
}

void register_var(OpuTop *opu_top, Addr sim_deviceAddress, const char* deviceName, int sim_size, int sim_constant, int sim_global, int sim_ext, Addr sim_hostVar)
{
    opu_not_implemented(__my_func__,__LINE__);
#if 0
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: __opuRegisterVar(fatCubinHandle** = %x, hostVar* = 0x%x, deviceAddress* = 0x%x, deviceName* = %s, ext = %d, size = %d, constant = %d, global = %d)\n",
            /*sim_fatCubinHandle*/ 0, sim_hostVar, sim_deviceAddress,
            deviceName, sim_ext, sim_size, sim_constant, sim_global);

    if (sim_constant && !sim_global && !sim_ext) {
        opu_top->getGPGPUCtx()->func_sim->gpgpu_ptx_sim_register_const_variable((void*)sim_hostVar, deviceName, sim_size);
    } else if (!sim_constant && !sim_global && !sim_ext) {
        opu_top->getGPGPUCtx()->func_sim->gpgpu_ptx_sim_register_global_variable((void*)sim_hostVar, deviceName, sim_size);
    } else {
        panic("__opuRegisterVar: Don't know how to register variable!");
    }
#endif
}

void libopuRegisterVar(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
#if 0
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);

    // Addr sim_fatCubinHandle = *((Addr*)helper.getParam(0, true));
    Addr sim_hostVar = *((Addr*)helper.getParam(1, true));
    Addr sim_deviceAddress = *((Addr*)helper.getParam(2, true));
    Addr sim_deviceName = *((Addr*)helper.getParam(3, true));
    int sim_ext = *((int*)helper.getParam(4));
    int sim_size = *((int*)helper.getParam(5));
    int sim_constant = *((int*)helper.getParam(6));
    int sim_global = *((int*)helper.getParam(7));

    const char* deviceName = new char[MAX_STRING_LEN];
    OpuTop *opu_top = OpuTop::getOpuTop(g_active_device);
    helper.readString(sim_deviceName, (uint8_t*)deviceName, MAX_STRING_LEN);

    opu_top->saveVar(sim_deviceAddress, deviceName, sim_size, sim_constant, sim_global, sim_ext, sim_hostVar);

    register_var(opu_top, sim_deviceAddress, deviceName, sim_size, sim_constant, sim_global, sim_ext, sim_hostVar);
#endif
}

//  void __opuRegisterShared(void **fatCubinHandle, void **devicePtr)
void
__opuRegisterShared(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

void
__opuRegisterSharedVar(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

void
libopuRegisterTexture(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
#if 0
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);

    // Addr sim_fatCubinHandle = *((Addr*)helper.getParam(0, true));
    Addr sim_hostVar = *((Addr*)helper.getParam(1));
    // Addr sim_deviceAddress = *((Addr*)helper.getParam(2, true));
    Addr sim_deviceName = *((Addr*)helper.getParam(3, true));
    int sim_dim = *((int*)helper.getParam(4));
    int sim_norm = *((int*)helper.getParam(5));
    int sim_ext = *((int*)helper.getParam(6));
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: __opuRegisterTexture(fatCubinHandle** = %x, hostVar* = %x, deviceAddress* = %x, deviceName* = %x, dim = %d, norm = %d, ext = %d)\n",
            /*sim_fatCubinHandle*/ 0, sim_hostVar, /*sim_deviceAddress*/ 0,
            sim_deviceName, sim_dim, sim_norm, sim_ext);

    const char* deviceName = new char[MAX_STRING_LEN];
    OpuTop *opu_top = OpuTop::getOpuTop(g_active_device);
    gpgpu_t *gpu = opu_top->getTheGPU();
    helper.readString(sim_deviceName, (uint8_t*)deviceName, MAX_STRING_LEN);

    gpu->gpgpu_ptx_sim_bindNameToTexture(deviceName, (const struct textureReference*)sim_hostVar, sim_dim, sim_norm, sim_ext);
    warn("__opuRegisterTexture implementation is not complete!");
#endif
}

void
opuGLRegisterBufferObject(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

void
opuGLMapBufferObject(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

void
opuGLUnmapBufferObject(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

void
opuGLUnregisterBufferObject(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

// #if (opuRT_VERSION >= 2010)

void
opuHostAlloc(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

void
opuHostGetDevicePointer(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

void
opuSetValidDevices(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

void
opuSetDeviceFlags(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

void
opuEventCreateWithFlags(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

void
opuDriverGetVersion(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

void
opuRuntimeGetVersion(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

// #endif

void
opuGLSetGLDevice(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

void
opuWGLGetDevice(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

void
__opuMutexOperation(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

void
__opuTextureFetch(ThreadContext *tc, gpusyscall_t *call_params)
{
    opu_not_implemented(__my_func__,__LINE__);
}

namespace opu_math {
    uint64_t __opuMutexOperation(ThreadContext *tc, gpusyscall_t *call_params)
    {
        opu_not_implemented(__my_func__,__LINE__);
        return 0;
    }

    uint64_t __opuTextureFetch(ThreadContext *tc, gpusyscall_t *call_params)
    {
        opu_not_implemented(__my_func__,__LINE__);
        return 0;
    }

    uint64_t __opuSynchronizeThreads(ThreadContext *tc, gpusyscall_t *call_params)
    {
        //TODO This function should syncronize if we support Asyn kernel calls
        return g_last_cudaError = cudaSuccess;
    }

    void  __opuTextureFetch(const void *tex, void *index, int integer, void *val) {
        opu_not_implemented(__my_func__,__LINE__);
    }

    void __opuMutexOperation(int lock)
    {
        opu_not_implemented(__my_func__,__LINE__);
    }
}

/// static functions

#if 0
static int load_static_globals(GPUSyscallHelper *helper, symbol_table *symtab, unsigned min_gaddr)
{
    opu_not_implemented(__my_func__,__LINE__);
    DPRINTF(GPUSyscalls, "GPGPU-Sim PTX: loading globals with explicit initializers\n");
    int ng_bytes = 0;
    return ng_bytes;
}

static int load_constants(GPUSyscallHelper *helper, symbol_table *symtab, addr_t min_gaddr)
{
   opu_not_implemented(__my_func__,__LINE__);
   DPRINTF(GPUSyscalls, "GPGPU-Sim PTX: loading constants with explicit initializers\n");
   int nc_bytes = 0;
   return nc_bytes;
}
void
sysgem5gpu_active(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    OpuTop::getOpuTop(g_active_device)->checkUpdateThreadContext(tc);

    OpuTop *opu_top = OpuTop::getOpuTop(g_active_device);

    Addr sim_active = *((Addr*)helper.getParam(0, true));

    int active = opu_top->is_active();
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: sysgem5gpu_active = %d\n", sim_active);

    helper.writeBlob(sim_active, (uint8_t*)(&active), sizeof(int));
    g_last_cudaError = cudaSuccess;
}

void
sysgem5gpu_system_call(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);

    Addr command = *((Addr*)helper.getParam(0, true));
    const char* command_string = new char[MAX_STRING_LEN];
    helper.readString(command, (uint8_t*)command_string, MAX_STRING_LEN);

    int result = 0;
    result = system(command_string);

    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: sysgem5gpu_system_call = %s, result=%d\n", command, result);

    helper.setReturn((uint8_t*)&result, sizeof(int));
    delete[] command_string;
}
#endif
