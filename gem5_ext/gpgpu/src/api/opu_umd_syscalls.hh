/*
 * Copyright (c) 2011 Mark D. Hill and David A. Wood
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

#ifndef __SIM_GPGPU_SYSCALLS_HH__
#define __SIM_GPGPU_SYSCALLS_HH__

#include "api/gpu_syscall_helper.hh"
#include "sim/syscall_emul.hh"

/*******************************
       OPU UMD API MEMBERS
********************************/
#if !defined(__DRIVER_TYPES_H__)
#include "driver_types.h"
#endif
/*
enum opuError
{
    opuSuccess                           =      0,   // No errors
    opuErrorMissingConfiguration         =      1,   // Missing configuration error
    opuErrorMemoryAllocation             =      2,   // Memory allocation error
    opuErrorInitializationError          =      3,   // Initialization error
    opuErrorLaunchFailure                =      4,   // Launch failure
    opuErrorPriorLaunchFailure           =      5,   // Prior launch failure
    opuErrorLaunchTimeout                =      6,   // Launch timeout error
    opuErrorLaunchOutOfResources         =      7,   // Launch out of resources error
    opuErrorInvalidDeviceFunction        =      8,   // Invalid device function
    opuErrorInvalidConfiguration         =      9,   // Invalid configuration
    opuErrorInvalidDevice                =     10,   // Invalid device
    opuErrorInvalidValue                 =     11,   // Invalid value
    opuErrorInvalidPitchValue            =     12,   // Invalid pitch value
    opuErrorInvalidSymbol                =     13,   // Invalid symbol
    opuErrorMapBufferObjectFailed        =     14,   // Map buffer object failed
    opuErrorUnmapBufferObjectFailed      =     15,   // Unmap buffer object failed
    opuErrorInvalidHostPointer           =     16,   // Invalid host pointer
    opuErrorInvalidDevicePointer         =     17,   // Invalid device pointer
    opuErrorInvalidTexture               =     18,   // Invalid texture
    opuErrorInvalidTextureBinding        =     19,   // Invalid texture binding
    opuErrorInvalidChannelDescriptor     =     20,   // Invalid channel descriptor
    opuErrorInvalidMemcpyDirection       =     21,   // Invalid memcpy direction
    opuErrorAddressOfConstant            =     22,   // Address of constant error
                                                      // \deprecated
                                                      // This error return is deprecated as of
                                                      // opu 3.1. Variables in constant memory
                                                      // may now have their address taken by the
                                                      // runtime via ::opuGetSymbolAddress().
    opuErrorTextureFetchFailed           =     23,   // Texture fetch failed
    opuErrorTextureNotBound              =     24,   // Texture not bound error
    opuErrorSynchronizationError         =     25,   // Synchronization error
    opuErrorInvalidFilterSetting         =     26,   // Invalid filter setting
    opuErrorInvalidNormSetting           =     27,   // Invalid norm setting
    opuErrorMixedDeviceExecution         =     28,   // Mixed device execution
    opuErroropurtUnloading              =     29,   // opu runtime unloading
    opuErrorUnknown                      =     30,   // Unknown error condition
    opuErrorNotYetImplemented            =     31,   // Function not yet implemented
    opuErrorMemoryValueTooLarge          =     32,   // Memory value too large
    opuErrorInvalidResourceHandle        =     33,   // Invalid resource handle
    opuErrorNotReady                     =     34,   // Not ready error
    opuErrorInsufficientDriver           =     35,   // opu runtime is newer than driver
    opuErrorSetOnActiveProcess           =     36,   // Set on active process error
    opuErrorInvalidSurface               =     37,   // Invalid surface
    opuErrorNoDevice                     =     38,   // No opu-capable devices detected
    opuErrorECCUncorrectable             =     39,   // Uncorrectable ECC error detected
    opuErrorSharedObjectSymbolNotFound   =     40,   // Link to a shared object failed to resolve
    opuErrorSharedObjectInitFailed       =     41,   // Shared object initialization failed
    opuErrorUnsupportedLimit             =     42,   // ::opuLimit not supported by device
    opuErrorDuplicateVariableName        =     43,   // Duplicate global variable lookup by string name
    opuErrorDuplicateTextureName         =     44,   // Duplicate texture lookup by string name
    opuErrorDuplicateSurfaceName         =     45,   // Duplicate surface lookup by string name
    opuErrorDevicesUnavailable           =     46,   // All opu-capable devices are busy (see ::opuComputeMode) or unavailable
    opuErrorStartupFailure               =   0x7f,   // Startup failure
    opuErrorApiFailureBase               =  10000    // API failure base
};

typedef enum opuError opuError_t;
*/

