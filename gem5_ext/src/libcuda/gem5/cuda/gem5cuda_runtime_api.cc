/*
 * This version of the CUDA runtime is included for use with gem5-gpu. The
 * following copyright should appear before other original copyright notices:
 *
 * Copyright (c) 2012-2014 Mark D. Hill and David A. Wood
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

// This file created from cuda_runtime_api.h distributed with CUDA 1.1
// Changes Copyright 2009,  Tor M. Aamodt, Ali Bakhoda and George L. Yuan
// University of British Columbia

/*
 * cuda_runtime_api.cc
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdarg.h>
#include <dlfcn.h>
#include <unistd.h>
#include <stdint.h>

#ifdef OPENGL_SUPPORT
#define GL_GLEXT_PROTOTYPES
 #ifdef __APPLE__
 #include <GLUT/glut.h> // Apple's version of GLUT is here
 #else
 #include <GL/gl.h>
 #endif
#endif

// #include "__cudaFatFormat.h"
#include "cuda_runtime_api.h"

#ifndef __MEM_DEBUG__
// Wrap m5op in a namespace so calls to m5_gpu can be intercepted and
// pre-processed out if debugging or running tests on hardware
namespace m5op {
    extern "C" {
        #include "m5op.h"
    }
}
#endif

#include "../m5op/gem5_syscall_util.h"
#include "../../abstract_hardware_model.h"
// #include "../src/cuda-sim/ptx_ir.h"

extern "C" void app_direct_call_gpu(uint64_t gpusysno, uint64_t call_params);
extern "C" void gem5_main();

typedef void (*pfn_app_direct_call_gpu)(uint64_t gpusysno, uint64_t call_params);
typedef void (*pfn_gem5_main)();

static int app_run_in_gem5 = 0;
static pfn_app_direct_call_gpu g_app_direct_call_gpu = nullptr;
static pfn_gem5_main g_gem5_main = nullptr;
// TODO local thread
static void set_run_in_gem5() {
    char *mode = getenv("APP_RUN_IN_GEM5");
    if (mode) {
        app_run_in_gem5 = 1;
    } else {
        app_run_in_gem5 = -1;
        void* libgem5_sc_handle = dlopen("libgem5_sc.so", RTLD_LAZY | RTLD_GLOBAL);
        if (libgem5_sc_handle == nullptr) {
            printf("dlopen error - %s\n", dlerror());
            assert(false);
        }
        g_gem5_main = (pfn_gem5_main)dlsym(libgem5_sc_handle, "gem5_main");
        if (g_gem5_main == nullptr) {
            printf("dlopen error - %s\n", dlerror());
            printf("load symbol g_gem5_main faile\n");
            assert(false);
        }
        /*
        void* libgem5_handle = dlopen("libgem5_opt.so", RTLD_NOW);
        if (libgem5_handle = nullptr) {
            printf("dlopen error - %s\n", dlerror());
            assert(false);
        }
        */
        g_app_direct_call_gpu = (pfn_app_direct_call_gpu)dlsym(libgem5_sc_handle, "app_direct_call_gpu");
        if (g_app_direct_call_gpu == nullptr) {
            printf("dlopen error - %s\n", dlerror());
            printf("load symbol app_direct_call_gpu fail\n");
            assert(false);
        }
        (g_gem5_main)();
    }
}


inline void m5_gpu(uint64_t __gpusysno, uint64_t call_params) {
    if (app_run_in_gem5 == 0) {
        set_run_in_gem5();
    }
    if (app_run_in_gem5 != 1) {
        // call pseudo_inst.cc
        (g_app_direct_call_gpu)(__gpusysno, call_params);
    } else {
#ifndef __MEM_DEBUG__
    m5op::m5_gpu(__gpusysno, call_params);
#endif
    }
}

//cudaError_t g_last_cudaError = cudaSuccess;
extern cudaError_t g_last_cudaError;

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern "C" {

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

 cudaError_t  gem5cudaMallocHelper(void **ptr, size_t size, unsigned type)
{
    gpusyscall_t call_params;
    call_params.num_args = 2;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(void **);
    call_params.arg_lengths[1] = sizeof(size_t);
    call_params.total_bytes = call_params.arg_lengths[0] + call_params.arg_lengths[1];

    call_params.args = new char[call_params.total_bytes];

    call_params.ret = new char[sizeof(cudaError_t)];
    cudaError_t* ret_spot = (cudaError_t*)call_params.ret;
    *ret_spot = cudaSuccess;

    int bytes_off = 0;
    int lengths_off = 0;

    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&ptr, call_params.arg_lengths[0]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&size, call_params.arg_lengths[1]);

    m5_gpu(type, (uint64_t)&call_params);
    cudaError_t ret = *((cudaError_t*)call_params.ret);

    delete call_params.args;
    delete call_params.arg_lengths;
    delete call_params.ret;

    if (ret == cudaErrorApiFailureBase) {
        // This return code indicates that memory management must be handled
        // outside the simulator, so CPU must allocate memory for GPU
        *ptr = checkedAlignedAlloc(size);
        ret = cudaSuccess;

        if (type == CUDA_MALLOC_DEVICE) {
            // Touch all pages to ensure OS mapping
            touchPages((unsigned char*)*ptr, size);

            // Need to register this memory as device memory in simulator.
            // This registration is used if/when enforcing access permissions
            // between CPU and GPU
            call_params.num_args = 2;
            call_params.arg_lengths = new int[call_params.num_args];

            call_params.arg_lengths[0] = sizeof(void*);
            call_params.arg_lengths[1] = sizeof(size_t);
            call_params.total_bytes = call_params.arg_lengths[0] + call_params.arg_lengths[1];

            call_params.args = new char[call_params.total_bytes];

            call_params.ret = new char[sizeof(cudaError_t)];
            ret_spot = (cudaError_t*)call_params.ret;
            *ret_spot = cudaSuccess;

            bytes_off = 0;
            lengths_off = 0;

            pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)ptr, call_params.arg_lengths[0]);
            pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&size, call_params.arg_lengths[1]);

            m5_gpu(82, (uint64_t)&call_params);
            assert(*((cudaError_t*)call_params.ret) == cudaSuccess);

            delete[] call_params.args;
            delete[] call_params.arg_lengths;
            delete[] call_params.ret;
        }
    }

    return ret;
}

 cudaError_t  gem5cudaMalloc(void **devPtr, size_t size)
{
    return gem5cudaMallocHelper(devPtr, size, CUDA_MALLOC_DEVICE);
}

 cudaError_t  gem5cudaMallocHost(void **ptr, size_t size)
{
    return gem5cudaMallocHelper(ptr, size, CUDA_MALLOC_HOST);
}

 cudaError_t  gem5cudaMallocPitch(void **devPtr, size_t *pitch, size_t width, size_t height)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
//   unsigned malloc_width_inbytes = width;
//   printf("GPGPU-Sim PTX: cudaMallocPitch (width = %d)\n", malloc_width_inbytes);
//   CUctx_st* ctx = GPGPUSim_Context();
//   *devPtr = ctx->get_device()->get_gpgpu()->gpu_malloc(malloc_width_inbytes*height);
//   pitch[0] = malloc_width_inbytes;
//   if ( *devPtr  ) {
//      return g_last_cudaError = cudaSuccess;
//   } else {
//      return g_last_cudaError = cudaErrorMemoryAllocation;
//   }
}

#if 1
 cudaError_t  gem5cudaMallocArray(struct libcuda::cudaArray **array, const struct cudaChannelFormatDesc *desc, size_t width, size_t height, unsigned int flags)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
//   unsigned size = width * height * ((desc->x + desc->y + desc->z + desc->w)/8);
//   CUctx_st* context = GPGPUSim_Context();
//   (*array) = (struct cudaArray*) malloc(sizeof(struct cudaArray));
//   (*array)->desc = *desc;
//   (*array)->width = width;
//   (*array)->height = height;
//   (*array)->size = size;
//   (*array)->dimensions = 2;
//   ((*array)->devPtr32)= (int) (long long)context->get_device()->get_gpgpu()->gpu_mallocarray(size);
//   printf("GPGPU-Sim PTX: cudaMallocArray: devPtr32 = %d\n", ((*array)->devPtr32));
//   ((*array)->devPtr) = (void*) (long long) ((*array)->devPtr32);
//   if ( ((*array)->devPtr) ) {
//       return g_last_cudaError = cudaSuccess;
//   } else {
//       return g_last_cudaError = cudaErrorMemoryAllocation;
//   }
}
#endif

 cudaError_t  gem5cudaFreeHelper(void* ptr, unsigned type)
{
    gpusyscall_t call_params;
    call_params.num_args = 1;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(void*);
    call_params.total_bytes = call_params.arg_lengths[0];

    call_params.args = new char[call_params.total_bytes];
    call_params.ret = new char[sizeof(cudaError_t)];
    cudaError_t* ret_spot = (cudaError_t*)call_params.ret;
    *ret_spot = cudaSuccess;

    int bytes_off = 0;
    int lengths_off = 0;

    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&ptr, call_params.arg_lengths[0]);

    m5_gpu(type, (uint64_t)&call_params);
    cudaError_t ret = *((cudaError_t*)call_params.ret);

    if (ret == cudaErrorApiFailureBase) {
        // This is returned to indicate that libcuda should free the memory
        // i.e. the memory was allocated on the CPU
        free(ptr);
        ret = g_last_cudaError = cudaSuccess;
    }

    delete call_params.args;
    delete call_params.arg_lengths;
    delete call_params.ret;

    return ret;
}

 cudaError_t  gem5cudaFree(void *devPtr)
{
    return gem5cudaFreeHelper(devPtr, CUDA_FREE_DEVICE);
}

 cudaError_t  gem5cudaFreeHost(void *ptr)
{
    return gem5cudaFreeHelper(ptr, CUDA_FREE_HOST);
}

