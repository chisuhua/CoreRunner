// Copyright (c) 2009-2021, Tor M. Aamodt, Wilson W.L. Fung, Ali Bakhoda,
// George L. Yuan, Andrew Turner, Inderpreet Singh, Vijay Kandiah, Nikos Hardavellas
// The University of British Columbia, Northwestern University
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer;
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution;
// 3. Neither the names of The University of British Columbia, Northwestern 
//    University nor the names of their contributors may be used to
//    endorse or promote products derived from this software without specific
//    prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <float.h>
#include "./shader.h"
#include "./gpu-sim.h"
#include "./gpu-misc.h"
#include "./cuda-sim/cuda-sim.h"
/*
#include "addrdec.h"

#include "dram.h"
#include "stat-tool.h"
#include "gpu-misc.h"
#include "gpu-sim.h"
#include "../libcuda/cuda-sim/ptx_sim.h"
#include "../libcuda/cuda-sim/ptx-stats.h"
#include "../libcuda/cuda-sim/cuda-sim.h"
#include "mem_fetch.h"
#include "mem_latency_stat.h"
#include "visualizer.h"
#include "../statwrapper.h"
#include "icnt_wrapper.h"
#include <string.h>
#include <limits.h>
#include "traffic_breakdown.h"
#include "shader_trace.h"
*/

namespace libcuda {
#define PRIORITIZE_MSHR_OVER_WB 1
#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN(a,b) (((a)<(b))?(a):(b))

unsigned int shader_core_config::max_cta( const kernel_info_t &k ) const {
  unsigned threads_per_cta = k.threads_per_cta();
  const class function_info *kernel = k.entry();
  unsigned int padded_cta_size = threads_per_cta;
  if (padded_cta_size % warp_size)
    padded_cta_size = ((padded_cta_size / warp_size) + 1) * (warp_size);

  // Limit by n_threads/shader
  unsigned int result_thread = n_thread_per_shader / padded_cta_size;

  const struct gpgpu_ptx_sim_info *kernel_info = ptx_sim_kernel_info(kernel);

  // Limit by shmem/shader
  unsigned int result_shmem = (unsigned)-1;
  if (kernel_info->smem > 0)
    result_shmem = gpgpu_shmem_size / kernel_info->smem;

  // Limit by register count, rounded up to multiple of 4.
  unsigned int result_regs = (unsigned)-1;
  if (kernel_info->regs > 0)
    result_regs = gpgpu_shader_registers /
                  (padded_cta_size * ((kernel_info->regs + 3) & ~3));

  // Limit by CTA
  unsigned int result_cta = max_cta_per_core;

  unsigned result = result_thread;
  result = gs_min2(result, result_shmem);
  result = gs_min2(result, result_regs);
  result = gs_min2(result, result_cta);

  static const struct gpgpu_ptx_sim_info *last_kinfo = NULL;
  if (last_kinfo !=
      kernel_info) {  // Only print out stats if kernel_info struct changes
    last_kinfo = kernel_info;
    printf("GPGPU-Sim uArch: CTA/core = %u, limited by:", result);
    if (result == result_thread) printf(" threads");
    if (result == result_shmem) printf(" shmem");
    if (result == result_regs) printf(" regs");
    if (result == result_cta) printf(" cta_limit");
    printf("\n");
  }

  // gpu_max_cta_per_shader is limited by number of CTAs if not enough to keep
  // all cores busy
  if (k.num_blocks() < result * num_shader()) {
    result = k.num_blocks() / num_shader();
    if (k.num_blocks() % num_shader()) result++;
  }

  assert(result <= MAX_CTA_PER_SHADER);
  if (result < 1) {
    printf(
        "GPGPU-Sim uArch: ERROR ** Kernel requires more resources than shader "
        "has.\n");
    if (gpgpu_ignore_resources_limitation) {
      printf(
          "GPGPU-Sim uArch: gpgpu_ignore_resources_limitation is set, ignore "
          "the ERROR!\n");
      return 1;
    }
    abort();
  }
  return result;
}

simt_core_cluster::simt_core_cluster( class gpgpu_sim *gpu, 
                                    unsigned cluster_id, 
                                    const struct shader_core_config *config, 
                                    const struct memory_config *mem_config,
                                    shader_core_stats *stats, 
                                    class memory_stats_t *mstats )
{
  m_config = config;
  m_cta_issue_next_core=m_config->n_simt_cores_per_cluster-1; // this causes first launch to use hw cta 0
  m_cluster_id=cluster_id;
  m_gpu = gpu;
  m_stats = stats;
  m_memory_stats = mstats;
}

}