enum opuMemcpyKind
{
    opuMemcpyHostToHost          =   0,      // Host   -> Host
    opuMemcpyHostToDevice        =   1,      // Host   -> Device
    opuMemcpyDeviceToHost        =   2,      // Device -> Host
    opuMemcpyDeviceToDevice      =   3       // Device -> Device
};

const char* opuMemcpyKindStrings[] =
{
    "opuMemcpyHostToHost",
    "opuMemcpyHostToDevice",
    "opuMemcpyDeviceToHost",
    "opuMemcpyDeviceToDevice"
};


/*DEVICE_BUILTIN*/
/*
struct uint3
{
  unsigned int x, y, z;
};
*/

typedef struct CUevent_st *opuEvent_t;

typedef struct OpuFuncAttributes {
   size_t sharedSizeBytes;
   size_t constSizeBytes;
   size_t localSizeBytes;
   int maxThreadsPerBlock;
   int numRegs;
   int ptxVersion;
   int binaryVersion;
   int __opuReserved[6];
} OpuFuncAttributes;

/*******************************
     opu API GEM5 HANDLERS
********************************/

void libopuMalloc(ThreadContext *tc, gpusyscall_t *call_params);
void libopuMallocHost(ThreadContext *tc, gpusyscall_t *call_params);
void libopuRegisterDeviceMemory(ThreadContext *tc, gpusyscall_t *call_params);
void opuMallocPitch(ThreadContext *tc, gpusyscall_t *call_params);
void opuMallocArray(ThreadContext *tc, gpusyscall_t *call_params);
void libopuFree(ThreadContext *tc, gpusyscall_t *call_params);
void libopuFreeHost(ThreadContext *tc, gpusyscall_t *call_params);
void opuFreeArray(ThreadContext *tc, gpusyscall_t *call_params);
void libopuMemcpy(ThreadContext *tc, gpusyscall_t *call_params);
void opuMemcpyToArray(ThreadContext *tc, gpusyscall_t *call_params);
void opuMemcpyFromArray(ThreadContext *tc, gpusyscall_t *call_params);
void opuMemcpyArrayToArray(ThreadContext *tc, gpusyscall_t *call_params);
void opuMemcpy2D(ThreadContext *tc, gpusyscall_t *call_params);
void opuMemcpy2DToArray(ThreadContext *tc, gpusyscall_t *call_params);
void opuMemcpy2DFromArray(ThreadContext *tc, gpusyscall_t *call_params);
void opuMemcpy2DArrayToArray(ThreadContext *tc, gpusyscall_t *call_params);
void libopuMemcpyToSymbol(ThreadContext *tc, gpusyscall_t *call_params);
void libopuMemcpyFromSymbol(ThreadContext *tc, gpusyscall_t *call_params);
void opuMemcpyAsync(ThreadContext *tc, gpusyscall_t *call_params);
void opuMemcpyToArrayAsync(ThreadContext *tc, gpusyscall_t *call_params);
void opuMemcpyFromArrayAsync(ThreadContext *tc, gpusyscall_t *call_params);
void opuMemcpy2DAsync(ThreadContext *tc, gpusyscall_t *call_params);
void opuMemcpy2DToArrayAsync(ThreadContext *tc, gpusyscall_t *call_params);
void opuMemcpy2DFromArrayAsync(ThreadContext *tc, gpusyscall_t *call_params);
void opuBlockThread(ThreadContext *tc, gpusyscall_t *call_params);
void libopuMemset(ThreadContext *tc, gpusyscall_t *call_params);
void opuMemset2D(ThreadContext *tc, gpusyscall_t *call_params);
void opuGetSymbolAddress(ThreadContext *tc, gpusyscall_t *call_params);
void opuGetSymbolSize(ThreadContext *tc, gpusyscall_t *call_params);
void libopuGetDeviceCount(ThreadContext *tc, gpusyscall_t *call_params);
void libopuGetDeviceProperties(ThreadContext *tc, gpusyscall_t *call_params);
void opuChooseDevice(ThreadContext *tc, gpusyscall_t *call_params);
void libopuSetDevice(ThreadContext *tc, gpusyscall_t *call_params);
void libopuGetDevice(ThreadContext *tc, gpusyscall_t *call_params);
void opuBindTexture(ThreadContext *tc, gpusyscall_t *call_params);
void opuBindTextureToArray(ThreadContext *tc, gpusyscall_t *call_params);
void opuUnbindTexture(ThreadContext *tc, gpusyscall_t *call_params);
void opuGetTextureAlignmentOffset(ThreadContext *tc, gpusyscall_t *call_params);
void opuGetTextureReference(ThreadContext *tc, gpusyscall_t *call_params);
void opuGetChannelDesc(ThreadContext *tc, gpusyscall_t *call_params);
void opuCreateChannelDesc(ThreadContext *tc, gpusyscall_t *call_params);
void opuGetLastError(ThreadContext *tc, gpusyscall_t *call_params);
void opuGetErrorString(ThreadContext *tc, gpusyscall_t *call_params);
void libgem5opuConfigureCall(ThreadContext *tc, gpusyscall_t *call_params);
void libgem5opuSetupArgument(ThreadContext *tc, gpusyscall_t *call_params);
void libgem5opuLaunch(ThreadContext *tc, gpusyscall_t *call_params);
void opuStreamCreate(ThreadContext *tc, gpusyscall_t *call_params);
void opuStreamDestroy(ThreadContext *tc, gpusyscall_t *call_params);
void opuStreamSynchronize(ThreadContext *tc, gpusyscall_t *call_params);
void opuStreamQuery(ThreadContext *tc, gpusyscall_t *call_params);
void opuEventCreate(ThreadContext *tc, gpusyscall_t *call_params);
void opuEventRecord(ThreadContext *tc, gpusyscall_t *call_params);
void opuEventQuery(ThreadContext *tc, gpusyscall_t *call_params);
void opuEventSynchronize(ThreadContext *tc, gpusyscall_t *call_params);
void opuEventDestroy(ThreadContext *tc, gpusyscall_t *call_params);
void opuEventElapsedTime(ThreadContext *tc, gpusyscall_t *call_params);
void opuThreadExit(ThreadContext *tc, gpusyscall_t *call_params);
void opuThreadSynchronize(ThreadContext *tc, gpusyscall_t *call_params);
void __opuSynchronizeThreads(ThreadContext *tc, gpusyscall_t *call_params);
void libgem5opuRegisterFatBinary(ThreadContext *tc, gpusyscall_t *call_params);
// void libgem5opuRegisterPtxInfo(ThreadContext *tc, gpusyscall_t *call_params);
// void sysgem5gpu_active(ThreadContext *tc, gpusyscall_t *call_params);
// void sysgem5gpu_system_call(ThreadContext *tc, gpusyscall_t *call_params);
void libopuRegisterFatBinaryFinalize(ThreadContext *tc, gpusyscall_t *call_params);
void __opuCheckAllocateLocal(ThreadContext *tc, gpusyscall_t *call_params);
void __opuSetLocalAllocation(ThreadContext *tc, gpusyscall_t *call_params);
void libopuUnregisterFatBinary(ThreadContext *tc, gpusyscall_t *call_params);
void libopuRegisterFunction(ThreadContext *tc, gpusyscall_t *call_params);
void libopuRegisterVar(ThreadContext *tc, gpusyscall_t *call_params);
void __opuRegisterShared(ThreadContext *tc, gpusyscall_t *call_params);
void __opuRegisterSharedVar(ThreadContext *tc, gpusyscall_t *call_params);
void libopuRegisterTexture(ThreadContext *tc, gpusyscall_t *call_params);
void opuGLRegisterBufferObject(ThreadContext *tc, gpusyscall_t *call_params);
void opuGLMapBufferObject(ThreadContext *tc, gpusyscall_t *call_params);
void opuGLUnmapBufferObject(ThreadContext *tc, gpusyscall_t *call_params);
void opuGLUnregisterBufferObject(ThreadContext *tc, gpusyscall_t *call_params);