void
blockThread()
{
    // Cache line align the bool to ensure other values are not allocated on
    // the same line to avoid contention and Ruby functional access failures
    bool *is_free;
    is_free = (bool*) checkedAlignedAlloc(CACHE_BLOCK_SIZE_BYTES);
    *is_free = false;

    gpusyscall_t call_params;
    call_params.num_args = 1;
    call_params.arg_lengths = new int[call_params.num_args];
    call_params.arg_lengths[0] = sizeof(bool*);
    call_params.total_bytes = call_params.arg_lengths[0];
    call_params.args = new char[call_params.total_bytes];
    int bytes_off = 0;
    int lengths_off = 0;
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&is_free, call_params.arg_lengths[0]);

    // This while loop is used to suppress interrupts that awaken the thread.
    // When a thread is woken up, it may handle a system call, and it should
    // return to blocking (suspended) if the mutex has not yet been cleared.
    bool free_to_pass = *is_free;
    while(!free_to_pass) {
        // cudaBlockThread,    /* 83 */
        m5_gpu(83, (uint64_t)&call_params);
        free_to_pass = *is_free;
#ifdef __MEM_DEBUG__
        free_to_pass = true;
#endif
    }

    delete[] call_params.args;
    delete[] call_params.arg_lengths;
    delete is_free;
}

#if 0
 cudaError_t  gem5cudaFreeArray(struct cudaArray *array)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
};
#endif


/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/
// cudaMemcpy syscall
//
 cudaError_t  gem5cudaMemcpyAsync(void *dst, const void *src, size_t count, enum cudaMemcpyKind kind, cudaStream_t stream)
{
#ifndef NO_TOUCH_PAGES
    // If transfer will access host memory, touch it to ensure OS page mapping
    if (kind == cudaMemcpyHostToDevice) {
        touchPages((unsigned char*)src, count);
    } else if(kind == cudaMemcpyDeviceToHost) {
        touchPages((unsigned char*)dst, count);
    }
#endif

    gpusyscall_t call_params;
    call_params.num_args = 5;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(void*);
    call_params.arg_lengths[1] = sizeof(const void*);
    call_params.arg_lengths[2] = sizeof(size_t);
    call_params.arg_lengths[3] = sizeof(enum cudaMemcpyKind);
    call_params.arg_lengths[4] = sizeof(cudaStream_t);
    call_params.total_bytes = call_params.arg_lengths[0] +
            call_params.arg_lengths[1] + call_params.arg_lengths[2] +
            call_params.arg_lengths[3] + call_params.arg_lengths[4];

    call_params.args = new char[call_params.total_bytes];
    call_params.ret = new char[sizeof(bool)];
    bool* ret_spot = (bool*)call_params.ret;
    *ret_spot = false;

    int bytes_off = 0;
    int lengths_off = 0;
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&dst, call_params.arg_lengths[0]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&src, call_params.arg_lengths[1]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&count, call_params.arg_lengths[2]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&kind, call_params.arg_lengths[3]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&stream, call_params.arg_lengths[4]);

    m5_gpu(7, (uint64_t)&call_params);
    bool block_thread = *((bool*)call_params.ret);

    delete call_params.args;
    delete call_params.arg_lengths;
    delete call_params.ret;

    if (block_thread) {
        blockThread();
    }

    return cudaSuccess;
}

__host__ cudaError_t CUDARTAPI gem5cudaMemcpy(void *dst, const void *src, size_t count, enum cudaMemcpyKind kind)
{
#ifndef NO_TOUCH_PAGES
    // If transfer will access host memory, touch it to ensure OS page mapping
    if (kind == cudaMemcpyHostToDevice) {
        touchPages((unsigned char*)src, count);
    } else if(kind == cudaMemcpyDeviceToHost) {
        touchPages((unsigned char*)dst, count);
    }
#endif

    gpusyscall_t call_params;
    call_params.num_args = 4;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(void*);
    call_params.arg_lengths[1] = sizeof(const void*);
    call_params.arg_lengths[2] = sizeof(size_t);
    call_params.arg_lengths[3] = sizeof(enum cudaMemcpyKind);
    call_params.total_bytes = call_params.arg_lengths[0] +
            call_params.arg_lengths[1] + call_params.arg_lengths[2] +
            call_params.arg_lengths[3];

    call_params.args = new char[call_params.total_bytes];
    call_params.ret = new char[sizeof(bool)];
    bool* ret_spot = (bool*)call_params.ret;
    *ret_spot = false;

    int bytes_off = 0;
    int lengths_off = 0;
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&dst, call_params.arg_lengths[0]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&src, call_params.arg_lengths[1]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&count, call_params.arg_lengths[2]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&kind, call_params.arg_lengths[3]);

    m5_gpu(7, (uint64_t)&call_params);
    bool block_thread = *((bool*)call_params.ret);

    delete call_params.args;
    delete call_params.arg_lengths;
    delete call_params.ret;

    if (block_thread) {
        blockThread();
    }

    return cudaSuccess;
}

#if 0
 cudaError_t  gem5cudaMemcpyToArray(struct cudaArray *dst, size_t wOffset, size_t hOffset, const void *src, size_t count, enum cudaMemcpyKind kind)
{
     cuda_not_implemented(__FILE__, __my_func__, __LINE__);
     return g_last_cudaError = cudaErrorUnknown;
//   CUctx_st *context = GPGPUSim_Context();
//   gpgpu_t *gpu = context->get_device()->get_gpgpu();
//   size_t size = count;
//   printf("GPGPU-Sim PTX: cudaMemcpyToArray\n");
//   if( kind == cudaMemcpyHostToDevice )
//      gpu->memcpy_to_gpu( (size_t)(dst->devPtr), src, size);
//   else if( kind == cudaMemcpyDeviceToHost )
//      gpu->memcpy_from_gpu( dst->devPtr, (size_t)src, size);
//   else if( kind == cudaMemcpyDeviceToDevice )
//      gpu->memcpy_gpu_to_gpu( (size_t)(dst->devPtr), (size_t)src, size);
//   else {
//      printf("GPGPU-Sim PTX: cudaMemcpyToArray - ERROR : unsupported cudaMemcpyKind\n");
//      abort();
//   }
//   dst->devPtr32 = (unsigned) (size_t)(dst->devPtr);
//   return g_last_cudaError = cudaSuccess;
}
#endif

#if 0
 cudaError_t  gem5cudaMemcpyFromArray(void *dst, const struct cudaArray *src, size_t wOffset, size_t hOffset, size_t count, enum cudaMemcpyKind kind)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
}

 cudaError_t  gem5cudaMemcpyArrayToArray(struct cudaArray *dst, size_t wOffsetDst, size_t hOffsetDst, const struct cudaArray *src, size_t wOffsetSrc, size_t hOffsetSrc, size_t count, enum cudaMemcpyKind kind)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
}
#endif

 cudaError_t  gem5cudaMemcpy2D(void *dst, size_t dpitch, const void *src, size_t spitch, size_t width, size_t height, enum cudaMemcpyKind kind)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
//   CUctx_st *context = GPGPUSim_Context();
//   gpgpu_t *gpu = context->get_device()->get_gpgpu();
//   struct cudaArray *cuArray_ptr;
//   size_t size = spitch*height;
//   cuArray_ptr = (cudaArray*)dst;
//   gpgpusim_ptx_assert( (dpitch==spitch), "different src and dst pitch not supported yet" );
//   if( kind == cudaMemcpyHostToDevice )
//      gpu->memcpy_to_gpu( (size_t)dst, src, size );
//   else if( kind == cudaMemcpyDeviceToHost )
//      gpu->memcpy_from_gpu( dst, (size_t)src, size );
//   else if( kind == cudaMemcpyDeviceToDevice )
//      gpu->memcpy_gpu_to_gpu( (size_t)dst, (size_t)src, size);
//   else {
//      printf("GPGPU-Sim PTX: cudaMemcpy2D - ERROR : unsupported cudaMemcpyKind\n");
//      abort();
//   }
//   return g_last_cudaError = cudaSuccess;
}

#if 0
 cudaError_t  gem5cudaMemcpy2DToArray(struct cudaArray *dst, size_t wOffset, size_t hOffset, const void *src, size_t spitch, size_t width, size_t height, enum cudaMemcpyKind kind)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
