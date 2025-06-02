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

#ifndef __GEM5_SYSCALL_UTIL_H__
#define __GEM5_SYSCALL_UTIL_H__

/*******************************************************************************
 *
 * Variable declarations used in the CUDA runtime
 *
 ******************************************************************************/

enum { CUDA_MALLOC_DEVICE = 0,
       CUDA_MALLOC_HOST = 1,
       CUDA_FREE_DEVICE = 4,
       CUDA_FREE_HOST = 5
};

// const char* cudaErrorStrings;


/*******************************************************************************
 *
 * Functionality for debugging the gem5-gpu CUDA runtime library
 *
 ******************************************************************************/

#ifdef __DEBUG__
#define DPRINTF(...) do { fprintf(stderr, "gem5-gpu CUDA Syscalls: "); fprintf(stderr, __VA_ARGS__); } while(0);
#else
#define DPRINTF(...) do {} while(0);
#endif

#if defined __APPLE__
#   define __my_func__ __PRETTY_FUNCTION__
#else
# if defined __cplusplus ? __GNUC_PREREQ (2, 6) : __GNUC_PREREQ (2, 4)
#   define __my_func__ __PRETTY_FUNCTION__
# else
#  if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#   define __my_func__ __func__
#  else
#   define __my_func__ ((__const char *) 0)
#  endif
# endif
#endif

void cuda_not_implemented(const char* file, const char* func, unsigned line);


/*******************************************************************************
 *
 * Memory handling functionality within the CUDA runtime library
 *
 ******************************************************************************/

#define CACHE_BLOCK_SIZE_BYTES 128
#define PAGE_SIZE_BYTES 4096

unsigned char touchPages(unsigned char *ptr, size_t size);

void *checkedAlignedAlloc(size_t size, size_t align_gran = CACHE_BLOCK_SIZE_BYTES);

extern const char* cudaErrorStrings[];

/*******************************************************************************
 *
 * Functionality to communicate between the benchmark CUDA runtime and gem5-gpu
 *
 ******************************************************************************/

/*
 * A helper struct to communicate with gem5-gpu. The variables are used as
 * follows so that gem5-gpu knows how to access these data in the memory of
 * the simulated system:
 *  - total_bytes: The total number of bytes that comprise the arguments to
 *                 be communicated to gem5-gpu. This aids in pulling the args
 *                 array into gem5-gpu memory.
 *  - num_args: The number of arguments packed to be communicated to gem5-gpu.
 *  - arg_lengths: An array of the size of each argument to the gem5-gpu CUDA
 *                 call.
 *  - args: An array of the actual argument values.
 *  - ret: An array to hold the return value from the gem5-gpu CUDA call.
 */
typedef struct gpucall {
    int total_bytes;
    int num_args;
    int* arg_lengths;
    char* args;
    char* ret;
} gpusyscall_t;

/*
 * A function to pack data into arrays of the gpusyscall_t for communication
 * with gem5-gpu
 */
void pack(char *bytes, int &bytes_off, int *lengths, int &lengths_off, char *arg, int arg_size);

#endif // __GEM5_SYSCALL_UTIL_H__
