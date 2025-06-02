// This file created from cuda_runtime_api.h distributed with CUDA 1.1
// Changes Copyright 2009,  Tor M. Aamodt, Ali Bakhoda and George L. Yuan
// University of British Columbia

/*
 * cuda_syscalls.cc
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
 * NOTE: The files libcuda/cuda_runtime_api.c and src/cuda-sim/cuda-math.h
 * are derived from the CUDA Toolset available from http://www.nvidia.com/cuda
 * (property of NVIDIA).  The files benchmarks/BlackScholes/ and
 * benchmarks/template/ are derived from the CUDA SDK available from
 * http://www.nvidia.com/cuda (also property of NVIDIA).  The files from
 * src/intersim/ are derived from Booksim (a simulator provided with the
 * textbook "Principles and Practices of Interconnection Networks" available
 * from http://cva.stanford.edu/books/ppin/). As such, those files are bound by
 * the corresponding legal terms and conditions set forth separately (original
 * copyright notices are left in files from these sources and where we have
 * modified a file our copyright notice appears before the original copyright
 * notice).
 *
 * Using this version of GPGPU-Sim requires a complete installation of CUDA
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

#include "api/libcuda_syscalls.hh"
#include "api/gpu_syscall_helper.hh"
#include "cuda-sim/cuda-sim.h"
#include "cuda-sim/ptx_ir.h"
#include "cuda-sim/ptx_loader.h"
#include "cuda-sim/ptx_parser.h"
#include "debug/GPUSyscalls.hh"
#include "gpgpu-sim/gpu-sim.h"
#include "gpgpusim_entrypoint.h"
#include "gpu/gpgpu-sim/cuda_gpu.hh"
#include "stream_manager.h"
#include "../libcuda_sim/gpgpu_context.h"
#include "cpu/simple_thread.hh"
#include "arch/x86/vecregs.hh"

#define MAX_STRING_LEN 1000

typedef struct CUstream_st *cudaStream_t;

static int load_static_globals(GPUSyscallHelper *helper, symbol_table *symtab, unsigned min_gaddr);
static int load_constants(GPUSyscallHelper *helper, symbol_table *symtab, addr_t min_gaddr);

unsigned g_active_device = 0; // Active CUDA-enabled GPU that runs the code
cudaError_t g_last_cudaError = cudaSuccess;

using namespace gem5;
/*
class AppSystem {
public:
    AppSystem() {}
};
*/
#if 0
class AppThreadContext : public ThreadState, public ThreadContext {
public:
    /*
    using VecLane8 = VecLaneT<uint8_t, false>;
    using VecLane16 = VecLaneT<uint16_t, false>;
    using VecLane32 = VecLaneT<uint32_t, false>;
    using VecLane64 = VecLaneT<uint64_t, false>;

    using ConstVecLane8 = VecLaneT<uint8_t, true>;
    using ConstVecLane16 = VecLaneT<uint16_t, true>;
    using ConstVecLane32 = VecLaneT<uint32_t, true>;
    using ConstVecLane64 = VecLaneT<uint64_t, true>;
    */

    typedef ThreadContext::Status ThreadStatus;

    ::gem5::X86ISA::VecRegContainer vecReg;
    ::gem5::X86ISA::VecElem vecElem;
    ::gem5::X86ISA::VecPredRegContainer vecPredReg;
    ThreadStatus status_;
    gem5::System    *system_;
    AppThreadContext() : ThreadState(nullptr, 1, nullptr)
    {
        status_ = ThreadStatus();
        // system_ = new AppSystem();
    }
    BaseCPU *getCpuPtr() {return nullptr;};
    int cpuId() const {return 0;};
    uint32_t socketId() const {return 0;};
    int threadId() const {return 0;};
    void setThreadId(int id) {};
    ContextID contextId() const {return ContextID();};
    void setContextId(ContextID id) {};
    BaseMMU *getMMUPtr() {return nullptr;};
#ifndef BUILD_PPU
    CheckerCPU *getCheckerCpuPtr() {return nullptr;};
#endif
    BaseISA *getIsaPtr() {return nullptr;};
    ::gem5::X86ISA::Decoder *getDecoderPtr() {return nullptr;};
    System *getSystemPtr() {return nullptr;};
    PortProxy &getVirtProxy() {return ThreadState::getVirtProxy();};
    void initMemProxies(ThreadContext *tc) {};
    Process *getProcessPtr() {return nullptr;};
    void setProcessPtr(Process *p) {};
    ThreadStatus status() const {return status_;};
    void setStatus(ThreadStatus new_status) {};
    void activate() {};
    void suspend() {};
    void halt() {};
    void takeOverFrom(ThreadContext *old_context) {};
    void regStats(const std::string &name) {};
    void scheduleInstCountEvent(Event *event, Tick count) {};
    void descheduleInstCountEvent(Event *event) {};
    Tick getCurrentInstCount() {return Tick();};
    Tick readLastActivate() {return Tick();};
    Tick readLastSuspend() {return Tick();};
    void copyArchRegs(ThreadContext *tc) {};
    void clearArchRegs() {};
    RegVal readIntReg(RegIndex reg_idx) const {return RegVal();};
    RegVal readFloatReg(RegIndex reg_idx) const {return RegVal();};
    const ::gem5::X86ISA::VecRegContainer& readVecReg(const RegId& reg) const {return vecReg;};
    ::gem5::X86ISA::VecRegContainer& getWritableVecReg(const RegId& reg) {return vecReg;};
    const ::gem5::X86ISA::VecElem& readVecElem(const RegId& reg) const {return vecElem;};

    const ::gem5::X86ISA::VecPredRegContainer& readVecPredReg(
            const RegId& reg) const {return vecPredReg;};
    ::gem5::X86ISA::VecPredRegContainer& getWritableVecPredReg(
            const RegId& reg) {return vecPredReg;};

    RegVal readCCReg(RegIndex reg_idx) const {return RegVal();};
    void setIntReg(RegIndex reg_idx, RegVal val) {};
    void setFloatReg(RegIndex reg_idx, RegVal val) {};
    void setVecReg(const RegId& reg, const ::gem5::X86ISA::VecRegContainer& val) {};
    void setVecElem(const RegId& reg, const ::gem5::X86ISA::VecElem& val) {};
    void setVecPredReg(const RegId& reg,
            const ::gem5::X86ISA::VecPredRegContainer& val) {};
    void setCCReg(RegIndex reg_idx, RegVal val) {};
    ::gem5::X86ISA::PCState pcState() const {return ::gem5::X86ISA::PCState();};
    void pcState(const ::gem5::X86ISA::PCState &val) {};
    void pcStateNoRecord(const ::gem5::X86ISA::PCState &val) {};
    Addr instAddr() const {return Addr();};
    Addr nextInstAddr() const {return Addr();};
    MicroPC microPC() const {return MicroPC();};
    RegVal readMiscRegNoEffect(RegIndex misc_reg) const {return RegVal();};
    RegVal readMiscReg(RegIndex misc_reg) {return RegVal();};
    void setMiscRegNoEffect(RegIndex misc_reg, RegVal val) {};
    // TODO schi add from gem5-gpu
    void setMiscRegActuallyNoEffect(int misc_reg, const RegVal &val) {};
    void setMiscReg(RegIndex misc_reg, RegVal val) {};
    RegId flattenRegId(const RegId& reg_id) const {return RegId();};
    unsigned readStCondFailures() const {return 0;};
    void setStCondFailures(unsigned sc_failures) {};
    int exit() { return 1; };
    RegVal readIntRegFlat(RegIndex idx) const {return RegVal();};
    void setIntRegFlat(RegIndex idx, RegVal val) {};
    RegVal readFloatRegFlat(RegIndex idx) const {return RegVal();};
    void setFloatRegFlat(RegIndex idx, RegVal val) {};
    const ::gem5::X86ISA::VecRegContainer&
        readVecRegFlat(RegIndex idx) const {return vecReg;};
    ::gem5::X86ISA::VecRegContainer& getWritableVecRegFlat(RegIndex idx) {return vecReg;};
    void setVecRegFlat(RegIndex idx,
            const ::gem5::X86ISA::VecRegContainer& val) {};