//   CUctx_st *context = GPGPUSim_Context();
//   gpgpu_t *gpu = context->get_device()->get_gpgpu();
//   size_t size = spitch*height;
//   size_t channel_size = dst->desc.w+dst->desc.x+dst->desc.y+dst->desc.z;
//   gpgpusim_ptx_assert( ((channel_size%8) == 0), "none byte multiple destination channel size not supported (sz=%u)", channel_size );
//   unsigned elem_size = channel_size/8;
//   gpgpusim_ptx_assert( (dst->dimensions==2), "copy to none 2D array not supported" );
//   gpgpusim_ptx_assert( (wOffset==0), "non-zero wOffset not yet supported" );
//   gpgpusim_ptx_assert( (hOffset==0), "non-zero hOffset not yet supported" );
//   gpgpusim_ptx_assert( (dst->height == (int)height), "partial copy not supported" );
//   gpgpusim_ptx_assert( (elem_size*dst->width == width), "partial copy not supported" );
//   gpgpusim_ptx_assert( (spitch == width), "spitch != width not supported" );
//   if( kind == cudaMemcpyHostToDevice )
//      gpu->memcpy_to_gpu( (size_t)(dst->devPtr), src, size);
//   else if( kind == cudaMemcpyDeviceToHost )
//      gpu->memcpy_from_gpu( dst->devPtr, (size_t)src, size);
//   else if( kind == cudaMemcpyDeviceToDevice )
//      gpu->memcpy_gpu_to_gpu( (size_t)dst->devPtr, (size_t)src, size);
//   else {
//      printf("GPGPU-Sim PTX: cudaMemcpy2D - ERROR : unsupported cudaMemcpyKind\n");
//      abort();
//   }
//   dst->devPtr32 = (unsigned) (size_t)(dst->devPtr);
//   return g_last_cudaError = cudaSuccess;
}

 cudaError_t  gem5cudaMemcpy2DFromArray(void *dst, size_t dpitch, const struct cudaArray *src, size_t wOffset, size_t hOffset, size_t width, size_t height, enum cudaMemcpyKind kind)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
}

 cudaError_t  gem5cudaMemcpy2DArrayToArray(struct cudaArray *dst, size_t wOffsetDst, size_t hOffsetDst, const struct cudaArray *src, size_t wOffsetSrc, size_t hOffsetSrc, size_t width, size_t height, enum cudaMemcpyKind kind)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
}
#endif

 cudaError_t  gem5cudaMemcpyToSymbolAsync(const char *symbol, const void *src, size_t count, size_t offset, enum cudaMemcpyKind kind, cudaStream_t stream)
{
#ifndef NO_TOUCH_PAGES
    // Touch host memory to ensure OS page mapping
    touchPages((unsigned char*)src, count);
#endif

    gpusyscall_t call_params;
    call_params.num_args = 6;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(const char*);
    call_params.arg_lengths[1] = sizeof(const void*);
    call_params.arg_lengths[2] = sizeof(size_t);
    call_params.arg_lengths[3] = sizeof(size_t);
    call_params.arg_lengths[4] = sizeof(enum cudaMemcpyKind);
    call_params.arg_lengths[5] = sizeof(cudaStream_t);
    call_params.total_bytes = call_params.arg_lengths[0] +
            call_params.arg_lengths[1] + call_params.arg_lengths[2] +
            call_params.arg_lengths[3] + call_params.arg_lengths[4] +
            call_params.arg_lengths[5];

    call_params.args = new char[call_params.total_bytes];
    call_params.ret = new char[sizeof(bool)];
    bool* ret_spot = (bool*)call_params.ret;
    *ret_spot = false;

    int bytes_off = 0;
    int lengths_off = 0;
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&symbol, call_params.arg_lengths[0]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&src, call_params.arg_lengths[1]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&count, call_params.arg_lengths[2]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&offset, call_params.arg_lengths[3]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&kind, call_params.arg_lengths[4]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&stream, call_params.arg_lengths[5]);

    m5_gpu(15, (uint64_t)&call_params);
    bool block_thread = *((bool*)call_params.ret);

    delete call_params.args;
    delete call_params.arg_lengths;
    delete call_params.ret;

    if (block_thread) {
        blockThread();
    }

    return cudaSuccess;
}

 cudaError_t  gem5cudaMemcpyFromSymbolAsync(void *dst, const char *symbol, size_t count, size_t offset, enum cudaMemcpyKind kind, cudaStream_t stream )
{
#ifndef NO_TOUCH_PAGES
    // Touch host memory to ensure OS page mapping
    touchPages((unsigned char*)dst, count);
#endif

    gpusyscall_t call_params;
    call_params.num_args = 6;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(void*);
    call_params.arg_lengths[1] = sizeof(const char*);
    call_params.arg_lengths[2] = sizeof(size_t);
    call_params.arg_lengths[3] = sizeof(size_t);
    call_params.arg_lengths[4] = sizeof(enum cudaMemcpyKind);
    call_params.arg_lengths[5] = sizeof(cudaStream_t);
    call_params.total_bytes = call_params.arg_lengths[0] +
            call_params.arg_lengths[1] + call_params.arg_lengths[2] +
            call_params.arg_lengths[3] + call_params.arg_lengths[4] +
            call_params.arg_lengths[5];

    call_params.args = new char[call_params.total_bytes];
    call_params.ret = new char[sizeof(bool)];
    bool* ret_spot = (bool*)call_params.ret;
    *ret_spot = false;

    int bytes_off = 0;
    int lengths_off = 0;
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&dst, call_params.arg_lengths[0]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&symbol, call_params.arg_lengths[1]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&count, call_params.arg_lengths[2]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&offset, call_params.arg_lengths[3]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&kind, call_params.arg_lengths[4]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&stream, call_params.arg_lengths[5]);

    m5_gpu(16, (uint64_t)&call_params);
    bool block_thread = *((bool*)call_params.ret);

    delete call_params.args;
    delete call_params.arg_lengths;
    delete call_params.ret;

    if (block_thread) {
        blockThread();
    }

    return cudaSuccess;
}

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/
/*
  cudaError_t  gem5cudaMemcpyAsync(void *dst, const void *src, size_t count, enum cudaMemcpyKind kind, cudaStream_t stream)
{
     cuda_not_implemented(__FILE__, __my_func__, __LINE__);
     return g_last_cudaError = cudaErrorUnknown;
//    struct CUstream_st *s = (struct CUstream_st *)stream;
//    switch( kind ) {
//    case cudaMemcpyHostToDevice: g_stream_manager->push( stream_operation(src,(size_t)dst,count,s) ); break;
//    case cudaMemcpyDeviceToHost: g_stream_manager->push( stream_operation((size_t)src,dst,count,s) ); break;
//    case cudaMemcpyDeviceToDevice: g_stream_manager->push( stream_operation((size_t)src,(size_t)dst,count,s) ); break;
//    default:
//        abort();
//    }
//    return g_last_cudaError = cudaSuccess;
}
*/
#if 0
 cudaError_t  gem5cudaMemcpyToArrayAsync(struct cudaArray *dst, size_t wOffset, size_t hOffset, const void *src, size_t count, enum cudaMemcpyKind kind, cudaStream_t stream)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
}

 cudaError_t  gem5cudaMemcpyFromArrayAsync(void *dst, const struct cudaArray *src, size_t wOffset, size_t hOffset, size_t count, enum cudaMemcpyKind kind, cudaStream_t stream)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
}
#endif

 cudaError_t  gem5cudaMemcpy2DAsync(void *dst, size_t dpitch, const void *src, size_t spitch, size_t width, size_t height, enum cudaMemcpyKind kind, cudaStream_t stream)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
}

#if 0
 cudaError_t  gem5cudaMemcpy2DToArrayAsync(struct cudaArray *dst, size_t wOffset, size_t hOffset, const void *src, size_t spitch, size_t width, size_t height, enum cudaMemcpyKind kind, cudaStream_t stream)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
}

 cudaError_t  gem5cudaMemcpy2DFromArrayAsync(void *dst, size_t dpitch, const struct cudaArray *src, size_t wOffset, size_t hOffset, size_t width, size_t height, enum cudaMemcpyKind kind, cudaStream_t stream)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
}
#endif

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/
/*
 cudaError_t  gem5cudaMemsetAsync(void *mem, int c, size_t count, cudaStream_t stream)
{
    gpusyscall_t call_params;
    call_params.num_args = 4;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(void*);
    call_params.arg_lengths[1] = sizeof(int);
    call_params.arg_lengths[2] = sizeof(size_t);
    call_params.arg_lengths[3] = sizeof(cudaStream_t);
    call_params.total_bytes = call_params.arg_lengths[0] +
                  call_params.arg_lengths[1] + call_params.arg_lengths[2] +
                  call_params.arg_lengths[3];

    call_params.args = new char[call_params.total_bytes];

    call_params.ret = new char[sizeof(cudaError_t)];
    cudaError_t* ret_spot = (cudaError_t*)call_params.ret;
    *ret_spot = cudaSuccess;

    int bytes_off = 0;
    int lengths_off = 0;

    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&mem, call_params.arg_lengths[0]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&c, call_params.arg_lengths[1]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&count, call_params.arg_lengths[2]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&count, call_params.arg_lengths[3]);

    m5_gpu(23, (uint64_t)&call_params);
    cudaError_t ret = *((cudaError_t*)call_params.ret);

    delete call_params.args;
    delete call_params.arg_lengths;
    delete call_params.ret;

    if (ret == cudaErrorApiFailureBase) {
        // This return code indicates that memory management must be handled
        // outside the simulator, so CPU must allocate memory for GPU
        memset(mem, c, count);
        ret = g_last_cudaError = cudaSuccess;
    }

    return ret;
}
*/

