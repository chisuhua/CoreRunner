/*
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdarg.h>
#include <dlfcn.h>
#include <unistd.h>
#include <stdint.h>

#include "gem5_syscall_util.h"

const char* cudaErrorStrings[] =
{
    "no error",                                     // 0
    "__global__ function call is not configured",   // 1
    "out of memory",                                // 2
    "initialization error",                         // 3
    "unspecified launch failure",                   // 4
    "unspecified launch failure in prior launch",   // 5
    "the launch timed out and was terminated",      // 6
    "too many resources requested for launch",      // 7
    "invalid device function ",                     // 8
    "invalid configuration argument",               // 9
    "invalid device ordinal",                       // 10
    "invalid argument",                             // 11
    "invalid pitch argument",                       // 12
    "invalid device symbol",                        // 13
    "mapping of buffer object failed",              // 14
    "unmapping of buffer object failed",            // 15
    "invalid host pointer",                         // 16
    "invalid device pointer",                       // 17
    "invalid texture reference",                    // 18
    "texture is not bound to a pointer",            // 19
    "invalid channel descriptor",                   // 20
    "invalid copy direction for memcpy",            // 21
    "unspecified driver error",                     // 22
    "fetch from texture failed",                    // 23
    "cannot fetch from a texture that is not bound", // 24
    "incorrect use of __syncthreads()",             // 25
    "linear filtering not supported for non-float type", // 26
    "read as normalized float not supported for 32-bit non float type", // 27
    "device emulation mode and device execution mode cannot be mixed", // 28
    "unload of CUDA runtime failed",                // 29
    "unknown error",                                // 30
    "feature is not yet implemented",               // 31
    "memory size or pointer value too large to fit in 32 bit", // 32
    "invalid resource handle",                      // 33
    "device not ready",                             // 34
    "CUDA driver version is insufficient for CUDA runtime version", // 35
    "setting the device when a process is active is not allowed", // 36
    "invalid surface reference",                    // 37
    "no CUDA-capable device is detected",           // 38
    "uncorrectable ECC error encountered",          // 39
    "shared object symbol not found",               // 40
    "shared object initialization failed",          // 41
    "limit is not supported on this architecture",  // 42
    "duplicate global variable looked up by string name", // 43
    "duplicate texture looked up by string name",   // 44
    "duplicate surface looked up by string name",   // 45
    "all CUDA-capable devices are busy or unavailable", // 46
};

void cuda_not_implemented(const char* file, const char* func, unsigned line)
{
   fflush(stdout);
   fflush(stderr);
   fprintf(stderr, "\n\nGPGPU-Sim PTX: Execution error: CUDA API function \"%s()\" has not been implemented yet.\n"
           "                 [$GEM5_GPU_BENCHMARKS/libcuda/%s around line %u]\n\n\n",
           func, file, line);
   fflush(stdout);
   abort();
}


/*******************************************************************************
 *
 * Memory handling functionality within the CUDA runtime library
 *
 ******************************************************************************/
unsigned char touchPages(unsigned char *ptr, size_t size)
{
    unsigned char sum = 0;
    for (unsigned i = 0; i < size; i += PAGE_SIZE_BYTES) {
        sum += ptr[i];
    }
    sum += ptr[size-1];
    return sum;
}

void *checkedAlignedAlloc(size_t size, size_t align_gran)
{
    void *to_return = NULL;
    int error = posix_memalign(&to_return, align_gran, size);
    if (error) {
        fprintf(stderr, "ERROR: allocation failed with code: %d, Exiting...\n", error);
        exit(-1);
    }
    return to_return;
}


/*******************************************************************************
 *
 * Functionality to communicate between the benchmark CUDA runtime and gem5-gpu
 *
 ******************************************************************************/

/*
 * A function to pack data into arrays of the gpusyscall_t for communication
 * with gem5-gpu
 */
void pack(char *bytes, int &bytes_off, int *lengths, int &lengths_off, char *arg, int arg_size)
{
    for (int i = 0; i < arg_size; i++) {
        bytes[bytes_off + i] = *arg;
        arg++;
    }
    *(lengths + lengths_off) = arg_size;

    bytes_off += arg_size;
    lengths_off += 1;
}