    const ::gem5::X86ISA::VecElem& readVecElemFlat(RegIndex idx,
            const ElemIndex& elem_idx) const {return vecElem;};
    void setVecElemFlat(RegIndex idx, const ElemIndex& elem_idx,
            const ::gem5::X86ISA::VecElem& val) {};

    const ::gem5::X86ISA::VecPredRegContainer &
        readVecPredRegFlat(RegIndex idx) const {return vecPredReg;};
    ::gem5::X86ISA::VecPredRegContainer& getWritableVecPredRegFlat(
            RegIndex idx) {return vecPredReg;};
    void setVecPredRegFlat(RegIndex idx,
            const ::gem5::X86ISA::VecPredRegContainer& val) {};

    RegVal readCCRegFlat(RegIndex idx) const {return RegVal();};
    void setCCRegFlat(RegIndex idx, RegVal val) {};
    void htmAbortTransaction(uint64_t htm_uid,
                                     HtmFailureFaultCause cause) {};
    BaseHTMCheckpointPtr& getHtmCheckpointPtr() override {
        // return std::unique_ptr<BaseHTMCheckpoint>(new BaseHTMCheckpoint());
        panic("%s not implemented.", __FUNCTION__);
    };
    void setHtmCheckpointPtr(BaseHTMCheckpointPtr cpt) {};

    bool schedule(PCEvent *e) override { return true; }
    bool remove(PCEvent *e) override { return true; }
};

ThreadContext* create_thread_context() {
    static ThreadContext *app_context_outside_gem5 = nullptr;
    if (app_context_outside_gem5 == nullptr) {
        app_context_outside_gem5 = new ::AppThreadContext();
    }
    return app_context_outside_gem5;
}
#endif
extern ThreadContext* create_thread_context();

extern "C" void app_direct_call_gpu(uint64_t gpusysno, uint64_t call_params)
{
    ThreadContext *tc = create_thread_context();
    gpgpu_funcs[gpusysno](tc, (gpusyscall_t*)call_params);
}

void register_ptx_function(const char *name, function_info *impl)
{
   // TODO: Figure out the best location for this function
}