// FIXME schi add it
cudaError_t  gem5cudaMemset(void *mem, int c, size_t count)
{
    gpusyscall_t call_params;
    call_params.num_args = 3;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(void*);
    call_params.arg_lengths[1] = sizeof(int);
    call_params.arg_lengths[2] = sizeof(size_t);
    call_params.total_bytes = call_params.arg_lengths[0] +
                  call_params.arg_lengths[1] + call_params.arg_lengths[2];

    call_params.args = new char[call_params.total_bytes];

    call_params.ret = new char[sizeof(cudaError_t)];
    cudaError_t* ret_spot = (cudaError_t*)call_params.ret;
    *ret_spot = cudaSuccess;

    int bytes_off = 0;
    int lengths_off = 0;

    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&mem, call_params.arg_lengths[0]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&c, call_params.arg_lengths[1]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&count, call_params.arg_lengths[2]);

    m5_gpu(23, (uint64_t)&call_params);
    cudaError_t ret = *((cudaError_t*)call_params.ret);

    delete call_params.args;
    delete call_params.arg_lengths;
    delete call_params.ret;

    if (ret == cudaErrorApiFailureBase) {
        // This return code indicates that memory management must be handled
        // outside the simulator, so CPU must allocate memory for GPU
        memset(mem, c, count);
        ret = g_last_cudaError = cudaSuccess;
    }

    return ret;

}

 cudaError_t  gem5cudaMemset2D(void *mem, size_t pitch, int c, size_t width, size_t height)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
}

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

 cudaError_t  gem5cudaGetSymbolAddress(void **devPtr, const char *symbol)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
}

 cudaError_t  gem5cudaGetSymbolSize(size_t *size, const char *symbol)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
}

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

 cudaError_t  gem5cudaGetDeviceCount(int *count)
{
    gpusyscall_t call_params;
    call_params.num_args = 1;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(int*);
    call_params.total_bytes = call_params.arg_lengths[0];

    call_params.args = new char[call_params.total_bytes];
    call_params.ret = new char[sizeof(cudaError_t)];
    cudaError_t* ret_spot = (cudaError_t*)call_params.ret;
    *ret_spot = cudaSuccess;

    int bytes_off = 0;
    int lengths_off = 0;

    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&count, call_params.arg_lengths[0]);

    m5_gpu(27, (uint64_t)&call_params);
    cudaError_t ret = *((cudaError_t*)call_params.ret);

    delete call_params.args;
    delete call_params.arg_lengths;
    delete call_params.ret;

    return ret;
}

 cudaError_t  gem5cudaGetDeviceProperties(struct cudaDeviceProp* prop, int device)
{
    gpusyscall_t call_params;
    call_params.num_args = 2;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(struct cudaDeviceProp*);
    call_params.arg_lengths[1] = sizeof(int);
    call_params.total_bytes = call_params.arg_lengths[0] + call_params.arg_lengths[1];

    call_params.args = new char[call_params.total_bytes];
    call_params.ret = new char[sizeof(cudaError_t)];
    cudaError_t* ret_spot = (cudaError_t*)call_params.ret;
    *ret_spot = cudaSuccess;

    int bytes_off = 0;
    int lengths_off = 0;

    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&prop, call_params.arg_lengths[0]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&device, call_params.arg_lengths[1]);

    m5_gpu(28, (uint64_t)&call_params);
    cudaError_t ret = *((cudaError_t*)call_params.ret);

    delete call_params.args;
    delete call_params.arg_lengths;
    delete call_params.ret;

    return ret;
}

 cudaError_t  gem5cudaChooseDevice(int *device, const struct cudaDeviceProp *prop)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
//   _cuda_device_id *dev = GPGPUSim_Init();
//   *device = dev->get_id();
//   return g_last_cudaError = cudaSuccess;
}

 cudaError_t  gem5cudaSetDevice(int device)
{
    gpusyscall_t call_params;
    call_params.num_args = 1;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(int);
    call_params.total_bytes = call_params.arg_lengths[0];

    call_params.args = new char[call_params.total_bytes];
    call_params.ret = new char[sizeof(cudaError_t)];
    cudaError_t* ret_spot = (cudaError_t*)call_params.ret;
    *ret_spot = cudaSuccess;

    int bytes_off = 0;
    int lengths_off = 0;

    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char*)&device, call_params.arg_lengths[0]);

    m5_gpu(30, (uint64_t)&call_params);
    cudaError_t ret = *((cudaError_t*)call_params.ret);

    delete call_params.args;
    delete call_params.arg_lengths;
    delete call_params.ret;

    return ret;
}


 cudaError_t  gem5cudaGetDevice(int *device)
{
    gpusyscall_t call_params;
    call_params.num_args = 1;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(int*);
    call_params.total_bytes = call_params.arg_lengths[0];

    call_params.args = new char[call_params.total_bytes];
    call_params.ret = new char[sizeof(cudaError_t)];
    cudaError_t* ret_spot = (cudaError_t*)call_params.ret;
    *ret_spot = cudaSuccess;

    int bytes_off = 0;
    int lengths_off = 0;

    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char*)&device, call_params.arg_lengths[0]);

    m5_gpu(31, (uint64_t)&call_params);
    cudaError_t ret = *((cudaError_t*)call_params.ret);

    delete call_params.args;
    delete call_params.arg_lengths;
    delete call_params.ret;

    return ret;
}

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

 cudaError_t  gem5cudaBindTexture(size_t *offset,
        const struct textureReference *texref, const void *devPtr,
        const struct cudaChannelFormatDesc *desc, size_t size)
{
    gpusyscall_t call_params;
    call_params.num_args = 5;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(size_t*);
    call_params.arg_lengths[1] = sizeof(const struct textureReference*);
    call_params.arg_lengths[2] = sizeof(const void*);
    call_params.arg_lengths[3] = sizeof(const struct cudaChannelFormatDesc*);
    call_params.arg_lengths[4] = sizeof(size_t);
    call_params.total_bytes = call_params.arg_lengths[0] +
            call_params.arg_lengths[1] + call_params.arg_lengths[2] +
            call_params.arg_lengths[3] + call_params.arg_lengths[4];

    call_params.args = new char[call_params.total_bytes];
    call_params.ret = new char[sizeof(cudaError_t)];
    cudaError_t* ret_spot = (cudaError_t*)call_params.ret;
    *ret_spot = cudaSuccess;

    int bytes_off = 0;
    int lengths_off = 0;
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&offset, call_params.arg_lengths[0]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&texref, call_params.arg_lengths[1]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&devPtr, call_params.arg_lengths[2]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&desc, call_params.arg_lengths[3]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&size, call_params.arg_lengths[4]);

    m5_gpu(32, (uint64_t)&call_params);
    cudaError_t ret = *((cudaError_t*)call_params.ret);

    delete call_params.args;
    delete call_params.arg_lengths;
    delete call_params.ret;

    return ret;
}

#if 0
 cudaError_t  gem5cudaBindTextureToArray(const struct textureReference *texref, const struct cudaArray *array, const struct cudaChannelFormatDesc *desc)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
//    CUctx_st *context = GPGPUSim_Context();
//    gpgpu_t *gpu = context->get_device()->get_gpgpu();
//   printf("GPGPU-Sim PTX: in cudaBindTextureToArray: %p %p\n", texref, array);
//   printf("GPGPU-Sim PTX:   devPtr32 = %x\n", array->devPtr32);
//   printf("GPGPU-Sim PTX:   Name corresponding to textureReference: %s\n", gpu->gpgpu_ptx_sim_findNamefromTexture(texref));
//   printf("GPGPU-Sim PTX:   Texture Normalized? = %d\n", texref->normalized);
//   gpu->gpgpu_ptx_sim_bindTextureToArray(texref, array);
//   return g_last_cudaError = cudaSuccess;
}
#endif

 cudaError_t  gem5cudaUnbindTexture(const struct textureReference *texref)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaSuccess;
}

 cudaError_t  gem5cudaGetTextureAlignmentOffset(size_t *offset, const struct textureReference *texref)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
}

 cudaError_t  gem5cudaGetTextureReference(const struct textureReference **texref, const char *symbol)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
}

#if 0
 cudaError_t  gem5cudaGetChannelDesc(struct cudaChannelFormatDesc *desc, const struct cudaArray *array)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
