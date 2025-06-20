// Copyright (c) 2009-2011, Tor M. Aamodt, Tayler Hetherington, Ahmed ElTantawy,
// The University of British Columbia
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// Redistributions in binary form must reproduce the above copyright notice, this
// list of conditions and the following disclaimer in the documentation and/or
// other materials provided with the distribution.
// Neither the name of The University of British Columbia nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef GPGPU_SIM_WRAPPER_H_
#define GPGPU_SIM_WRAPPER_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include "processor.h"

using namespace std;

template <typename T>
struct avg_max_min_counters {
  T avg;
  T max;
  T min;

  avg_max_min_counters() {
    avg = 0;
    max = 0;
    min = 0;
  }
};

#ifndef COEFF_STRUCT
#define COEFF_STRUCT

struct PowerscalingCoefficients{
    double int_coeff;
    double int_mul_coeff;
    double int_mul24_coeff;
    double int_mul32_coeff;
    double int_div_coeff;
    double fp_coeff;
    double dp_coeff;
    double fp_mul_coeff;
    double fp_div_coeff;
    double dp_mul_coeff;
    double dp_div_coeff;
    double sqrt_coeff;
    double log_coeff;
    double sin_coeff;
    double exp_coeff;
    double tensor_coeff;
    double tex_coeff;
};

#endif

class gpgpu_sim_wrapper {
 public:
  gpgpu_sim_wrapper(bool power_simulation_enabled, char* xmlfile, int power_simulation_mode, bool dvfs_enabled);
  ~gpgpu_sim_wrapper();

  void init_mcpat(char* xmlfile, char* powerfile, char* power_trace_file,
                  char* metric_trace_file, char* steady_state_file,
                  bool power_sim_enabled, bool trace_enabled,
                  bool steady_state_enabled, bool power_per_cycle_dump,
                  double steady_power_deviation, double steady_min_period,
                  int zlevel, double init_val, int stat_sample_freq, int power_sim_mode, 
                  bool dvfs_enabled, unsigned clock_freq, unsigned num_shaders);
  void init_mcpat_hw_mode(unsigned gpu_sim_cycle);
  void detect_print_steady_state(int position, double init_val);
  void close_files();
  void open_files();
  void compute();
  void dump();
  void print_trace_files();
  void update_components_power();
  double calculate_static_power();
  void update_coefficients();
  void reset_counters();
  void print_power_kernel_stats(double gpu_sim_cycle, double gpu_tot_sim_cycle,
                                double init_value,
                                const std::string& kernel_info_string,
                                bool print_trace);
  void power_metrics_calculations();
  void set_model_voltage(double model_voltage);
  void set_inst_power(bool clk_gated_lanes, double tot_cycles,
                      double busy_cycles, double tot_inst, double int_inst,
                      double fp_inst, double load_inst, double store_inst,
                      double committed_inst);
  void set_regfile_power(double reads, double writes, double ops);
  void set_icache_power(double accesses, double misses);
  void set_ccache_power(double accesses, double misses);
  void set_tcache_power(double accesses, double misses);
  void set_shrd_mem_power(double accesses);
  void set_l1cache_power(double read_accesses, double read_misses,
                         double write_accesses, double write_misses);
  void set_l2cache_power(double read_accesses, double read_misses,
                         double write_accesses, double write_misses);
  void set_num_cores(double num_core);
  void set_idle_core_power(double num_idle_core);
  void set_duty_cycle_power(double duty_cycle);
  void set_mem_ctrl_power(double reads, double writes, double dram_precharge);
  void set_exec_unit_power(double fpu_accesses, double ialu_accesses,
                           double sfu_accesses);
  void set_int_accesses(double ialu_accesses, double imul24_accesses, 
                        double imul32_accesses, double imul_accesses, 
                        double idiv_accesses);
  void set_dp_accesses(double dpu_accesses, double dpmul_accesses, 
                       double dpdiv_accesses);
  void set_fp_accesses(double fpu_accesses, double fpmul_accesses, 
                       double fpdiv_accesses);
  void set_trans_accesses(double sqrt_accesses, double log_accesses, 
                       double sin_accesses, double exp_accesses);
  void set_tensor_accesses(double tensor_accesses);
  void set_tex_accesses(double tex_accesses);
  void set_avg_active_threads(float active_threads);
  void set_active_lanes_power(double sp_avg_active_lane,
                              double sfu_avg_active_lane);
  void set_NoC_power(double noc_tot_acc);
  bool sanity_check(double a, double b);

  PowerscalingCoefficients * get_scaling_coeffs();

 private:
  void print_steady_state(int position, double init_val);

  Processor* proc;
  ParseXML* p;
  // power parameters
  double const_dynamic_power;
  double avg_threads_per_warp_tot;
  double proc_power;
  double num_cores;
  double num_idle_cores;
  unsigned num_perf_counters;  // # of performance counters
  unsigned num_pwr_cmps;       // # of components modelled
  int kernel_sample_count;     // # of samples per kernel
  int total_sample_count;      // # of samples per benchmark

  std::vector<avg_max_min_counters<double> >
      kernel_cmp_pwr;  // Per-kernel component power avg/max/min values
  std::vector<avg_max_min_counters<double> >
      kernel_cmp_perf_counters;  // Per-kernel component avg/max/min performance
                                 // counters

  double kernel_tot_power;  // Total per-kernel power
  avg_max_min_counters<double>
      kernel_power;  // Per-kernel power avg/max/min values
  avg_max_min_counters<double>
      gpu_tot_power;  // Global GPU power avg/max/min values (across kernels)

  bool has_written_avg;

  std::vector<double> sample_cmp_pwr;  // Current sample component powers
  std::vector<double>
      sample_perf_counters;  // Current sample component perf. counts
  std::vector<double> initpower_coeff;
  std::vector<double> effpower_coeff;

  // For calculating steady-state average
  unsigned sample_start;
  double sample_val;
  double init_inst_val;
  double tot_sfu_accesses;
  double tot_fpu_accesses;
  double modeled_chip_voltage;
  unsigned avg_threads_per_warp;
  std::vector<double> samples;
  std::vector<double> samples_counter;
  std::vector<double> pwr_counter;

  char* xml_filename;
  char* g_power_filename;
  char* g_power_trace_filename;
  char* g_metric_trace_filename;
  char* g_steady_state_tracking_filename;
  bool g_power_simulation_enabled;
  int g_power_simulation_mode;
  bool g_dvfs_enabled;
  bool g_steady_power_levels_enabled;
  bool g_power_trace_enabled;
  bool g_power_per_cycle_dump;
  double gpu_steady_power_deviation;
  double gpu_steady_min_period;
  int g_power_trace_zlevel;
  double gpu_stat_sample_frequency;
  int gpu_stat_sample_freq;

  std::ofstream powerfile;
  gzFile power_trace_file;
  gzFile metric_trace_file;
  gzFile steady_state_tacking_file;
};
#endif /* GPGPU_SIM_WRAPPER_H_ */