// void sysgem5gpu_extract_ptx_files_using_cuobjdump(ThreadContext *tc, gpusyscall_t *call_params);
// void sysgem5gpu_cuobjdumpParseBinary(ThreadContext *tc, gpusyscall_t *call_params);
// void sysgem5gpu_symbol_lookup(ThreadContext *tc, gpusyscall_t *call_params);
// void sysgem5gpu_symbol_get_function(ThreadContext *tc, gpusyscall_t *call_params);


void opuHostAlloc(ThreadContext *tc, gpusyscall_t *call_params);
void opuHostGetDevicePointer(ThreadContext *tc, gpusyscall_t *call_params);
void opuSetValidDevices(ThreadContext *tc, gpusyscall_t *call_params);
void opuSetDeviceFlags(ThreadContext *tc, gpusyscall_t *call_params);
void libopuFuncGetAttributes(ThreadContext *tc, gpusyscall_t *call_params);
void opuEventCreateWithFlags(ThreadContext *tc, gpusyscall_t *call_params);
void opuDriverGetVersion(ThreadContext *tc, gpusyscall_t *call_params);
void opuRuntimeGetVersion(ThreadContext *tc, gpusyscall_t *call_params);


void opuGLSetGLDevice(ThreadContext *tc, gpusyscall_t *call_params);
void opuWGLGetDevice(ThreadContext *tc, gpusyscall_t *call_params);
void __opuMutexOperation(ThreadContext *tc, gpusyscall_t *call_params);
void __opuTextureFetch(ThreadContext *tc, gpusyscall_t *call_params);