//   *desc = array->desc;
//   return g_last_cudaError = cudaSuccess;
}
#endif

 struct cudaChannelFormatDesc  gem5cudaCreateChannelDesc(int x, int y, int z, int w, enum cudaChannelFormatKind f)
{
    struct cudaChannelFormatDesc dummy;
    dummy.x = x;
    dummy.y = y;
    dummy.z = z;
    dummy.w = w;
    dummy.f = f;
    return dummy;
}

 cudaError_t  gem5cudaGetLastError(void)
{
    gpusyscall_t call_params;
    call_params.num_args = 0;
    call_params.total_bytes = 0;
    call_params.ret = new char[sizeof(cudaError_t)];
    cudaError_t* ret_spot = (cudaError_t*)call_params.ret;
    *ret_spot = cudaSuccess;

    m5_gpu(39, (uint64_t)&call_params);
    cudaError_t ret = *((cudaError_t*)call_params.ret);

    delete call_params.ret;

    if (ret >= cudaErrorApiFailureBase) {
        if (g_last_cudaError != cudaSuccess) {
            if (ret != g_last_cudaError) {
                ret = (ret < g_last_cudaError) ? ret : g_last_cudaError;
            }
        } else {
            ret = cudaSuccess;
        }
    }

    return ret;
}

 const char*  gem5cudaGetErrorString(cudaError_t error)
{
    return cudaErrorStrings[error];
}

 cudaError_t  gem5cudaConfigureCall(dim3 gridDim, dim3 blockDim, size_t sharedMem, cudaStream_t stream)
{
    gpusyscall_t call_params;
    call_params.num_args = 4;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(dim3);
    call_params.arg_lengths[1] = sizeof(dim3);
    call_params.arg_lengths[2] = sizeof(size_t);
    call_params.arg_lengths[3] = sizeof(cudaStream_t);
    call_params.total_bytes = call_params.arg_lengths[0] +
            call_params.arg_lengths[1] + call_params.arg_lengths[2] +
            call_params.arg_lengths[3];

    call_params.args = new char[call_params.total_bytes];
    call_params.ret = new char[sizeof(cudaError_t)];
    cudaError_t* ret_spot = (cudaError_t*)call_params.ret;
    *ret_spot = cudaSuccess;

    int bytes_off = 0;
    int lengths_off = 0;
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&gridDim, call_params.arg_lengths[0]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&blockDim, call_params.arg_lengths[1]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&sharedMem, call_params.arg_lengths[2]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&stream, call_params.arg_lengths[3]);

    m5_gpu(41, (uint64_t)&call_params);
    cudaError_t ret = *((cudaError_t*)call_params.ret);

    delete call_params.args;
    delete call_params.arg_lengths;
    delete call_params.ret;

    return ret;
}
/*
 cudaError_t  gem5cudaSetupArgument(function_info *f, const void **args)
{
    gpusyscall_t call_params;
    call_params.num_args = 2;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(function_info*);
    call_params.arg_lengths[1] = sizeof(const void**);
    call_params.total_bytes = call_params.arg_lengths[0] +
            call_params.arg_lengths[1];

    call_params.args = new char[call_params.total_bytes];
    call_params.ret = new char[sizeof(cudaError_t)];
    cudaError_t* ret_spot = (cudaError_t*)call_params.ret;
    *ret_spot = cudaSuccess;

    int bytes_off = 0;
    int lengths_off = 0;
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&f, call_params.arg_lengths[0]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&args, call_params.arg_lengths[1]);

    m5_gpu(42, (uint64_t)&call_params);
    cudaError_t ret = *((cudaError_t*)call_params.ret);

    delete call_params.args;
    delete call_params.arg_lengths;
    delete call_params.ret;

    return ret;
}
*/

 cudaError_t  gem5cudaSetupArgument(const void *arg, size_t size, size_t offset)
{
    gpusyscall_t call_params;
    call_params.num_args = 3;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(const void*);
    call_params.arg_lengths[1] = sizeof(size_t);
    call_params.arg_lengths[2] = sizeof(size_t);
    call_params.total_bytes = call_params.arg_lengths[0] +
            call_params.arg_lengths[1] + call_params.arg_lengths[2];

    call_params.args = new char[call_params.total_bytes];
    call_params.ret = new char[sizeof(cudaError_t)];
    cudaError_t* ret_spot = (cudaError_t*)call_params.ret;
    *ret_spot = cudaSuccess;

    int bytes_off = 0;
    int lengths_off = 0;
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&arg, call_params.arg_lengths[0]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&size, call_params.arg_lengths[1]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&offset, call_params.arg_lengths[2]);

    m5_gpu(42, (uint64_t)&call_params);
    cudaError_t ret = *((cudaError_t*)call_params.ret);

    delete call_params.args;
    delete call_params.arg_lengths;
    delete call_params.ret;

    return ret;
}

 cudaError_t  gem5cudaFuncSetCacheConfig(const char *func, enum cudaFuncCache cacheConfig)
{
    DPRINTF("WARN: cudaFuncSetCacheConfig(%s, %d) ignored\n", func, cacheConfig);
    return g_last_cudaError = cudaSuccess;
}

 cudaError_t  gem5cudaLaunch(const char *hostFun) // , kernel_info_t* grid)
{
    gpusyscall_t call_params;
    call_params.num_args = 1;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(const char*);
    // call_params.arg_lengths[1] = sizeof(kernel_info_t*);
    // call_params.total_bytes = call_params.arg_lengths[0] + call_params.arg_lengths[1];
    call_params.total_bytes = call_params.arg_lengths[0];

    call_params.args = new char[call_params.total_bytes];
    call_params.ret = new char[sizeof(cudaError_t)];
    cudaError_t* ret_spot = (cudaError_t*)call_params.ret;
    *ret_spot = cudaSuccess;

    int bytes_off = 0;
    int lengths_off = 0;
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&hostFun, call_params.arg_lengths[0]);
    // pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&grid, call_params.arg_lengths[1]);

    // call libgem5cudaLaunch
    m5_gpu(43, (uint64_t)&call_params);
    cudaError_t ret = *((cudaError_t*)call_params.ret);

    delete call_params.args;
    delete call_params.arg_lengths;
    delete call_params.ret;

    return ret;
}


/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

 cudaError_t  gem5cudaStreamCreate(cudaStream_t *stream)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
//   printf("GPGPU-Sim PTX: cudaStreamCreate\n");
//#if (CUDART_VERSION >= 3000)
//   *stream = new struct CUstream_st();
//   g_stream_manager->add_stream(*stream);
//#else
//   *stream = 0;
//   printf("GPGPU-Sim PTX: WARNING: Asynchronous kernel execution not supported (%s)\n", __my_func__);
//#endif
//   return g_last_cudaError = cudaSuccess;
}

 cudaError_t  gem5cudaStreamDestroy(cudaStream_t stream)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
//#if (CUDART_VERSION >= 3000)
//    g_stream_manager->destroy_stream(stream);
//#endif
//   return g_last_cudaError = cudaSuccess;
}

 cudaError_t  gem5cudaStreamSynchronize(cudaStream_t stream)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
//#if (CUDART_VERSION >= 3000)
//    if( stream == NULL )
//        return g_last_cudaError = cudaErrorInvalidResourceHandle;
//    stream->synchronize(); // Needs to be converted to blockThread()
//#else
//    printf("GPGPU-Sim PTX: WARNING: Asynchronous kernel execution not supported (%s)\n", __my_func__);
//#endif
//    return g_last_cudaError = cudaSuccess;
}

 cudaError_t  gem5cudaStreamQuery(cudaStream_t stream)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
//#if (CUDART_VERSION >= 3000)
//   if( stream == NULL )
//       return g_last_cudaError = cudaErrorInvalidResourceHandle;
//   return g_last_cudaError = stream->empty()?cudaSuccess:cudaErrorNotReady;
//#else
//   printf("GPGPU-Sim PTX: WARNING: Asynchronous kernel execution not supported (%s)\n", __my_func__);
//   return g_last_cudaError = cudaSuccess; // it is always success because all cuda calls are synchronous
//#endif
}

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

 cudaError_t  gem5cudaEventCreate(cudaEvent_t *event)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
//   CUevent_st *e = new CUevent_st(false);
// TODO: Convert to make call into gem5-gpu
//   g_timer_events[e->get_uid()] = e;
//#if CUDART_VERSION >= 3000
//   *event = e;
//#else
//   *event = e->get_uid();
//#endif
//   return g_last_cudaError = cudaSuccess;
}

 cudaError_t  gem5cudaEventRecord(cudaEvent_t event, cudaStream_t stream)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
//   CUevent_st *e = get_event(event);
//   if( !e ) return g_last_cudaError = cudaErrorUnknown;
//   struct CUstream_st *s = (struct CUstream_st *)stream;
//   stream_operation op(e,s);
//   g_stream_manager->push(op);
//   return g_last_cudaError = cudaSuccess;
}

 cudaError_t  gem5cudaEventQuery(cudaEvent_t event)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
//   CUevent_st *e = get_event(event);
//   if( e == NULL ) {
//      return g_last_cudaError = cudaErrorInvalidValue;
//   } else if( e->done() ) {
//      return g_last_cudaError = cudaSuccess;
//   } else {
//      return g_last_cudaError = cudaErrorNotReady;
//   }
}

 cudaError_t  gem5cudaEventSynchronize(cudaEvent_t event)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
//    printf("GPGPU-Sim API: cudaEventSynchronize ** waiting for event\n");
//    fflush(stdout);
//    CUevent_st *e = (CUevent_st*) event;
//    while( !e->done() )
//        ;
//    printf("GPGPU-Sim API: cudaEventSynchronize ** event detected\n");
//    fflush(stdout);
//    return g_last_cudaError = cudaSuccess;
}

 cudaError_t  gem5cudaEventDestroy(cudaEvent_t event)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
//   CUevent_st *e = get_event(event);
//   unsigned event_uid = e->get_uid();
// TODO: Convert to make call into gem5-gpu
//   event_tracker_t::iterator pe = g_timer_events.find(event_uid);
//   if( pe == g_timer_events.end() )
//      return g_last_cudaError = cudaErrorInvalidValue;
//   g_timer_events.erase(pe);
//   return g_last_cudaError = cudaSuccess;
}

 cudaError_t  gem5cudaEventElapsedTime(float *ms, cudaEvent_t start, cudaEvent_t end)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