kernel_info_t *gpgpu_cuda_ptx_sim_init_grid(gpgpu_ptx_sim_arg_list_t args,
                                            struct dim3 gridDim,
                                            struct dim3 blockDim,
                                            function_info* entry)
{
   CudaGPU *cudaGPU = CudaGPU::getCudaGPU(g_active_device);
   gpgpu_t *gpu = cudaGPU->getTheGPU();

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

extern "C" void gem5_ptxinfo_addinfo()
{
    if (!strcmp("__cuda_dummy_entry__",get_ptxinfo_kname())) {
      // this string produced by ptxas for empty ptx files (e.g., bandwidth test)
        clear_ptxinfo();
        return;
    }
    CudaGPU *cudaGPU = CudaGPU::getCudaGPU(g_active_device);
    print_ptxinfo();
    cudaGPU->add_ptxinfo(get_ptxinfo_kname(), get_ptxinfo());
    clear_ptxinfo();
}

void cuda_not_implemented(const char* func, unsigned line)
{
    fflush(stdout);
    fflush(stderr);
    printf("\n\ngem5-gpu CUDA: Execution error: CUDA API function \"%s()\" has not been implemented yet.\n"
            "                 [gem5-gpu/src/gem5/%s around line %u]\n\n\n",
    func,__FILE__, line);
    fflush(stdout);
    abort();
}

typedef std::map<unsigned,CUevent_st*> event_tracker_t;

int CUevent_st::m_next_event_uid;
event_tracker_t g_timer_events;
std::list<kernel_config> g_cuda_launch_stack;

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

void
libcudaMalloc(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_devPtr = *((Addr*)helper.getParam(0, true));
    size_t sim_size = *((size_t*)helper.getParam(1));
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: cudaMalloc(devPtr = %x, size = %d)\n", sim_devPtr, sim_size);

    g_last_cudaError = cudaSuccess;

    CudaGPU *cudaGPU = CudaGPU::getCudaGPU(g_active_device);

    if (!cudaGPU->isManagingGPUMemory()) {
        // Tell CUDA runtime to allocate memory
        cudaError_t to_return = cudaErrorApiFailureBase;
        helper.setReturn((uint8_t*)&to_return, sizeof(cudaError_t));
        return;
    } else {
        Addr addr = cudaGPU->allocateGPUMemory(sim_size);
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
libcudaMallocHost(ThreadContext *tc, gpusyscall_t *call_params) {
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_ptr = *((Addr*)helper.getParam(0, true));
    size_t sim_size = *((size_t*)helper.getParam(1));
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: cudaMallocHost(ptr = %x, size = %d)\n", sim_ptr, sim_size);

    g_last_cudaError = cudaSuccess;
    // Tell CUDA runtime to allocate memory
    cudaError_t to_return = cudaErrorApiFailureBase;
    helper.setReturn((uint8_t*)&to_return, sizeof(cudaError_t));
}

void
libcudaRegisterDeviceMemory(ThreadContext *tc, gpusyscall_t *call_params)
{
    // This GPU syscall is used to initialize tracking of GPU memory so that
    // the GPU can do TLB lookups and if necessary, physical memory allocations
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_devicePtr = *((Addr*)helper.getParam(0, true));
    size_t sim_size = *((size_t*)helper.getParam(1));
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: cudaRegisterDeviceMemory(devicePtr = %x, size = %d)\n", sim_devicePtr, sim_size);

    CudaGPU *cudaGPU = CudaGPU::getCudaGPU(g_active_device);
    cudaGPU->registerDeviceMemory(tc, sim_devicePtr, sim_size);
}

void
cudaMallocPitch(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

//__host__ cudaError_t CUDARTAPI cudaMallocArray(struct cudaArray **array, const struct cudaChannelFormatDesc *desc, size_t width, size_t height __dv(1)) {
void
cudaMallocArray(ThreadContext *tc, gpusyscall_t *call_params) {
    cuda_not_implemented(__my_func__,__LINE__);
}

void
libcudaFree(ThreadContext *tc, gpusyscall_t *call_params) {
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_devPtr = *((Addr*)helper.getParam(0, true));
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: cudaFree(devPtr = %x)\n", sim_devPtr);

    CudaGPU *cudaGPU = CudaGPU::getCudaGPU(g_active_device);

    if (!cudaGPU->isManagingGPUMemory()) {
        g_last_cudaError = cudaSuccess;
        // Tell CUDA runtime to free memory
        cudaError_t to_return = cudaErrorApiFailureBase;
        helper.setReturn((uint8_t*)&to_return, sizeof(cudaError_t));
    } else {
        // TODO: Tell SPA to free this memory
        cudaError_t to_return = cudaSuccess;
        helper.setReturn((uint8_t*)&to_return, sizeof(cudaError_t));
    }
}

void
libcudaFreeHost(ThreadContext *tc, gpusyscall_t *call_params) {
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_ptr = *((Addr*)helper.getParam(0, true));
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: cudaFreeHost(ptr = %x)\n", sim_ptr);

    g_last_cudaError = cudaSuccess;
    // Tell CUDA runtime to free memory
    cudaError_t to_return = cudaErrorApiFailureBase;
    helper.setReturn((uint8_t*)&to_return, sizeof(cudaError_t));
}

//__host__ cudaError_t CUDARTAPI cudaFreeArray(struct cudaArray *array){
void
cudaFreeArray(ThreadContext *tc, gpusyscall_t *call_params) {
    cuda_not_implemented(__my_func__,__LINE__);
};


/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

void
libcudaMemcpy(ThreadContext *tc, gpusyscall_t *call_params) {
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_dst = *((Addr*)helper.getParam(0, true));
    Addr sim_src = *((Addr*)helper.getParam(1, true));
    size_t sim_count = *((size_t*)helper.getParam(2));
    enum cudaMemcpyKind sim_kind = *((enum cudaMemcpyKind*)helper.getParam(3));

    CudaGPU *cudaGPU = CudaGPU::getCudaGPU(g_active_device);

    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: cudaMemcpy(dst = %x, src = %x, count = %d, kind = %s)\n",
            sim_dst, sim_src, sim_count, cudaMemcpyKindStrings[sim_kind]);

    bool suspend = false;
    if (sim_count == 0) {
        g_last_cudaError = cudaSuccess;
        helper.setReturn((uint8_t*)&suspend, sizeof(bool));
        return;
    }

    if (sim_kind == cudaMemcpyHostToDevice) {
        stream_operation mem_op((const void*)sim_src, (size_t)sim_dst, sim_count, 0);
        mem_op.setThreadContext(tc);
        cudaGPU->getStreamManager()->push(mem_op);
    } else if (sim_kind == cudaMemcpyDeviceToHost) {
        stream_operation mem_op((size_t)sim_src, (void*)sim_dst, sim_count, 0);
        mem_op.setThreadContext(tc);
        cudaGPU->getStreamManager()->push(mem_op);
    } else if (sim_kind == cudaMemcpyDeviceToDevice) {
        stream_operation mem_op((size_t)sim_src, (size_t)sim_dst, sim_count, 0);
        mem_op.setThreadContext(tc);
        cudaGPU->getStreamManager()->push(mem_op);
    } else {
        panic("GPGPU-Sim PTX: cudaMemcpy - ERROR : unsupported cudaMemcpyKind\n");
    }

    suspend = cudaGPU->needsToBlock();
    assert(suspend);
    g_last_cudaError = cudaSuccess;
    helper.setReturn((uint8_t*)&suspend, sizeof(bool));
}

//__host__ cudaError_t CUDARTAPI cudaMemcpyToArray(struct cudaArray *dst, size_t wOffset, size_t hOffset, const void *src, size_t count, enum cudaMemcpyKind kind) {
void
cudaMemcpyToArray(ThreadContext *tc, gpusyscall_t *call_params) {
    cuda_not_implemented(__my_func__,__LINE__);
}

//__host__ cudaError_t CUDARTAPI cudaMemcpyFromArray(void *dst, const struct cudaArray *src, size_t wOffset, size_t hOffset, size_t count, enum cudaMemcpyKind kind) {
void
cudaMemcpyFromArray(ThreadContext *tc, gpusyscall_t *call_params) {
    cuda_not_implemented(__my_func__,__LINE__);
}

//__host__ cudaError_t CUDARTAPI cudaMemcpyArrayToArray(struct cudaArray *dst, size_t wOffsetDst, size_t hOffsetDst,
//                                                      const struct cudaArray *src, size_t wOffsetSrc, size_t hOffsetSrc,
//                                                      size_t count, enum cudaMemcpyKind kind __dv(cudaMemcpyDeviceToDevice)) {
void
cudaMemcpyArrayToArray(ThreadContext *tc, gpusyscall_t *call_params) {
    cuda_not_implemented(__my_func__,__LINE__);
}

//__host__ cudaError_t CUDARTAPI cudaMemcpy2D(void *dst, size_t dpitch, const void *src, size_t spitch, size_t width,
//                                            size_t height, enum cudaMemcpyKind kind) {
void
cudaMemcpy2D(ThreadContext *tc, gpusyscall_t *call_params) {
    cuda_not_implemented(__my_func__,__LINE__);
}

//__host__ cudaError_t CUDARTAPI cudaMemcpy2DToArray(struct cudaArray *dst, size_t wOffset, size_t hOffset, const void *src,
//                                                  size_t spitch, size_t width, size_t height, enum cudaMemcpyKind kind) {
void
cudaMemcpy2DToArray(ThreadContext *tc, gpusyscall_t *call_params) {
    cuda_not_implemented(__my_func__,__LINE__);
}

//__host__ cudaError_t CUDARTAPI cudaMemcpy2DFromArray(void *dst, size_t dpitch, const struct cudaArray *src, size_t wOffset,
//                                                  size_t hOffset, size_t width, size_t height, enum cudaMemcpyKind kind) {
void
cudaMemcpy2DFromArray(ThreadContext *tc, gpusyscall_t *call_params) {
    cuda_not_implemented(__my_func__,__LINE__);
}

//__host__ cudaError_t CUDARTAPI cudaMemcpy2DArrayToArray(struct cudaArray *dst, size_t wOffsetDst, size_t hOffsetDst,
//                                                      const struct cudaArray *src, size_t wOffsetSrc, size_t hOffsetSrc,
//                                                      size_t width, size_t height, enum cudaMemcpyKind kind __dv(cudaMemcpyDeviceToDevice)) {
void
cudaMemcpy2DArrayToArray(ThreadContext *tc, gpusyscall_t *call_params) {
    cuda_not_implemented(__my_func__,__LINE__);
}

void
libcudaMemcpyToSymbol(ThreadContext *tc, gpusyscall_t *call_params) {
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_symbol = *((Addr*)helper.getParam(0, true));
    Addr sim_src = *((Addr*)helper.getParam(1, true));
    size_t sim_count = *((size_t*)helper.getParam(2));
    size_t sim_offset = *((size_t*)helper.getParam(3));
    enum cudaMemcpyKind sim_kind = *((enum cudaMemcpyKind*)helper.getParam(4));

    CudaGPU *cudaGPU = CudaGPU::getCudaGPU(g_active_device);

    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: cudaMemcpyToSymbol(symbol = %x, src = %x, count = %d, offset = %d, kind = %s)\n",
            sim_symbol, sim_src, sim_count, sim_offset, cudaMemcpyKindStrings[sim_kind]);

    assert(sim_kind == cudaMemcpyHostToDevice);
    stream_operation mem_op((const void*)sim_src, (const char*)sim_symbol, sim_count, sim_offset, NULL);
    mem_op.setThreadContext(tc);
    cudaGPU->getStreamManager()->push(mem_op);

    bool suspend = cudaGPU->needsToBlock();
    assert(suspend);
    g_last_cudaError = cudaSuccess;
    helper.setReturn((uint8_t*)&suspend, sizeof(bool));
}

void
libcudaMemcpyFromSymbol(ThreadContext *tc, gpusyscall_t *call_params) {
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_dst = *((Addr*)helper.getParam(0, true));
    Addr sim_symbol = *((Addr*)helper.getParam(1, true));
    size_t sim_count = *((size_t*)helper.getParam(2));
    size_t sim_offset = *((size_t*)helper.getParam(3));
    enum cudaMemcpyKind sim_kind = *((enum cudaMemcpyKind*)helper.getParam(4));

    CudaGPU *cudaGPU = CudaGPU::getCudaGPU(g_active_device);

    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: cudaMemcpyToSymbol(symbol = %x, src = %x, count = %d, offset = %d, kind = %s)\n",
            sim_symbol, sim_dst, sim_count, sim_offset, cudaMemcpyKindStrings[sim_kind]);

    assert(sim_kind == cudaMemcpyDeviceToHost);
    stream_operation mem_op((const char*)sim_symbol, (void*)sim_dst, sim_count, sim_offset, NULL);
    mem_op.setThreadContext(tc);
    cudaGPU->getStreamManager()->push(mem_op);

    bool suspend = cudaGPU->needsToBlock();
    assert(suspend);
    g_last_cudaError = cudaSuccess;
    helper.setReturn((uint8_t*)&suspend, sizeof(bool));
}

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

//	__host__ cudaError_t CUDARTAPI cudaMemcpyAsync(void *dst, const void *src, size_t count, enum cudaMemcpyKind kind, cudaStream_t stream)
void
cudaMemcpyAsync(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

//	__host__ cudaError_t CUDARTAPI cudaMemcpyToArrayAsync(struct cudaArray *dst, size_t wOffset, size_t hOffset,
//	                                                        const void *src, size_t count, enum cudaMemcpyKind kind, cudaStream_t stream)
void
cudaMemcpyToArrayAsync(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

//	__host__ cudaError_t CUDARTAPI cudaMemcpyFromArrayAsync(void *dst, const struct cudaArray *src, size_t wOffset, size_t hOffset,
//	                                                        size_t count, enum cudaMemcpyKind kind, cudaStream_t stream)
void
cudaMemcpyFromArrayAsync(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

//	__host__ cudaError_t CUDARTAPI cudaMemcpy2DAsync(void *dst, size_t dpitch, const void *src, size_t spitch, size_t width,
//	                                                size_t height, enum cudaMemcpyKind kind, cudaStream_t stream)
void
cudaMemcpy2DAsync(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

void
cudaMemcpy2DToArrayAsync(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

void
cudaMemcpy2DFromArrayAsync(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

void
cudaBlockThread(ThreadContext *tc, gpusyscall_t *call_params)
{
    // Similar to futex in syscalls, except we need to track the variable to
    // be set
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);
    Addr sim_is_free_ptr = *((Addr*)helper.getParam(0, true));

    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: cudaBlockThread(tc = %x, is_free_ptr = %x)\n", tc, sim_is_free_ptr);

    CudaGPU *cudaGPU = CudaGPU::getCudaGPU(g_active_device);
    cudaGPU->blockThread(tc, sim_is_free_ptr);
}

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

void
libcudaMemset(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_mem = *((Addr*)helper.getParam(0, true));
    int sim_c = *((int*)helper.getParam(1));
    size_t sim_count = *((size_t*)helper.getParam(2));
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: cudaMemset(mem = %x, c = %d, count = %d)\n", sim_mem, sim_c, sim_count);

    CudaGPU *cudaGPU = CudaGPU::getCudaGPU(g_active_device);

    if (!cudaGPU->isManagingGPUMemory() && !cudaGPU->isAccessingHostPagetable()) {
        // Signal to libcuda that it should handle the memset. This is required
        // if the copy engine may be unable to access the CPU's pagetable to get
        // address translations (unified memory without access host pagetable)
        g_last_cudaError = cudaErrorApiFailureBase;
        helper.setReturn((uint8_t*)&g_last_cudaError, sizeof(cudaError_t));
    } else {
        stream_operation mem_op((size_t)sim_mem, sim_c, sim_count, 0);
        mem_op.setThreadContext(tc);
        cudaGPU->getStreamManager()->push(mem_op);
        g_last_cudaError = cudaSuccess;
        helper.setReturn((uint8_t*)&g_last_cudaError, sizeof(cudaError_t));

        bool suspend = cudaGPU->needsToBlock();
        assert(suspend);
    }
}

void
cudaMemset2D(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

void
cudaGetSymbolAddress(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

void
cudaGetSymbolSize(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

void
libcudaGetDeviceCount(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);
    Addr sim_count = *((Addr*)helper.getParam(0, true));

    int count = CudaGPU::getNumCudaDevices();
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: cudaGetDeviceCount(count* = %x) = %d\n", sim_count, count);

    helper.writeBlob(sim_count, (uint8_t*)(&count), sizeof(int));
    g_last_cudaError = cudaSuccess;
}

void
libcudaGetDeviceProperties(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_prop = *((Addr*)helper.getParam(0, true));
    int sim_device = *((int*)helper.getParam(1));
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: cudaGetDeviceProperties(prop* = %x, device = %d)\n", sim_prop, sim_device);
    CudaGPU *cudaGPU = CudaGPU::getCudaGPU(g_active_device);
    if (sim_device <= CudaGPU::getNumCudaDevices())  {
        CudaGPU::CudaDeviceProperties *prop = cudaGPU->getDeviceProperties();
        helper.writeBlob(sim_prop, (uint8_t*)(prop), sizeof(CudaGPU::CudaDeviceProperties));
        g_last_cudaError = cudaSuccess;
    } else {
        g_last_cudaError = cudaErrorInvalidDevice;
    }
    helper.setReturn((uint8_t*)&g_last_cudaError, sizeof(cudaError_t));
}

void
cudaChooseDevice(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

void
libcudaSetDevice(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);

    int sim_device = *((int*)helper.getParam(0));
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: cudaSetDevice(device = %d)\n", sim_device);
    if (sim_device <= CudaGPU::getNumCudaDevices()) {
        g_active_device = sim_device;
        g_last_cudaError = cudaSuccess;
    } else {
        g_last_cudaError = cudaErrorInvalidDevice;
    }
    helper.setReturn((uint8_t*)&g_last_cudaError, sizeof(cudaError_t));
}

void
libcudaGetDevice(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_device = *((Addr*)helper.getParam(0, true));
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: cudaGetDevice(device = 0x%x)\n", sim_device);
    if (g_active_device <= CudaGPU::getNumCudaDevices()) {
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

// __host__ cudaError_t CUDARTAPI cudaBindTexture(size_t *offset, const struct textureReference *texref, const void *devPtr,
//                                                      const struct cudaChannelFormatDesc *desc, size_t size __dv(UINT_MAX))
void
cudaBindTexture(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    cuda_not_implemented(__my_func__,__LINE__);
}

void
cudaBindTextureToArray(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

void
cudaUnbindTexture(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

void
cudaGetTextureAlignmentOffset(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

void
cudaGetTextureReference(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

/*******************************************************************************
 *                                                                              *
 *                                                                              *
 *                                                                              *
*******************************************************************************/

void
cudaGetChannelDesc(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

void
cudaCreateChannelDesc(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

/*******************************************************************************
 *                                                                              *
 *                                                                              *
 *                                                                              *
*******************************************************************************/

void
cudaGetLastError(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: cudaGetLastError()\n");
    helper.setReturn((uint8_t*)&g_last_cudaError, sizeof(cudaError_t));
}

void
cudaGetErrorString(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

/*******************************************************************************
 *                                                                              *
 *                                                                              *
 *                                                                              *
*******************************************************************************/
void libgem5cudaConfigureCall(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);

    dim3 sim_gridDim = *((dim3*)helper.getParam(0));
    dim3 sim_blockDim = *((dim3*)helper.getParam(1));
    size_t sim_sharedMem = *((size_t*)helper.getParam(2));
    cudaStream_t sim_stream = *((cudaStream_t*)helper.getParam(3));
    if (sim_stream) {
        panic("gem5-fusion doesn't currently support CUDA streams");
    }
    assert(!sim_stream); // We do not currently support CUDA streams
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: cudaConfigureCall(tc = %p, gridDim = (%u,%u,%u), blockDim = (%u,%u,%u), sharedMem = %u, stream)\n",
            tc, sim_gridDim.x, sim_gridDim.y, sim_gridDim.z, sim_blockDim.x,
            sim_blockDim.y, sim_blockDim.z, sim_sharedMem);

    g_cuda_launch_stack.push_back(kernel_config(sim_gridDim, sim_blockDim, sim_sharedMem, sim_stream));
    g_last_cudaError = cudaSuccess;
}

// void gem5cudaSetupArgument_(ThreadContext *tc, gpusyscall_t *call_params){
void libgem5cudaSetupArgument(ThreadContext *tc, gpusyscall_t *call_params){
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_arg = *((Addr*)helper.getParam(0, true));
    size_t sim_size = *((size_t*)helper.getParam(1));
    size_t sim_offset = *((size_t*)helper.getParam(2));

    const void* arg = new uint8_t[sim_size];
    helper.readBlob(sim_arg, (uint8_t*)arg, sim_size);
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: cudaSetupArgument(tc = %p, arg = %x, size = %d, offset = %d) *arg = %p\n",
            tc, sim_arg, sim_size, sim_offset, arg);

    assert(!g_cuda_launch_stack.empty());
    kernel_config &config = g_cuda_launch_stack.back();
    config.set_arg(arg, sim_size, sim_offset);

    g_last_cudaError = cudaSuccess;
}

/*
void gem5cudaSetupArgument_(GPUSyscallHelper &helper, Addr sim_arg, size_t sim_size, size_t sim_offset)
{
    const void* arg = new uint8_t[sim_size];
    helper.readBlob(sim_arg, (uint8_t*)arg, sim_size);
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: cudaSetupArgument(arg = %x, size = %d, offset = %d) *arg = %p\n",
            sim_arg, sim_size, sim_offset, arg);

    assert(!g_cuda_launch_stack.empty());
    kernel_config &config = g_cuda_launch_stack.back();
    config.set_arg(arg, sim_size, sim_offset);
}


void libgem5cudaSetupArgument(ThreadContext *tc, gpusyscall_t *call_params){
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);

    function_info* f = *((function_info**)helper.getParam(0, true));
    Addr sim_arg = *((Addr*)helper.getParam(1, true));
    unsigned num_args = f->num_args();

    Addr* args = new Addr[num_args];
    helper.readBlob(sim_arg, (uint8_t*)args, num_args * sizeof(void*));

    for(unsigned i = 0; i < num_args; i++){
       	std::pair<size_t, unsigned> p = f->get_param_config(i);
       	gem5cudaSetupArgument_(helper, (Addr)(args[i]), p.first, p.second);
    }

    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: cudaSetupArguments(tc = %p, arg = %x) *arg = %p\n",
            tc, sim_arg);

    g_last_cudaError = cudaSuccess;
}

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



void libgem5cudaLaunch(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_hostFun = *((Addr*)helper.getParam(0, true));

    CudaGPU *cudaGPU = CudaGPU::getCudaGPU(g_active_device);
    char *mode = getenv("PTX_SIM_MODE_FUNC");
    int ptx_sim_mode;
    if (mode)
        sscanf(mode,"%u", &ptx_sim_mode);
    assert(!g_cuda_launch_stack.empty());
    kernel_config config = g_cuda_launch_stack.back();
    struct CUstream_st *stream = config.get_stream();
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: cudaLaunch(tc = %p, hostFun* = %x)\n", tc, sim_hostFun);
    kernel_info_t *grid = gpgpu_cuda_ptx_sim_init_grid(config.get_args(), config.grid_dim(), config.block_dim(), cudaGPU->get_kernel((const char*)sim_hostFun));
    grid->set_inst_base_vaddr(cudaGPU->getInstBaseVaddr());
    std::string kname = grid->name();
    stream_operation op(grid, ptx_sim_mode, stream);
    op.setThreadContext(tc);
    cudaGPU->getStreamManager()->push(op);
    g_cuda_launch_stack.pop_back();
    g_last_cudaError = cudaSuccess;
}

size_t getMaxThreadsPerBlock(struct cudaFuncAttributes attr) {
    CudaGPU *cudaGPU = CudaGPU::getCudaGPU(g_active_device);
    CudaGPU::CudaDeviceProperties *prop;

    prop = cudaGPU->getDeviceProperties();

    size_t max = prop->maxThreadsPerBlock;

    if ((prop->regsPerBlock / attr.numRegs) < max) {
        max = prop->regsPerBlock / attr.numRegs;
    }

    return max;
}

void
libcudaFuncGetAttributes(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_attr = *((Addr*)helper.getParam(0, true));
    Addr sim_hostFun = *((Addr*)helper.getParam(1, true));

    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: cudaFuncGetAttributes(attr* = %x, hostFun* = %x)\n", sim_attr, sim_hostFun);

    CudaGPU *cudaGPU = CudaGPU::getCudaGPU(g_active_device);
    function_info *entry = cudaGPU->get_kernel((const char*)sim_hostFun);

    if (entry) {
        const struct gpgpu_ptx_sim_info *kinfo = entry->get_kernel_info();
        cudaFuncAttributes attr;
        attr.sharedSizeBytes = kinfo->smem;
        attr.constSizeBytes  = kinfo->cmem;
        attr.localSizeBytes  = kinfo->lmem;
        attr.numRegs         = kinfo->regs;
        attr.maxThreadsPerBlock = getMaxThreadsPerBlock(attr);
        attr.ptxVersion      = kinfo->ptx_version;
        attr.binaryVersion   = kinfo->sm_target;
        helper.writeBlob(sim_attr, (uint8_t*)&attr, sizeof(cudaFuncAttributes));
        g_last_cudaError = cudaSuccess;
    } else {
        g_last_cudaError = cudaErrorInvalidDeviceFunction;
    }

    helper.setReturn((uint8_t*)&g_last_cudaError, sizeof(cudaError_t));
}

/*******************************************************************************
 *                                                                              *
 *                                                                              *
 *                                                                              *
*******************************************************************************/

// __host__ cudaError_t CUDARTAPI cudaStreamCreate(cudaStream_t *stream)
void
cudaStreamCreate(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

// __host__ cudaError_t CUDARTAPI cudaStreamDestroy(cudaStream_t stream)
void
cudaStreamDestroy(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

// __host__ cudaError_t CUDARTAPI cudaStreamSynchronize(cudaStream_t stream)
void
cudaStreamSynchronize(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

// __host__ cudaError_t CUDARTAPI cudaStreamQuery(cudaStream_t stream)
void
cudaStreamQuery(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

/*******************************************************************************
 *                                                                              *
 *                                                                              *
 *                                                                              *
*******************************************************************************/

// __host__ cudaError_t CUDARTAPI cudaEventCreate(cudaEvent_t *event)
void
cudaEventCreate(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

void
cudaEventRecord(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

void
cudaEventQuery(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

void
cudaEventSynchronize(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

void
cudaEventDestroy(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

void
cudaEventElapsedTime(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

/*******************************************************************************
 *                                                                              *
 *                                                                              *
 *                                                                              *
*******************************************************************************/

void
cudaThreadExit(ThreadContext *tc, gpusyscall_t *call_params)
{
    // This function should clean-up any/all resources associated with the
    // current device in the passed thread context
    cuda_not_implemented(__my_func__,__LINE__);
}

void
cudaThreadSynchronize(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: cudaThreadSynchronize(), tc = %x\n", tc);
    CudaGPU *cudaGPU = CudaGPU::getCudaGPU(g_active_device);
    bool suspend = cudaGPU->needsToBlock();
    g_last_cudaError = cudaSuccess;
    helper.setReturn((uint8_t*)&suspend, sizeof(bool));
}

void
__cudaSynchronizeThreads(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

void
deleteFatCudaBinary(__cudaFatCudaBinary* fat_cubin) {
    if (fat_cubin->ident) delete[] fat_cubin->ident;
    if (fat_cubin->ptx) {
        // @TODO: This might need to loop... consider splitting out the
        // CUDA binary read into a separate helper class that tracks the
        // number of ptx_entries that are read in
        if (fat_cubin->ptx->gpuProfileName) delete[] fat_cubin->ptx->gpuProfileName;
        if (fat_cubin->ptx->ptx) delete[] fat_cubin->ptx->ptx;
        delete[] fat_cubin->ptx;
    }
    delete fat_cubin;
}

symbol_table* registering_symtab = NULL;
unsigned registering_fat_cubin_handle = 0;
int registering_allocation_size = -1;
Addr registering_allocation_ptr = 0;
Addr registering_local_alloc_ptr = 0;

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
get_local_alloc_size(CudaGPU *cudaGPU) {
    unsigned cores = cudaGPU->getDeviceProperties()->multiProcessorCount;
    unsigned threads_per_core = cudaGPU->getMaxThreadsPerMultiprocessor();
    // NOTE: Per technical specs Wikipedia: http://en.wikipedia.org/wiki/CUDA
    // For CUDA GPUs with compute capability 1.x, each thread should be able to
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

void
finalize_global_and_constant_setup(ThreadContext *tc, Addr base_addr, symbol_table* symtab)
{
    CudaGPU *cudaGPU = CudaGPU::getCudaGPU(g_active_device);
    Addr curr_addr = base_addr;
    Addr next_addr = 0;
    symbol_table::iterator iter;
    for (iter = symtab->global_iterator_begin(); iter != symtab->global_iterator_end(); iter++) {
        symbol* global = *iter;
        global->set_address(curr_addr);
        cudaGPU->registerDeviceMemory(tc, curr_addr, global->get_size_in_bytes());
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
        cudaGPU->registerDeviceMemory(tc, curr_addr, constant->get_size_in_bytes());
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
    CudaGPU *cudaGPU = CudaGPU::getCudaGPU(g_active_device);
    gpgpu_t *gpu = cudaGPU->getTheGPU();

    registering_symtab = symtab;
    registering_fat_cubin_handle = handle;

    // Read ident member
    cudaGPU->add_binary(registering_symtab, registering_fat_cubin_handle);


}


#if 0
void
sysgem5gpu_symbol_lookup(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);

    symbol_table* symtab = *((symbol_table**)helper.getParam(0, true));
    Addr name = *((Addr*)helper.getParam(1, true));
    char* symbol_name = new char[MAX_STRING_LEN];
    helper.readString(name, (uint8_t*)symbol_name, MAX_STRING_LEN);

    symbol *s = symtab->lookup(symbol_name);

    // if (!cudaGPU->isManagingGPUMemory()) {
    helper.setReturn((uint8_t*)&s, sizeof(void*));
}

void
sysgem5gpu_symbol_get_function(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);

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

    // if (!cudaGPU->isManagingGPUMemory()) {
    helper.setReturn((uint8_t*)&f, sizeof(void*));
}
#endif

void
libgem5cudaRegisterFatBinary(ThreadContext *tc, gpusyscall_t *call_params)
{

    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);

    // Get CUDA call simulated parameters
    // Addr sim_fatCubin = *((Addr*)helper.getParam(0, true));
    // int sim_binSize = *((int*)helper.getParam(1));
    symbol_table* symtab = *((symbol_table**)helper.getParam(0, true));
    unsigned int handle = *((int*)helper.getParam(1));

    // DPRINTF(GPUSyscalls, "gem5 GPU Syscall: __cudaRegisterFatBinary(fatCubin* = %x, binSize = %d)\n", sim_fatCubin, sim_binSize);
    CudaGPU *cudaGPU = CudaGPU::getCudaGPU(g_active_device);

    assert(registering_symtab == NULL);
    registering_symtab = symtab;

    assert(registering_fat_cubin_handle == 0);
    registering_fat_cubin_handle = handle;

    // registerFatBinaryTop(&helper, sim_fatCubin, sim_binSize);
    registerFatBinaryTop(&helper, registering_symtab, registering_fat_cubin_handle);

    // FIXME now cudart load global and use cudaMemcpy instead
    registering_allocation_size = get_global_and_constant_alloc_size(registering_symtab);

    // FIXME register_allocation_size will used return to gem5cudaRegisterFatBinary
    //  and it call libcudaRegisterFatBinaryFinalize, which will call
    //  registerDeviceMemory, the size 0x4000 is hardcode now
    registering_allocation_size += 0x4000;


    // FIXME i don't think it is need cudaGPU->saveFatBinaryInfoTop(tc->threadId(), registering_fat_cubin_handle, sim_fatCubin, sim_binSize);

    if (!cudaGPU->isManagingGPUMemory()) {
        helper.setReturn((uint8_t*)&registering_allocation_size, sizeof(int));
        // FIXME hack temp for inst text
        // cudaGPU->registerDeviceMemory(tc, 0xF0000000, 0x4000);
    } else {
        assert(!registering_allocation_ptr);
        registering_allocation_ptr = cudaGPU->allocateGPUMemory(registering_allocation_size);
        // FIXME
        // g_program_memory_start = registering_allocation_ptr;
        int zero_allocation = 0;
        helper.setReturn((uint8_t*)&zero_allocation, sizeof(int));
    }
}

#if 0
void
libgem5cudaRegisterPtxInfo(ThreadContext *tc, gpusyscall_t *call_params)
{

    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);

    // Get CUDA call simulated parameters
    char* ptxinfo_kname = ((char*)helper.getParam(0, true));
    gpgpu_ptx_sim_info ptxinfo_kinfo = *((gpgpu_ptx_sim_info*)helper.getParam(1, true));

    // DPRINTF(GPUSyscalls, "gem5 GPU Syscall: __cudaRegisterFatBinary(fatCubin* = %x, binSize = %d)\n", sim_fatCubin, sim_binSize);
    CudaGPU *cudaGPU = CudaGPU::getCudaGPU(g_active_device);
    // gpgpu_ptxinfo_load_from_string(ptx, source_num);
    // TODO ptx_loader.cc ptxinfo_parse know ptxinfo_kname, ptxinfo_kinfo
    cudaGPU->add_ptxinfo(ptxinfo_kname, ptxinfo_kinfo);
}
#endif

unsigned int registerFatBinaryBottom(GPUSyscallHelper *helper, Addr sim_alloc_ptr)
{
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: __cudaRegisterFatBinaryFinalize(alloc_ptr* = 0x%x)\n", sim_alloc_ptr);

    assert(registering_symtab);
    assert(registering_fat_cubin_handle > 0);
    assert(registering_allocation_size >= 0);
    assert(sim_alloc_ptr || registering_allocation_size == 0);

    if (registering_allocation_size > 0) {
        // FIXME , i think libcuda runtime have dont it, so skip in here
        finalize_global_and_constant_setup(helper->getThreadContext(), sim_alloc_ptr, registering_symtab);
    }

    // FIXME , i think libcuda runtime have dont it, so skip in here
    load_static_globals(helper, registering_symtab, STATIC_ALLOC_LIMIT);
    load_constants(helper, registering_symtab, STATIC_ALLOC_LIMIT);

    unsigned int handle = registering_fat_cubin_handle;

    registering_symtab = NULL;
    registering_fat_cubin_handle = 0;
    registering_allocation_size = -1;
    registering_allocation_ptr = 0;

    return handle;
}

void
libcudaRegisterFatBinaryFinalize(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);
    Addr sim_alloc_ptr = *((Addr*)helper.getParam(0, true));

    CudaGPU *cudaGPU = CudaGPU::getCudaGPU(g_active_device);

    // cudaGPU->saveFatBinaryInfoBottom(sim_alloc_ptr);

    unsigned int handle;
    if (!cudaGPU->isManagingGPUMemory()) {
        // cudaGPU->saveFatBinaryInfoBottom(sim_alloc_ptr);
        handle = registerFatBinaryBottom(&helper, sim_alloc_ptr);
        // g_program_memory_start = sim_alloc_ptr;
        cudaGPU->registerDeviceMemory(tc, sim_alloc_ptr, 0x4000);
    } else {
        assert(!sim_alloc_ptr);
        assert(registering_allocation_ptr || registering_allocation_size == 0);
        // cudaGPU->saveFatBinaryInfoBottom(registering_allocation_ptr);
        handle = registerFatBinaryBottom(&helper, registering_allocation_ptr);
    }

    // TODO: If local memory has been allocated and has been mapped by the CPU
    // thread, register the allocation with the GPU for address translation.
    if (registering_local_alloc_ptr /*FIXME && !cudaGPU->getAccessHostPagetable()*/) {
        cudaGPU->registerDeviceMemory(tc, registering_local_alloc_ptr, get_local_alloc_size(cudaGPU));
    }

    helper.setReturn((uint8_t*)&handle, sizeof(void**), true);
}

void
__cudaCheckAllocateLocal(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);

    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: __cudaCheckAllocateLocal()\n");

    CudaGPU *cudaGPU = CudaGPU::getCudaGPU(g_active_device);

    assert(registering_symtab);
    if (registering_symtab->get_local_next() > 0 && (registering_local_alloc_ptr == NULL)) {
        unsigned long long local_alloc_size = get_local_alloc_size(cudaGPU);
        if (!cudaGPU->isManagingGPUMemory()) {
            DPRINTF(GPUSyscalls, "gem5 GPU Syscall:      CPU must allocate local: %lluB\n", local_alloc_size);
            helper.setReturn((uint8_t*)&local_alloc_size, sizeof(unsigned long long), false);
        } else {
            DPRINTF(GPUSyscalls, "gem5 GPU Syscall:      GPU allocating local...\n");
            registering_local_alloc_ptr = cudaGPU->allocateGPUMemory(local_alloc_size);
            cudaGPU->setLocalBaseVaddr(registering_local_alloc_ptr);
            cudaGPU->registerDeviceMemory(tc, registering_local_alloc_ptr, local_alloc_size);
            unsigned long long zero_allocation = 0;
            helper.setReturn((uint8_t*)&zero_allocation, sizeof(int));
        }
    }
}

void
__cudaSetLocalAllocation(ThreadContext *tc, gpusyscall_t *call_params) {
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);
    Addr sim_alloc_ptr = *((Addr*)helper.getParam(0, true));

    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: __cudaSetLocalAllocation(alloc_ptr* = 0x%x)\n", sim_alloc_ptr);

    CudaGPU *cudaGPU = CudaGPU::getCudaGPU(g_active_device);

    // Save the local memory base address
    assert(!cudaGPU->isManagingGPUMemory());
    assert(!registering_local_alloc_ptr);
    registering_local_alloc_ptr = sim_alloc_ptr;
    cudaGPU->setLocalBaseVaddr(registering_local_alloc_ptr);

    // TODO: Need to check if using host or GPU page mappings. If the GPU is
    // not able to access the host's pagetable, then the memory pages need to
    // be mapped for the GPU to access them.
    // global: bool registering_signal_local_map = false;
    // if (!cudaGPU->getAccessHostPagetable()) {
    //     registering_signal_local_map = true;
    //     helper.setReturn((uint8_t*)&signal_map_memory, sizeof(bool));
    // }
}

void
libcudaUnregisterFatBinary(ThreadContext *tc, gpusyscall_t *call_params)
{
    CudaGPU *cudaGPU = CudaGPU::getCudaGPU(g_active_device);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: libcudaUnregisterFatBinary() Faked\n");

    registering_local_alloc_ptr = 0;
}

void
libcudaRegisterFunction(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);

    Addr sim_fatCubinHandle = *((Addr*)helper.getParam(0, true));
    Addr sim_hostFun = *((Addr*)helper.getParam(1, true));
    Addr sim_deviceFun = *((Addr*)helper.getParam(2, true));
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: libcudaRegisterFunction(fatCubinHandle** = %x, hostFun* = %x, deviceFun* = %x)\n",
            sim_fatCubinHandle, sim_hostFun, sim_deviceFun);

    // Read device function name from simulated system memory
    char* device_fun = new char[MAX_STRING_LEN];
    CudaGPU *cudaGPU = CudaGPU::getCudaGPU(g_active_device);
    helper.readString(sim_deviceFun, (uint8_t*)device_fun, MAX_STRING_LEN);

    // Register function
    unsigned fat_cubin_handle = (unsigned)(unsigned long long)sim_fatCubinHandle;
    cudaGPU->register_function(fat_cubin_handle, (const char*)sim_hostFun, device_fun);
    // FIXME cudaGPU->saveFunctionNames(fat_cubin_handle, (const char*)sim_hostFun, device_fun);
    delete[] device_fun;
}

void register_var(CudaGPU *cudaGPU, Addr sim_deviceAddress, const char* deviceName, int sim_size, int sim_constant, int sim_global, int sim_ext, Addr sim_hostVar)
{
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: __cudaRegisterVar(fatCubinHandle** = %x, hostVar* = 0x%x, deviceAddress* = 0x%x, deviceName* = %s, ext = %d, size = %d, constant = %d, global = %d)\n",
            /*sim_fatCubinHandle*/ 0, sim_hostVar, sim_deviceAddress,
            deviceName, sim_ext, sim_size, sim_constant, sim_global);

    if (sim_constant && !sim_global && !sim_ext) {
        cudaGPU->getGPGPUCtx()->func_sim->gpgpu_ptx_sim_register_const_variable((void*)sim_hostVar, deviceName, sim_size);
    } else if (!sim_constant && !sim_global && !sim_ext) {
        cudaGPU->getGPGPUCtx()->func_sim->gpgpu_ptx_sim_register_global_variable((void*)sim_hostVar, deviceName, sim_size);
    } else {
        panic("__cudaRegisterVar: Don't know how to register variable!");
    }
}

void libcudaRegisterVar(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);

    // Addr sim_fatCubinHandle = *((Addr*)helper.getParam(0, true));
    Addr sim_hostVar = *((Addr*)helper.getParam(1, true));
    Addr sim_deviceAddress = *((Addr*)helper.getParam(2, true));
    Addr sim_deviceName = *((Addr*)helper.getParam(3, true));
    int sim_ext = *((int*)helper.getParam(4));
    int sim_size = *((int*)helper.getParam(5));
    int sim_constant = *((int*)helper.getParam(6));
    int sim_global = *((int*)helper.getParam(7));

    const char* deviceName = new char[MAX_STRING_LEN];
    CudaGPU *cudaGPU = CudaGPU::getCudaGPU(g_active_device);
    helper.readString(sim_deviceName, (uint8_t*)deviceName, MAX_STRING_LEN);

    cudaGPU->saveVar(sim_deviceAddress, deviceName, sim_size, sim_constant, sim_global, sim_ext, sim_hostVar);

    register_var(cudaGPU, sim_deviceAddress, deviceName, sim_size, sim_constant, sim_global, sim_ext, sim_hostVar);
}

//  void __cudaRegisterShared(void **fatCubinHandle, void **devicePtr)
void
__cudaRegisterShared(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

void
__cudaRegisterSharedVar(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

void
libcudaRegisterTexture(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);

    // Addr sim_fatCubinHandle = *((Addr*)helper.getParam(0, true));
    Addr sim_hostVar = *((Addr*)helper.getParam(1));
    // Addr sim_deviceAddress = *((Addr*)helper.getParam(2, true));
    Addr sim_deviceName = *((Addr*)helper.getParam(3, true));
    int sim_dim = *((int*)helper.getParam(4));
    int sim_norm = *((int*)helper.getParam(5));
    int sim_ext = *((int*)helper.getParam(6));
    DPRINTF(GPUSyscalls, "gem5 GPU Syscall: __cudaRegisterTexture(fatCubinHandle** = %x, hostVar* = %x, deviceAddress* = %x, deviceName* = %x, dim = %d, norm = %d, ext = %d)\n",
            /*sim_fatCubinHandle*/ 0, sim_hostVar, /*sim_deviceAddress*/ 0,
            sim_deviceName, sim_dim, sim_norm, sim_ext);

    const char* deviceName = new char[MAX_STRING_LEN];
    CudaGPU *cudaGPU = CudaGPU::getCudaGPU(g_active_device);
    gpgpu_t *gpu = cudaGPU->getTheGPU();
    helper.readString(sim_deviceName, (uint8_t*)deviceName, MAX_STRING_LEN);

    gpu->gpgpu_ptx_sim_bindNameToTexture(deviceName, (const struct textureReference*)sim_hostVar, sim_dim, sim_norm, sim_ext);
    warn("__cudaRegisterTexture implementation is not complete!");
}

void
cudaGLRegisterBufferObject(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

void
cudaGLMapBufferObject(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

void
cudaGLUnmapBufferObject(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

void
cudaGLUnregisterBufferObject(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

#if (CUDART_VERSION >= 2010)

void
cudaHostAlloc(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

void
cudaHostGetDevicePointer(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

void
cudaSetValidDevices(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

void
cudaSetDeviceFlags(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

void
cudaEventCreateWithFlags(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

void
cudaDriverGetVersion(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

void
cudaRuntimeGetVersion(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

#endif

void
cudaGLSetGLDevice(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

void
cudaWGLGetDevice(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

void
__cudaMutexOperation(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

void
__cudaTextureFetch(ThreadContext *tc, gpusyscall_t *call_params)
{
    cuda_not_implemented(__my_func__,__LINE__);
}

namespace cuda_math {
    uint64_t __cudaMutexOperation(ThreadContext *tc, gpusyscall_t *call_params)
    {
        cuda_not_implemented(__my_func__,__LINE__);
        return 0;
    }

    uint64_t __cudaTextureFetch(ThreadContext *tc, gpusyscall_t *call_params)
    {
        cuda_not_implemented(__my_func__,__LINE__);
        return 0;
    }

    uint64_t __cudaSynchronizeThreads(ThreadContext *tc, gpusyscall_t *call_params)
    {
        //TODO This function should syncronize if we support Asyn kernel calls
        return g_last_cudaError = cudaSuccess;
    }

    void  __cudaTextureFetch(const void *tex, void *index, int integer, void *val) {
        cuda_not_implemented(__my_func__,__LINE__);
    }

    void __cudaMutexOperation(int lock)
    {
        cuda_not_implemented(__my_func__,__LINE__);
    }
}

/// static functions

static int load_static_globals(GPUSyscallHelper *helper, symbol_table *symtab, unsigned min_gaddr)
{
    DPRINTF(GPUSyscalls, "GPGPU-Sim PTX: loading globals with explicit initializers\n");
    int ng_bytes = 0;
    symbol_table::iterator g = symtab->global_iterator_begin();

    for (; g != symtab->global_iterator_end(); g++) {
        symbol *global = *g;
        if (global->has_initializer()) {
            DPRINTF(GPUSyscalls, "GPGPU-Sim PTX:     initializing '%s'\n", global->name().c_str());
            // unsigned addr = global->get_address();
            const type_info *type = global->type();
            type_info_key ti = type->get_key();
            size_t size;
            int t;
            ti.type_decode(size, t);
            int nbytes = size/8;
            Addr offset = 0;
            std::list<operand_info> init_list = global->get_initializer();
            for (std::list<operand_info>::iterator i = init_list.begin(); i != init_list.end(); i++) {
                operand_info op = *i;
                ptx_reg_t value = op.get_literal_value();

                Addr addr = global->get_address();
				assert( (addr+offset+nbytes) < min_gaddr ); // min_gaddr is start of "heap" for cudaMalloc
                helper->writeBlob(addr + offset, (uint8_t*)&value, nbytes);

                offset += nbytes;
                ng_bytes += nbytes;
            }
            DPRINTF(GPUSyscalls, " wrote %u bytes to \'%s\'\n", offset, global->name());
        }
    }
    DPRINTF(GPUSyscalls, "GPGPU-Sim PTX: finished loading globals (%u bytes total).\n", ng_bytes);
    return ng_bytes;
}

static int load_constants(GPUSyscallHelper *helper, symbol_table *symtab, addr_t min_gaddr)
{
   DPRINTF(GPUSyscalls, "GPGPU-Sim PTX: loading constants with explicit initializers\n");
   int nc_bytes = 0;
   symbol_table::iterator g = symtab->const_iterator_begin();

   for (; g != symtab->const_iterator_end(); g++) {
      symbol *constant = *g;
      if (constant->is_const() && constant->has_initializer()) {

         // get the constant element data size
         int basic_type;
         size_t num_bits;
         constant->type()->get_key().type_decode(num_bits, basic_type);

         std::list<operand_info> init_list = constant->get_initializer();
         int nbytes_written = 0;
         for (std::list<operand_info>::iterator i = init_list.begin(); i != init_list.end(); ++i) {
            operand_info op = *i;
            ptx_reg_t value = op.get_literal_value();
            int nbytes = num_bits/8;
            switch (op.get_type()) {
            case int_t: assert(nbytes >= 1); break;
            case float_op_t: assert(nbytes == 4); break;
            case double_op_t: assert(nbytes >= 4); break; // account for double DEMOTING
            default:
               panic("Op type not recognized in load_constants"); break;
            }

            Addr addr = constant->get_address() + nbytes_written;
			assert( addr+nbytes < min_gaddr );
            helper->writeBlob(addr, (uint8_t*)&value, nbytes);

            DPRINTF(GPUSyscalls, " wrote %u bytes to \'%s\'\n", nbytes, constant->name());
            nc_bytes += nbytes;
            nbytes_written += nbytes;
         }
      }
   }
   DPRINTF(GPUSyscalls, "GPGPU-Sim PTX: finished loading constants (%u bytes total).\n", nc_bytes);
   return nc_bytes;
}
#if 0
void
sysgem5gpu_active(ThreadContext *tc, gpusyscall_t *call_params)
{
    GPUSyscallHelper helper(tc, call_params);
    CudaGPU::getCudaGPU(g_active_device)->checkUpdateThreadContext(tc);

    CudaGPU *cudaGPU = CudaGPU::getCudaGPU(g_active_device);

    Addr sim_active = *((Addr*)helper.getParam(0, true));

    int active = cudaGPU->is_active();
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