namespace opu_math {
    uint64_t __opuMutexOperation(ThreadContext *tc, gpusyscall_t *call_params);
    uint64_t __opuTextureFetch(ThreadContext *tc, gpusyscall_t *call_params);
    uint64_t __opuSynchronizeThreads(ThreadContext *tc, gpusyscall_t *call_params);
    void  __opuTextureFetch(const void *tex, void *index, int integer, void *val);
    void __opuMutexOperation(int lock);
}

typedef void (*opuFunc_t)(ThreadContext *, gpusyscall_t *);

opuFunc_t opu_umd_funcs[] = {
        libopuMalloc,             /* 0 */
        libopuMallocHost,         /* 1 */
        opuMallocPitch,        /* 2 */
        opuMallocArray,        /* 3 */
        libopuFree,               /* 4 */
        libopuFreeHost,           /* 5 */
        opuFreeArray,          /* 6 */
        libopuMemcpy,             /* 7 */
        opuMemcpyToArray,      /* 8 */
        opuMemcpyFromArray,   /* 9 */
        opuMemcpyArrayToArray,/* 10 */
        opuMemcpy2D,          /* 11 */
        opuMemcpy2DToArray,   /* 12 */
        opuMemcpy2DFromArray, /* 13 */
        opuMemcpy2DArrayToArray,/* 14 */
        libopuMemcpyToSymbol,    /* 15 */
        libopuMemcpyFromSymbol,  /* 16 */
        opuMemcpyAsync,       /* 17 */
        opuMemcpyToArrayAsync,/* 18 */
        opuMemcpyFromArrayAsync,/* 19 */
        opuMemcpy2DAsync,     /* 20 */
        opuMemcpy2DToArrayAsync,/* 21 */
        opuMemcpy2DFromArrayAsync,/* 22 */
        libopuMemset,            /* 23 */
        opuMemset2D,          /* 24 */
        opuGetSymbolAddress,  /* 25 */
        opuGetSymbolSize,     /* 26 */
        libopuGetDeviceCount,    /* 27 */
        libopuGetDeviceProperties,/* 28 */
        opuChooseDevice,      /* 29 */
        libopuSetDevice,         /* 30 */
        libopuGetDevice,         /* 31 */
        opuBindTexture,       /* 32 */
        opuBindTextureToArray,/* 33 */
        opuUnbindTexture,     /* 34 */
        opuGetTextureAlignmentOffset,/* 35 */
        opuGetTextureReference,/* 36 */
        opuGetChannelDesc,        /* 37 */
        opuCreateChannelDesc, /* 38 */
        opuGetLastError,        /* 39 */
        opuGetErrorString,        /* 40 */
        libgem5opuConfigureCall,        /* 41 */
        libgem5opuSetupArgument,        /* 42 */
        libgem5opuLaunch,        /* 43 */
        opuStreamCreate,        /* 44 */
        opuStreamDestroy,        /* 45 */
        opuStreamSynchronize,        /* 46 */
        opuStreamQuery,        /* 47 */
        opuEventCreate,        /* 48 */
        opuEventRecord,        /* 49 */
        opuEventQuery,        /* 50 */
        opuEventSynchronize,        /* 51 */
        opuEventDestroy,        /* 52 */
        opuEventElapsedTime,        /* 53 */
        opuThreadExit,        /* 54 */
        opuThreadSynchronize,        /* 55 */
        __opuSynchronizeThreads,    /* 56 */
        libgem5opuRegisterFatBinary,    /* 57 */
        libopuUnregisterFatBinary,   /* 58 */
        libopuRegisterFunction,        /* 59 */
        libopuRegisterVar,        /* 60 */
        __opuRegisterShared,        /* 61 */
        __opuRegisterSharedVar,        /* 62 */
        libopuRegisterTexture,        /* 63 */
        opuGLRegisterBufferObject,  /* 64 */
        opuGLMapBufferObject,        /* 65 */
        opuGLUnmapBufferObject,        /* 66 */
        opuGLUnregisterBufferObject,/* 67 */
        opuHostAlloc,        /* 68 */
        opuHostGetDevicePointer,        /* 69 */
        opuSetValidDevices,        /* 70 */
        opuSetDeviceFlags,        /* 71 */
        libopuFuncGetAttributes,        /* 72 */
        opuEventCreateWithFlags,        /* 73 */
        opuDriverGetVersion,        /* 74 */
        opuRuntimeGetVersion,        /* 75 */
        opuGLSetGLDevice,        /* 76 */
        opuWGLGetDevice,        /* 77 */
        __opuMutexOperation,        /* 78 */
        __opuTextureFetch,        /* 79 */
        __opuSynchronizeThreads,        /* 80 */
        libopuRegisterFatBinaryFinalize,    /* 81 */
        libopuRegisterDeviceMemory,    /* 82 */
        opuBlockThread,    /* 83 */
        __opuCheckAllocateLocal,    /* 84 */
        __opuSetLocalAllocation    /* 85 */
#if 0
        libgem5opuRegisterPtxInfo    /* 86 */
        sysgem5gpu_active,    /* 87 */
        sysgem5gpu_system_call,    /* 88 */
        sysgem5gpu_extract_ptx_files_using_cuobjdump,    /* 89 */
        sysgem5gpu_cuobjdumpParseBinary,    /* 90 */
        sysgem5gpu_symbol_lookup,        /* 91 */
        sysgem5gpu_symbol_get_function  /* 92 */
#endif
};

#endif