//   time_t elapsed_time;
//   CUevent_st *s = get_event(start);
//   CUevent_st *e = get_event(end);
//   if( s==NULL || e==NULL )
//      return g_last_cudaError = cudaErrorUnknown;
//   elapsed_time = e->clock() - s->clock();
//   *ms = 1000*elapsed_time;
//   return g_last_cudaError = cudaSuccess;
}

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

 cudaError_t  gem5cudaThreadExit(void)
{
    gpusyscall_t call_params;
    call_params.num_args = 0;
    call_params.total_bytes = 0;
    call_params.ret = new char[sizeof(cudaError_t)];
    cudaError_t* ret_spot = (cudaError_t*)call_params.ret;
    *ret_spot = cudaSuccess;

    m5_gpu(54, (uint64_t)&call_params);
    cudaError_t ret = *((cudaError_t*)call_params.ret);

    delete call_params.ret;

    return ret;
}

 cudaError_t  gem5cudaThreadSynchronize(void)
{
    gpusyscall_t call_params;
    call_params.num_args = 0;
    call_params.total_bytes = 0;
    call_params.ret = new char[sizeof(bool)];
    bool* ret_spot = (bool*)call_params.ret;
    *ret_spot = false;

    m5_gpu(55, (uint64_t)&call_params);
    bool block_thread = *((bool*)call_params.ret);

    delete call_params.ret;

    if (block_thread) {
        blockThread();
    }

    return cudaSuccess;
}

/*
int  __cudaSynchronizeThreads(void**, void*)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
//   return cudaThreadExit();
}
*/
/*
void gem5cudaRegisterPtxInfo(const char *ptxinfo_kname, gpgpu_ptx_sim_info ptxinfo_kinfo)
{
    gpusyscall_t call_params;
    call_params.num_args = 2;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(const char *);
    call_params.arg_lengths[1] = sizeof(gpgpu_ptx_sim_info);
    call_params.total_bytes = call_params.arg_lengths[0] + call_params.arg_lengths[1];

    call_params.args = new char[call_params.total_bytes];
    call_params.ret = (char*)new int;

    int* ret_spot1 = (int*)call_params.ret;
    *ret_spot1 = -1;

    int bytes_off = 0;
    int lengths_off = 0;

    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&ptxinfo_kname, call_params.arg_lengths[0]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&ptxinfo_kinfo, call_params.arg_lengths[1]);

    m5_gpu(86, (uint64_t)&call_params);

    delete call_params.args;
    delete call_params.arg_lengths;
}
*/

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/
// void**  gem5cudaRegisterFatBinary(symbol_table *symtab, unsigned int handle, char *ptxinfo_kname, gpgpu_ptx_sim_info ptxinfo_kinfo)
class symbol_table;
void**  gem5cudaRegisterFatBinary(symbol_table *symtab, unsigned int handle)
{
    // Now, tell gem5 + GPGPU-Sim to register the binary
    gpusyscall_t call_params;
    call_params.num_args = 2;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(symbol_table *);
    call_params.arg_lengths[1] = sizeof(unsigned int);
    call_params.total_bytes = call_params.arg_lengths[0] + call_params.arg_lengths[1];

    call_params.args = new char[call_params.total_bytes];
    call_params.ret = (char*)new int;
    int* ret_spot1 = (int*)call_params.ret;
    *ret_spot1 = -1;

    int bytes_off = 0;
    int lengths_off = 0;

    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&symtab, call_params.arg_lengths[0]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&handle, call_params.arg_lengths[1]);

    //    libgem5cudaRegisterFatBinary,    /* 57 */
    m5_gpu(57, (uint64_t)&call_params);

    int allocation_size = *((int*)call_params.ret);

    delete call_params.args;
    delete call_params.arg_lengths;
    delete call_params.ret;

#ifndef __MEM_DEBUG__
    if (allocation_size < 0) {
        printf("gem5 + GPGPU-Sim CUDA RT: Problem with const allocation... Exiting\n");
        exit(-1);
    }
#endif

    // Allocate memory for globals and constants:
    int padding = allocation_size % PAGE_SIZE_BYTES;
    if (padding > 0) {
        allocation_size -= padding;
        allocation_size += PAGE_SIZE_BYTES;
    }
    unsigned char* alloc_ptr = NULL;


    // Second up-call to check for need to allocate GPU local memory
    call_params.num_args = 0;
    call_params.total_bytes = 0;
    call_params.ret = new char[sizeof(unsigned long long)];
    unsigned long long* ret_spot3 = (unsigned long long*)call_params.ret;
    *ret_spot3 = 0;
    // Call __cudaCheckAllocateLocal
    m5_gpu(84, (uint64_t)&call_params);
    unsigned long long allocate_local = *((unsigned long long*)call_params.ret);
    delete call_params.ret;

    if (allocate_local > 0) {
        // allocate_local now stores the amount of memory that the simulator
        // needs to allocate for GPU local memory. Allocate that memory, and
        // pass the virtual address back to the simulator.
        alloc_ptr = (unsigned char*) checkedAlignedAlloc(allocate_local, PAGE_SIZE_BYTES);

        call_params.num_args = 1;
        call_params.arg_lengths = new int[call_params.num_args];
        call_params.arg_lengths[0] = sizeof(void *);
        call_params.total_bytes = call_params.arg_lengths[0];
        call_params.args = new char[call_params.total_bytes];
        call_params.ret = new char[sizeof(bool)];
        bool* ret_spot4 = (bool*)call_params.ret;
        *ret_spot4 = false;

        bytes_off = 0;
        lengths_off = 0;
        pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&alloc_ptr, call_params.arg_lengths[0]);

        // Send the local memory allocation pointer into the simulator, so it
        // knows how to access it. The return value from this upcall is whether
        // the simulator needs the CPU to touch the memory pages to ensure
        // they are mapped by the OS.
        // call __cudaSetLocalAllocation
        m5_gpu(85, (uint64_t)&call_params);
        bool map_local = *((bool*)call_params.ret);

        delete call_params.args;
        delete call_params.arg_lengths;
        delete call_params.ret;

        // The return from the upcall: If true, the simulator needs the GPU
        // local memory pages to be mapped, so touch them.
        if (map_local) {
            touchPages(alloc_ptr, allocate_local);
        }

        alloc_ptr = NULL;
    }


    // Third up-call to allocate globals and constants
    if (allocation_size > 0) {
        alloc_ptr = (unsigned char*) checkedAlignedAlloc(allocation_size, PAGE_SIZE_BYTES);

        // Const memory space is default allocated to 0, and this touches
        // all pages to ensure OS mapping... Double win
        memset(alloc_ptr, 0, allocation_size);
    }

    call_params.num_args = 1;
    call_params.arg_lengths = new int[call_params.num_args];
    call_params.arg_lengths[0] = sizeof(void *);
    call_params.total_bytes = call_params.arg_lengths[0];
    call_params.args = new char[call_params.total_bytes];
    call_params.ret = new char[sizeof(void**)];
    void*** ret_spot2 = (void***)call_params.ret;
    *ret_spot2 = NULL;

    bytes_off = 0;
    lengths_off = 0;
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&alloc_ptr, call_params.arg_lengths[0]);

    // call libcudaRegisterFatBinaryFinalize
    m5_gpu(81, (uint64_t)&call_params);
    void** ret = *((void***)call_params.ret);

    delete call_params.args;
    delete call_params.arg_lengths;
    delete call_params.ret;

    return ret;
}

void  gem5cudaRegisterFunction(void *fatCubinHandle,
        const char *hostFun, const char *deviceFun )
{
    gpusyscall_t call_params;
    call_params.num_args = 3;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(void*);
    call_params.arg_lengths[1] = sizeof(const char*);
    call_params.arg_lengths[2] = sizeof(const char*);
    call_params.total_bytes = call_params.arg_lengths[0] +
            call_params.arg_lengths[1] + call_params.arg_lengths[2];

    call_params.args = new char[call_params.total_bytes];

    int bytes_off = 0;
    int lengths_off = 0;

    touchPages((unsigned char*)hostFun, strlen(hostFun));
    touchPages((unsigned char*)deviceFun, strlen(deviceFun));

    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&fatCubinHandle, call_params.arg_lengths[0]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&hostFun, call_params.arg_lengths[1]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&deviceFun, call_params.arg_lengths[2]);

    // libcudaRegisterFunction,        /* 59 */
    m5_gpu(59, (uint64_t)&call_params);

    delete call_params.args;
    delete call_params.arg_lengths;
}


#if 0

void __cudaUnregisterFatBinary(void **fatCubinHandle)
{
    gpusyscall_t call_params;
    call_params.num_args = 1;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(void*);
    call_params.total_bytes = call_params.arg_lengths[0];

    call_params.args = new char[call_params.total_bytes];
    call_params.ret = new char[sizeof(void*)];

    int bytes_off = 0;
    int lengths_off = 0;
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&fatCubinHandle, call_params.arg_lengths[0]);

    m5_gpu(58, (uint64_t)&call_params);

    delete call_params.args;
    delete call_params.arg_lengths;
    delete call_params.ret;
}

void  __cudaRegisterFunction(void **fatCubinHandle,
        const char *hostFun, char *deviceFun, const char *deviceName,
        int thread_limit, uint3 *tid, uint3 *bid, dim3 *bDim, dim3 *gDim)
{
    gpusyscall_t call_params;
    call_params.num_args = 9;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(void**);
    call_params.arg_lengths[1] = sizeof(const char*);
    call_params.arg_lengths[2] = sizeof(char*);
    call_params.arg_lengths[3] = sizeof(const char*);
    call_params.arg_lengths[4] = sizeof(int);
    call_params.arg_lengths[5] = sizeof(uint3);
    call_params.arg_lengths[6] = sizeof(uint3);
    call_params.arg_lengths[7] = sizeof(dim3);
    call_params.arg_lengths[8] = sizeof(dim3);
    call_params.total_bytes = call_params.arg_lengths[0] +
            call_params.arg_lengths[1] + call_params.arg_lengths[2] +
            call_params.arg_lengths[3] + call_params.arg_lengths[4] +
            call_params.arg_lengths[5] + call_params.arg_lengths[6] +
            call_params.arg_lengths[7] + call_params.arg_lengths[8];

    call_params.args = new char[call_params.total_bytes];

    int bytes_off = 0;
    int lengths_off = 0;

    touchPages((unsigned char*)hostFun, strlen(hostFun));
    touchPages((unsigned char*)deviceFun, strlen(deviceFun));

    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&fatCubinHandle, call_params.arg_lengths[0]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&hostFun, call_params.arg_lengths[1]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&deviceFun, call_params.arg_lengths[2]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&deviceName, call_params.arg_lengths[3]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&thread_limit, call_params.arg_lengths[4]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&tid, call_params.arg_lengths[5]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&bid, call_params.arg_lengths[6]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&bDim, call_params.arg_lengths[7]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&gDim, call_params.arg_lengths[8]);

    m5_gpu(59, (uint64_t)&call_params);

    delete call_params.args;
    delete call_params.arg_lengths;
}

extern void __cudaRegisterVar(void **fatCubinHandle, char *hostVar,
        char *deviceAddress, const char *deviceName, int ext, int size,
        int constant, int global )
{
    gpusyscall_t call_params;
    call_params.num_args = 8;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(void**);
    call_params.arg_lengths[1] = sizeof(char*);
    call_params.arg_lengths[2] = sizeof(char*);
    call_params.arg_lengths[3] = sizeof(const char*);
    call_params.arg_lengths[4] = sizeof(int);
    call_params.arg_lengths[5] = sizeof(int);
    call_params.arg_lengths[6] = sizeof(int);
    call_params.arg_lengths[7] = sizeof(int);
    call_params.total_bytes = call_params.arg_lengths[0] +
            call_params.arg_lengths[1] + call_params.arg_lengths[2] +
            call_params.arg_lengths[3] + call_params.arg_lengths[4] +
            call_params.arg_lengths[5] + call_params.arg_lengths[6] +
            call_params.arg_lengths[7];

    call_params.args = new char[call_params.total_bytes];

    int bytes_off = 0;
    int lengths_off = 0;

    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&fatCubinHandle, call_params.arg_lengths[0]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&hostVar, call_params.arg_lengths[1]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&deviceAddress, call_params.arg_lengths[2]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&deviceName, call_params.arg_lengths[3]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&ext, call_params.arg_lengths[4]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&size, call_params.arg_lengths[5]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&constant, call_params.arg_lengths[6]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&global, call_params.arg_lengths[7]);

    m5_gpu(60, (uint64_t)&call_params);

    delete call_params.args;
    delete call_params.arg_lengths;
}

void __cudaRegisterShared(
                         void **fatCubinHandle,
                         void **devicePtr
                         )
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
//   // we don't do anything here
//   printf("GPGPU-Sim PTX: __cudaRegisterShared\n" );
}

void  __cudaRegisterSharedVar(
                                      void   **fatCubinHandle,
                                      void   **devicePtr,
                                      size_t   size,
                                      size_t   alignment,
                                      int      storage
                                      )
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
//   // we don't do anything here
//   printf("GPGPU-Sim PTX: __cudaRegisterSharedVar\n" );
}

void __cudaRegisterTexture(void **fatCubinHandle,
        const struct textureReference *hostVar, const void **deviceAddress,
        const char *deviceName, int dim, int norm, int ext)
{
    gpusyscall_t call_params;
    call_params.num_args = 7;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(void**);
    call_params.arg_lengths[1] = sizeof(const struct textureReference*);
    call_params.arg_lengths[2] = sizeof(const void**);
    call_params.arg_lengths[3] = sizeof(const char*);
    call_params.arg_lengths[4] = sizeof(int);
    call_params.arg_lengths[5] = sizeof(int);
    call_params.arg_lengths[6] = sizeof(int);
    call_params.total_bytes = call_params.arg_lengths[0] +
            call_params.arg_lengths[1] + call_params.arg_lengths[2] +
            call_params.arg_lengths[3] + call_params.arg_lengths[4] +
            call_params.arg_lengths[5] + call_params.arg_lengths[6];

    call_params.args = new char[call_params.total_bytes];

    int bytes_off = 0;
    int lengths_off = 0;

    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&fatCubinHandle, call_params.arg_lengths[0]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&hostVar, call_params.arg_lengths[1]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&deviceAddress, call_params.arg_lengths[2]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&deviceName, call_params.arg_lengths[3]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&dim, call_params.arg_lengths[4]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&norm, call_params.arg_lengths[5]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&ext, call_params.arg_lengths[6]);

    m5_gpu(63, (uint64_t)&call_params);

    delete call_params.args;
    delete call_params.arg_lengths;
}
#endif

#ifndef OPENGL_SUPPORT
typedef unsigned long GLuint;
#endif

cudaError_t gem5cudaGLRegisterBufferObject(GLuint bufferObj)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
//   printf("GPGPU-Sim PTX: Execution warning: ignoring call to \"%s\"\n", __my_func__ );
//   return g_last_cudaError = cudaSuccess;
}

struct glbmap_entry {
   GLuint m_bufferObj;
   void *m_devPtr;
   size_t m_size;
   struct glbmap_entry *m_next;
};
typedef struct glbmap_entry glbmap_entry_t;

// glbmap_entry_t* g_glbmap = NULL;
extern glbmap_entry_t* g_glbmap;

cudaError_t gem5cudaGLMapBufferObject(void** devPtr, GLuint bufferObj)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
//#ifdef OPENGL_SUPPORT
//   GLint buffer_size=0;
//   CUctx_st* ctx = GPGPUSim_Context();
//
//   glbmap_entry_t *p = g_glbmap;
//   while ( p && p->m_bufferObj != bufferObj )
//      p = p->m_next;
//   if ( p == NULL ) {
//      glBindBuffer(GL_ARRAY_BUFFER,bufferObj);
//      glGetBufferParameteriv(GL_ARRAY_BUFFER,GL_BUFFER_SIZE,&buffer_size);
//      assert( buffer_size != 0 );
//      *devPtr = ctx->get_device()->get_gpgpu()->gpu_malloc(buffer_size);
//
//      // create entry and insert to front of list
//      glbmap_entry_t *n = (glbmap_entry_t *) calloc(1,sizeof(glbmap_entry_t));
//      n->m_next = g_glbmap;
//      g_glbmap = n;
//
//      // initialize entry
//      n->m_bufferObj = bufferObj;
//      n->m_devPtr = *devPtr;
//      n->m_size = buffer_size;
//
//      p = n;
//   } else {
//      buffer_size = p->m_size;
//      *devPtr = p->m_devPtr;
//   }
//
//   if ( *devPtr  ) {
//      char *data = (char *) calloc(p->m_size,1);
//      glGetBufferSubData(GL_ARRAY_BUFFER,0,buffer_size,data);
//      memcpy_to_gpu( (size_t) *devPtr, data, buffer_size );
//      free(data);
//      printf("GPGPU-Sim PTX: cudaGLMapBufferObject %zu bytes starting at 0x%llx..\n", (size_t)buffer_size,
//             (unsigned long long) *devPtr);
//      return g_last_cudaError = cudaSuccess;
//   } else {
//      return g_last_cudaError = cudaErrorMemoryAllocation;
//   }
//
//   return g_last_cudaError = cudaSuccess;
//#else
//   fflush(stdout);
//   fflush(stderr);
//   printf("GPGPU-Sim PTX: GPGPU-Sim support for OpenGL integration disabled -- exiting\n");
//   fflush(stdout);
//   exit(50);
//#endif
}

cudaError_t gem5cudaGLUnmapBufferObject(GLuint bufferObj)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
//#ifdef OPENGL_SUPPORT
//   glbmap_entry_t *p = g_glbmap;
//   while ( p && p->m_bufferObj != bufferObj )
//      p = p->m_next;
//   if ( p == NULL )
//      return g_last_cudaError = cudaErrorUnknown;
//
//   char *data = (char *) calloc(p->m_size,1);
//   memcpy_from_gpu( data,(size_t)p->m_devPtr,p->m_size );
//   glBufferSubData(GL_ARRAY_BUFFER,0,p->m_size,data);
//   free(data);
//
//   return g_last_cudaError = cudaSuccess;
//#else
//   fflush(stdout);
//   fflush(stderr);
//   printf("GPGPU-Sim PTX: support for OpenGL integration disabled -- exiting\n");
//   fflush(stdout);
//   exit(50);
//#endif
}

cudaError_t gem5cudaGLUnregisterBufferObject(GLuint bufferObj)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
//   printf("GPGPU-Sim PTX: Execution warning: ignoring call to \"%s\"\n", __my_func__ );
//   return g_last_cudaError = cudaSuccess;
}

#if (CUDART_VERSION >= 2010)

cudaError_t  gem5cudaHostAlloc(void **pHost,  size_t bytes, unsigned int flags)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
//   *pHost = malloc(bytes);
//   if( *pHost )
//      return g_last_cudaError = cudaSuccess;
//   else
//      return g_last_cudaError = cudaErrorMemoryAllocation;
}

cudaError_t  gem5cudaHostGetDevicePointer(void **pDevice, void *pHost, unsigned int flags)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
}

cudaError_t  gem5cudaSetValidDevices(int *device_arr, int len)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
}

#if (CUDART_VERSION >= 3020)
cudaError_t  gem5cudaSetDeviceFlags(unsigned int flags)
#else
cudaError_t  gem5cudaSetDeviceFlags(int flags)
#endif
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
}

cudaError_t  gem5cudaFuncGetAttributes(struct cudaFuncAttributes *attr, const char *hostFun )
{
    gpusyscall_t call_params;
    call_params.num_args = 2;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(struct cudaFuncAttributes*);
    call_params.arg_lengths[1] = sizeof(const char*);
    call_params.total_bytes = call_params.arg_lengths[0] + call_params.arg_lengths[1];

    call_params.args = new char[call_params.total_bytes];

    call_params.ret = new char[sizeof(cudaError_t)];
    cudaError_t* ret_spot = (cudaError_t*)call_params.ret;
    *ret_spot = cudaSuccess;

    int bytes_off = 0;
    int lengths_off = 0;

    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&attr, call_params.arg_lengths[0]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&hostFun, call_params.arg_lengths[1]);

    m5_gpu(72, (uint64_t)&call_params);
    cudaError_t ret = *((cudaError_t*)call_params.ret);

    delete call_params.args;
    delete call_params.arg_lengths;
    delete call_params.ret;

    return ret;
}

#if (CUDART_VERSION >= 3020)
cudaError_t  gem5cudaEventCreateWithFlags(cudaEvent_t *event, unsigned int flags)
#else
cudaError_t  gem5cudaEventCreateWithFlags(cudaEvent_t *event, int flags)
#endif
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
//   CUevent_st *e = new CUevent_st(flags==cudaEventBlockingSync);
// TODO: Convert to make call into gem5-gpu
//   g_timer_events[e->get_uid()] = e;
//#if CUDART_VERSION >= 3000
//   *event = e;
//#else
//   *event = e->get_uid();
//#endif
//   return g_last_cudaError = cudaSuccess;
}

cudaError_t  gem5cudaDriverGetVersion(int *driverVersion)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
//   *driverVersion = CUDART_VERSION;
//   return g_last_cudaError = cudaErrorUnknown;
}

cudaError_t  gem5cudaRuntimeGetVersion(int *runtimeVersion)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
//   *runtimeVersion = CUDART_VERSION;
//   return g_last_cudaError = cudaErrorUnknown;
}

#endif

cudaError_t  gem5cudaGLSetGLDevice(int device)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
//   printf("GPGPU-Sim PTX: Execution warning: ignoring call to \"%s\"\n", __my_func__ );
//   return g_last_cudaError = cudaErrorUnknown;
}

typedef void* HGPUNV;

cudaError_t  gem5cudaWGLGetDevice(int *device, HGPUNV hGpu)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
}

/*
void  __cudaMutexOperation(int lock)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
}

void   __cudaTextureFetch(const void *tex, void *index, int integer, void *val)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
}
*/
cudaError_t  gem5gpu_finish_kernel() {
    return g_last_cudaError = cudaErrorUnknown;
}

cudaError_t  gem5gpu_stop_all_running_kernels() {
    return g_last_cudaError = cudaErrorUnknown;
}

cudaError_t  gem5gpu_print_stats() {
    return g_last_cudaError = cudaErrorUnknown;
}



bool  gem5gpu_active()
{
    gpusyscall_t call_params;
    call_params.num_args = 0;
    call_params.total_bytes = 0;
    call_params.ret = new char[sizeof(bool)];
    bool* ret_spot = (bool*)call_params.ret;
    *ret_spot = false;

    // gem5gpu_active
    m5_gpu(87, (uint64_t)&call_params);
    bool ret = *((bool*)call_params.ret);

    delete call_params.ret;

    return ret;
}

int gem5gpu_system_call(const char *command)
{
    gpusyscall_t call_params;
    call_params.num_args = 1;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(const char*);
    call_params.total_bytes = call_params.arg_lengths[0];

    call_params.args = new char[call_params.total_bytes];
    call_params.ret = new char[sizeof(bool)];

    bool* ret_spot = (bool*)call_params.ret;
    *ret_spot = false;

    int bytes_off = 0;
    int lengths_off = 0;
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&command, call_params.arg_lengths[0]);

    // gem5gpu_active
    m5_gpu(88, (uint64_t)&call_params);

    int ret = *((int*)call_params.ret);

    // delete[] call_params.args;
    // delete[] call_params.arg_lengths;
    // delete call_params.ret;

    return ret;
}

void gem5gpu_extract_ptx_files_using_cuobjdump(const char *ptx_list_file_name)
{
    printf("gem5gpu_cuobjdumpParseBinary is not used now\n");
    assert(false);
    gpusyscall_t call_params;
    call_params.num_args = 1;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(const char*);
    call_params.total_bytes = call_params.arg_lengths[0];

    call_params.args = new char[call_params.total_bytes];
    call_params.ret = new char[sizeof(bool)];

    bool* ret_spot = (bool*)call_params.ret;
    *ret_spot = false;

    int bytes_off = 0;
    int lengths_off = 0;
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&ptx_list_file_name, call_params.arg_lengths[0]);

    // sysgem5gpu_extract_ptx_files_using_cuobjdump
    m5_gpu(89, (uint64_t)&call_params);

    // int ret = *((int*)call_params.ret);

    delete[] call_params.args;
    delete[] call_params.arg_lengths;
    delete call_params.ret;

    // return ret;
}

#if 0
symbol_table* gem5gpu_cuobjdumpParseBinary(const char *ptxfile, unsigned  int handle)
{
    gpusyscall_t call_params;
    call_params.num_args = 2;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(const char*);
    call_params.arg_lengths[1] = sizeof(unsigned int);
    call_params.total_bytes = call_params.arg_lengths[0] +
                              call_params.arg_lengths[1];

    call_params.args = new char[call_params.total_bytes];
    call_params.ret = new char[sizeof(void*)];

    bool* ret_spot = (bool*)call_params.ret;
    *ret_spot = false;

    int bytes_off = 0;
    int lengths_off = 0;
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&ptxfile, call_params.arg_lengths[0]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&handle, call_params.arg_lengths[1]);

    // gem5gpu_cuobjdumpParseBinary
    m5_gpu(90, (uint64_t)&call_params);

    symbol_table* ret = *((symbol_table**)call_params.ret);

    delete[] call_params.args;
    delete[] call_params.arg_lengths;
    delete call_params.ret;

    return ret;
}

symbol* gem5gpu_symbol_lookup(symbol_table *symtab, const char* name)
{
    gpusyscall_t call_params;
    call_params.num_args = 2;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(symbol_table*);
    call_params.arg_lengths[1] = sizeof(const char*);
    call_params.total_bytes = call_params.arg_lengths[0] +
                              call_params.arg_lengths[1];

    call_params.args = new char[call_params.total_bytes];
    call_params.ret = new char[sizeof(symbol*)];

    bool* ret_spot = (bool*)call_params.ret;
    *ret_spot = false;

    int bytes_off = 0;
    int lengths_off = 0;
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&symtab, call_params.arg_lengths[0]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&name, call_params.arg_lengths[1]);

    // gem5gpu_symbol_lookup
    m5_gpu(91, (uint64_t)&call_params);

    symbol* ret = *((symbol**)call_params.ret);

    delete[] call_params.args;
    delete[] call_params.arg_lengths;
    delete call_params.ret;

    return ret;
}

function_info* gem5gpu_symbol_get_function(symbol_table *symtab, const char* name)
{
    gpusyscall_t call_params;
    call_params.num_args = 2;
    call_params.arg_lengths = new int[call_params.num_args];

    call_params.arg_lengths[0] = sizeof(symbol_table*);
    call_params.arg_lengths[1] = sizeof(const char*);
    call_params.total_bytes = call_params.arg_lengths[0] +
                              call_params.arg_lengths[1];

    call_params.args = new char[call_params.total_bytes];
    call_params.ret = new char[sizeof(function_info*)];

    bool* ret_spot = (bool*)call_params.ret;
    *ret_spot = false;

    int bytes_off = 0;
    int lengths_off = 0;
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&symtab, call_params.arg_lengths[0]);
    pack(call_params.args, bytes_off, call_params.arg_lengths, lengths_off, (char *)&name, call_params.arg_lengths[1]);

    // gem5gpu_symbol_lookup
    m5_gpu(92, (uint64_t)&call_params);

    function_info* ret = *((function_info**)call_params.ret);

    delete[] call_params.args;
    delete[] call_params.arg_lengths;
    delete call_params.ret;

    return ret;
}
#endif


bool  gem5gpu_cycle_insn_cta_max_hit()
{
    /*
    gpusyscall_t call_params;
    call_params.num_args = 0;
    call_params.total_bytes = 0;
    call_params.ret = new char[sizeof(bool)];
    bool* ret_spot = (bool*)call_params.ret;
    *ret_spot = false;

    // gem5gpu_cycle_inst_cta_max_hit
    m5_gpu(88, (uint64_t)&call_params);
    bool ret = *((bool*)call_params.ret);

    delete call_params.ret;

    return ret;
    */
    return false;
}


}  // extern "C"
/*
namespace cuda_math {

void  __cudaMutexOperation(int lock)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
}

void   __cudaTextureFetch(const void *tex, void *index, int integer, void *val)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
}

int  __cudaSynchronizeThreads(void**, void*)
{
    cuda_not_implemented(__FILE__, __my_func__, __LINE__);
    return g_last_cudaError = cudaErrorUnknown;
//   //TODO This function should syncronize if we support Asyn kernel calls
//   return g_last_cudaError = cudaSuccess;
}

}
*/


