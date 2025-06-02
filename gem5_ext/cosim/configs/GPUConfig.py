# Copyright (c) 2012 Mark D. Hill and David A. Wood
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met: redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer;
# redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution;
# neither the name of the copyright holders nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Authors: Jason Power, Joel Hestness

import pdb
import m5
import os
import re
from m5.objects import *
from m5.util.convert import *
from m5.util import fatal

gpu_core_configs = ['Fermi', 'Maxwell', 'Volta', 'Gpu', 'Opu']
system_configs = ['cpu_only', 'gpgpu', 'opu_libgem5', 'opu_se', 'opu_fs']

def addGPUOptions(parser):
    parser.add_argument("--clusters", default=16, help="Number of shader core clusters in the gpu that GPGPU-sim is simulating", type=int)
    parser.add_argument("--cores_per_cluster", default=1, help="Number of shader cores per cluster in the gpu that GPGPU-sim is simulating", type=int)
    parser.add_argument("--ctas_per_shader", default=8, help="Number of simultaneous CTAs that can be scheduled to a single shader", type=int)
    parser.add_argument("--sc_l1_size", default="64kB", help="size of l1 cache hooked up to each sc")
    parser.add_argument("--sc_l2_size", default="1MB", help="size of L2 cache divided by num L2 caches")
    parser.add_argument("--sc_l1_assoc", default=4, help="associativity of l1 cache hooked up to each sc", type=int)
    parser.add_argument("--sc_l2_assoc", default=16, help="associativity of L2 cache backing SC L1's", type=int)
    parser.add_argument("--shMemDelay", default=1, help="delay to access shared memory in gpgpu-sim ticks", type=int)
    parser.add_argument("--gpu_core_config", type=str, choices=gpu_core_configs, default='Opu', help="configure the GPU cores like %s" % gpu_core_configs)
    parser.add_argument("--kernel_stats", default=False, action="store_true", help="Dump statistics on GPU kernel boundaries")
    parser.add_argument("--total-mem-size", default='8GB', help="Total size of memory in system")
    parser.add_argument("--gpu_l1_buf_depth", type=int, default=96, help="Number of buffered L1 requests per shader")
    parser.add_argument("--flush_kernel_end", default=False, action="store_true", help="Flush the L1s at the end of each kernel. (Only VI_hammer)")
    parser.add_argument("--gpu-core-clock", default='700MHz', help="The frequency of GPU clusters (note: shaders operate at double this frequency when modeling Fermi)")
    parser.add_argument("--access-host-pagetable", action="store_true", default=False)
    parser.add_argument("--split", default=False, action="store_true", help="Use split CPU and GPU cache hierarchies instead of fusion")
    parser.add_argument("--ppu", default=False, action="store_true", help="Use PPU system cache hierarchies instead of fusion")
    parser.add_argument("--system-config", type=str, choices=system_configs, default='opu_libgem5', help="Use PPU system cache hierarchies instead of fusion")
    parser.add_argument("--dev-numa-high-bit", type=int, default=0, help="High order address bit to use for device NUMA mapping.")
    parser.add_argument("--num-dev-dirs", default=1, help="In split hierarchies, number of device directories", type=int)
    parser.add_argument("--gpu-mem-size", default='2GB', help="In split hierarchies, amount of GPU memory")
    parser.add_argument("--gpu_mem_ctl_latency", type=int, default=-1, help="GPU memory controller latency in cycles")
    parser.add_argument("--gpu_mem_freq", type=str, default=None, help="GPU memory controller frequency")
    parser.add_argument("--gpu_membus_busy_cycles", type=int, default=-1, help="GPU memory bus busy cycles per data transfer")
    parser.add_argument("--gpu_membank_busy_time", type=str, default=None, help="GPU memory bank busy time in ns (CL+tRP+tRCD+CAS)")
    parser.add_argument("--gpu_warp_size", type=int, default=32, help="Number of threads per warp, also functional units per shader core/SM")
    parser.add_argument("--gpu_atoms_per_subline", type=int, default=None, help="Maximum atomic ops to send per subline per access")
    parser.add_argument("--gpu_threads_per_core", type=int, default=1536, help="Maximum number of threads per GPU core (SM)")
    parser.add_argument("--gpgpusim-config", type=str, default=None, help="Path to the gpgpusim.config to use. This overrides the gpgpusim.config template")
    parser.add_argument("--gpu-l2-resource-stalls", action="store_true", default=False)
    parser.add_argument("--gpu_tlb_entries", type=int, default=0, help="Number of entries in GPU TLB. 0 implies infinite")
    parser.add_argument("--gpu_tlb_assoc", type=int, default=0, help="Associativity of the L1 TLB. 0 implies infinite")
    parser.add_argument("--pwc_size", default="8kB", help="Capacity of the page walk cache")
    parser.add_argument("--ce_buffering", type=int, default=128, help="Maximum cache lines buffered in the GPU CE. 0 implies infinite")
    parser.add_argument("--cp_buffering", type=int, default=128, help="Maximum cache lines buffered in the GPU CE. 0 implies infinite")
    parser.add_argument("--cp_firmware", type=str, default=None, help="the zephyr firmware")

def configureMemorySpaces(options):
    total_mem_range = AddrRange(options.total_mem_size)
    cpu_mem_range = total_mem_range
    gpu_mem_range = total_mem_range

    #ipdb.set_trace()

    #if options.split:
    #    buildEnv['PROTOCOL'] +=  '_split'
    #    total_mem_size = total_mem_range.size()
    #    gpu_mem_range = AddrRange(options.gpu_mem_size)
    #    if gpu_mem_range.size() >= total_mem_size:
    #        fatal("GPU memory size (%s) won't fit within total memory size (%s)!" % (options.gpu_mem_size, options.total_mem_size))
    #    gpu_segment_base_addr = Addr(total_mem_size - gpu_mem_range.size())
    #    gpu_mem_range = AddrRange(gpu_segment_base_addr, size = options.gpu_mem_size)
    #    options.total_mem_size = long(gpu_segment_base_addr)
    #    cpu_mem_range = AddrRange(options.total_mem_size)
    #elif options.ppu:
    #    buildEnv['PROTOCOL'] +=  '_ppu'
    #    total_mem_size = total_mem_range.size()
    #    gpu_mem_range = AddrRange(options.gpu_mem_size)
    #    if gpu_mem_range.size() >= total_mem_size:
    #        fatal("GPU memory size (%s) won't fit within total memory size (%s)!" % (options.gpu_mem_size, options.total_mem_size))
    #    gpu_segment_base_addr = Addr(total_mem_size - gpu_mem_range.size())
    #    gpu_mem_range = AddrRange(gpu_segment_base_addr, size = options.gpu_mem_size)
    #    options.total_mem_size = long(gpu_segment_base_addr)
    #    cpu_mem_range = AddrRange(options.total_mem_size)
    #    options.num_dev_dirs = 1
    if options.system_config == 'cpu_only':
        buildEnv['PROTOCOL'] +=  ''
    elif options.system_config == 'opu_libgem5':
        buildEnv['PROTOCOL'] +=  '_fusion'
        total_mem_size = total_mem_range.size()
        gpu_mem_range = AddrRange(options.gpu_mem_size)
        if gpu_mem_range.size() >= total_mem_size:
            fatal("GPU memory size (%s) won't fit within total memory size (%s)!" % (options.gpu_mem_size, options.total_mem_size))
        gpu_segment_base_addr = Addr(total_mem_size - gpu_mem_range.size())
        gpu_mem_range = AddrRange(gpu_segment_base_addr, size = options.gpu_mem_size)
        options.total_mem_size = gpu_segment_base_addr
        options.mem_size = total_mem_size
        cpu_mem_range = AddrRange(options.total_mem_size)
    else:
        buildEnv['PROTOCOL'] +=  '_fusion'
    return (cpu_mem_range, gpu_mem_range, total_mem_range)

def parseGpgpusimConfig(options):
    # parse gpgpu config file
    # First check the cwd, and if there is not a gpgpusim.config file there
    # Use the template found in gem5-fusion/configs/gpu_config and fill in
    # the missing information with command line options.
    config_outfile = "gpgpusim.config"
    if options.gpgpusim_config:
        usingTemplate = False
        gpgpusimconfig = options.gpgpusim_config
    else:
        if options.gpu_core_config == 'Fermi':
            gpgpusimconfig = os.path.join(os.path.dirname(__file__), 'gpu_config/gpgpusim.fermi.config.template')
        elif options.gpu_core_config == 'Maxwell':
            gpgpusimconfig = os.path.join(os.path.dirname(__file__), 'gpu_config/gpgpusim.maxwell.config.template')
        elif options.gpu_core_config == 'Volta':
            gpgpusimconfig = os.path.join(os.path.dirname(__file__), 'gpu_config/gpgpusim.volta.config.template')
        elif options.gpu_core_config == 'Gpu':
            gpgpusimconfig = os.path.join(os.path.dirname(__file__), 'gpu_config/gpgpusim.gpu.config.template')
        elif options.gpu_core_config == 'Opu':
            gpgpusimconfig = os.path.join(os.path.dirname(__file__), 'gpu_config/gpgpusim.opu.config.template')
            # TODO I set Ppu cluster to 1 for easy debug
            options.clusters = 1
            config_outfile = "opusim.config"
        usingTemplate = True
    if not os.path.isfile(gpgpusimconfig):
        fatal("Unable to find gpgpusim config (%s)" % gpgpusimconfig)
    f = open(gpgpusimconfig, 'r')
    config = f.read()
    f.close()

    if usingTemplate:
        print("Using template and command line options for gpgpusim.config")

        # Modify the GPGPU-Sim configuration template
        config = config.replace("%clusters%", str(options.clusters))
        config = config.replace("%cores_per_cluster%", str(options.cores_per_cluster))
        config = config.replace("%ctas_per_shader%", str(options.ctas_per_shader))
        if options.gpu_core_config == 'Fermi':
            icnt_outfile = os.path.join(m5.options.outdir, 'config_fermi_islip.icnt')
        elif options.gpu_core_config == 'Maxwell':
            icnt_outfile = os.path.join(m5.options.outdir, 'config_maxwell_islip.icnt')
        elif options.gpu_core_config == 'Volta':
            icnt_outfile = os.path.join(m5.options.outdir, 'config_volta_islip.icnt')
        elif options.gpu_core_config == 'Gpu':
            icnt_outfile = os.path.join(m5.options.outdir, 'config_gpu_islip.icnt')
        elif options.gpu_core_config == 'Opu':
            icnt_outfile = os.path.join(m5.options.outdir, 'config_opu_islip.icnt')
        config = config.replace("%icnt_file%", icnt_outfile)
        config = config.replace("%warp_size%", str(options.gpu_warp_size))
        # GPGPU-Sim config expects freq in MHz
        config = config.replace("%freq%", str(toFrequency(options.gpu_core_clock) / 1.0e6))
        config = config.replace("%threads_per_sm%", str(options.gpu_threads_per_core))
        options.num_sc = options.clusters*options.cores_per_cluster

        # Write out the configuration file to the output directory
        f = open(m5.options.outdir + '/{}'.format(config_outfile), 'w')
        f.write(config)
        f.close()
        gpgpusimconfig = m5.options.outdir + '/{}'.format(config_outfile)

        # Read in and modify the interconnect config template
        if options.gpu_core_config == 'Fermi':
            icnt_template = os.path.join(os.path.dirname(__file__), 'gpu_config/config_fermi_islip.template.icnt')
        elif options.gpu_core_config == 'Maxwell':
            icnt_template = os.path.join(os.path.dirname(__file__), 'gpu_config/config_maxwell_islip.template.icnt')
        elif options.gpu_core_config == 'Volta':
            icnt_template = os.path.join(os.path.dirname(__file__), 'gpu_config/config_volta_islip.template.icnt')
        elif options.gpu_core_config == 'Gpu':
            icnt_template = os.path.join(os.path.dirname(__file__), 'gpu_config/config_gpu_islip.template.icnt')
        elif options.gpu_core_config == 'Opu':
            icnt_template = os.path.join(os.path.dirname(__file__), 'gpu_config/config_opu_islip.template.icnt')
        f = open(icnt_template)
        icnt_config = f.read()
        f.close()

        # The number of nodes in the GPU network is the number of core clusters,
        # plus the number of GPU memory partitions, plus one extra (it is not
        # clear in GPGPU-Sim what this extra is for). Note: Aiming to remove
        # GPGPU-Sim interconnect completely as it only models parameter memory
        # handling currently (i.e. tiny fraction of accesses). Only model one
        # memory partition currently by default.
        num_icnt_nodes = str(options.clusters + 1 + 1)
        icnt_config = icnt_config.replace("%num_nodes%", num_icnt_nodes)

        # Write out the interconnect config file to the output directory
        f = open(icnt_outfile, 'w')
        f.write(icnt_config)
        f.close()
    else:
        print("Using gpgpusim.config for clusters, cores_per_cluster, Frequency, warp size")
        config = re.sub(re.compile("#.*?\n"), "", config)
        start = config.find("-gpgpu_n_clusters ") + len("-gpgpu_n_clusters ")
        end = config.find('-', start)
        gpgpu_n_clusters = int(config[start:end])
        start = config.find("-gpgpu_n_cores_per_cluster ") + len("-gpgpu_n_cores_per_cluster ")
        end = config.find('-', start)
        gpgpu_n_cores_per_cluster = int(config[start:end])
        num_sc = gpgpu_n_clusters * gpgpu_n_cores_per_cluster
        options.num_sc = num_sc
        start = config.find("-gpgpu_clock_domains ") + len("-gpgpu_clock_domains ")
        end = config.find(':', start)
        options.gpu_core_clock = config[start:end] + "MHz"
        start = config.find('-gpgpu_shader_core_pipeline ') + len('-gpgpu_shader_core_pipeline ')
        start = config.find(':', start) + 1
        end = config.find('\n', start)
        options.gpu_warp_size = int(config[start:end])

    if options.pwc_size == "0":
        # Bypass the shared L1 cache
        options.gpu_tlb_bypass_l1 = True
    else:
        # Do not bypass the page walk cache
        options.gpu_tlb_bypass_l1 = False

    # DEPRECATED: Get the GPU DRAM clock from the config file to be passed to
    # the DRAM component wrapper. This should be removed at a later date!
    config = re.sub(re.compile("#.*?\n"), "", config)
    if options.gpu_core_config == 'Opu':
        start = config.find("-opu_clock_domains ")
        end = config.find('\n', start)
        clk_domains = config[start:end].split(':')
    else:
        start = config.find("-gpgpu_clock_domains ")
        end = config.find('\n', start)
        clk_domains = config[start:end].split(':')
        options.gpu_dram_clock = clk_domains[3] + "MHz"

    return gpgpusimconfig

def createGPU(options, gpu_mem_range, system):
    pdb.set_trace()
    # DEPRECATED: Set a default GPU DRAM clock to be passed to the wrapper.
    # This must be eliminated when the wrapper can be removed.
    options.gpu_dram_clock = None

    gpgpusimOptions = parseGpgpusimConfig(options)

    # The GPU's clock domain is a source for all of the components within the
    # GPU. By making it a SrcClkDomain, it can be directly referenced to change
    # the GPU clock frequency dynamically.
    gpu = CudaGPU(warp_size = options.gpu_warp_size,
                  manage_gpu_memory = options.system_config == "opu_libgem5",
                  clk_domain = SrcClockDomain(clock = options.gpu_core_clock,
                                              voltage_domain = VoltageDomain()),
                  gpu_memory_range = gpu_mem_range)

    gpu.cores_wrapper = GPGPUSimComponentWrapper(clk_domain = gpu.clk_domain)

    gpu.icnt_wrapper = GPGPUSimComponentWrapper(clk_domain = DerivedClockDomain(
                                                    clk_domain = gpu.clk_domain,
                                                    clk_divider = 2))

    gpu.l2_wrapper = GPGPUSimComponentWrapper(clk_domain = gpu.clk_domain)
    gpu.dram_wrapper = GPGPUSimComponentWrapper(
                            clk_domain = SrcClockDomain(
                                clock = options.gpu_dram_clock,
                                voltage_domain = gpu.clk_domain.voltage_domain))

    warps_per_core = options.gpu_threads_per_core / options.gpu_warp_size
    gpu.shader_cores = [CudaCore(id = i, warp_contexts = warps_per_core)
                            for i in range(0, options.num_sc)]

    gpu.ce = GPUCopyEngine(driver_delay = 5000000,
                           buffering = options.ce_buffering
                          )

    # Create the zephyr os act as cp
    #gpu.cp = CommandProcessor(driver_delay = 5000,
    #                       buffering = options.cp_buffering,
    #                       start_tick = 20000,
    #                       file_name=options.cp_firmware
    #                      )
    #assert(options.cp_firmware != None)

    # The default setting for atoms_per_cache_subline is 3, consistent with
    # the Fermi microarchitecture. If the user wishes to set it differently,
    # note it and set the value for each shader core below.
    atoms_per_cache_subline = None
    if options.gpu_atoms_per_subline is not None:
        atoms_per_cache_subline = options.gpu_atoms_per_subline
    else:
        # If the user doesn't specify gpu_atoms_per_subline explicitly, then
        # use the number defined for the gpu_core_config
        if options.gpu_core_config == 'Maxwell':
            atoms_per_cache_subline = 32

    for sc in gpu.shader_cores:
        sc.lsq = ShaderLSQ()
        sc.lsq.data_tlb.entries = options.gpu_tlb_entries
        sc.lsq.forward_flush = (buildEnv['PROTOCOL'] == 'VI_hammer_fusion' \
                                and options.flush_kernel_end)
        sc.lsq.warp_size = options.gpu_warp_size
        sc.lsq.cache_line_size = options.cacheline_size
        if atoms_per_cache_subline is not None:
            sc.lsq.atoms_per_subline = atoms_per_cache_subline
        if options.gpu_threads_per_core % options.gpu_warp_size:
            fatal("gpu_warp_size must divide gpu_threads_per_core evenly.")
        sc.lsq.warp_contexts = warps_per_core
        if options.gpu_core_config == 'Fermi':
            # Fermi latency for zero-load independent memory instructions is
            # roughly 19 total cycles with ~4 cycles for tag access
            sc.lsq.l1_tag_cycles = 4
            sc.lsq.latency = 14
        elif options.gpu_core_config == 'Maxwell':
            # Maxwell latency for zero-load independent memory instructions is
            # 8-10 cycles quicker than Fermi, and tag access appears shorter
            sc.lsq.l1_tag_cycles = 1
            sc.lsq.latency = 6
        elif options.gpu_core_config == 'Volta':
            # FIXME, temp copy from Maxwell
            # Maxwell latency for zero-load independent memory instructions is
            # 8-10 cycles quicker than Fermi, and tag access appears shorter
            sc.lsq.l1_tag_cycles = 1
            sc.lsq.latency = 6
        elif options.gpu_core_config == 'Ppu':
            # FIXME, temp copy from Maxwell
            # Maxwell latency for zero-load independent memory instructions is
            # 8-10 cycles quicker than Fermi, and tag access appears shorter
            sc.lsq.l1_tag_cycles = 1
            sc.lsq.latency = 6




    # This is a stop-gap solution until we implement a better way to register device memory
    if options.access_host_pagetable:
        gpu.access_host_pagetable = True
        for sc in gpu.shader_cores:
            sc.itb.access_host_pagetable = True
            sc.lsq.data_tlb.access_host_pagetable = True
        gpu.ce.device_dtb.access_host_pagetable = True
        gpu.ce.host_dtb.access_host_pagetable = True
        #gpu.cp.device_dtb.access_host_pagetable = True
        #gpu.cp.host_dtb.access_host_pagetable = True

    gpu.shared_mem_delay = options.shMemDelay
    gpu.config_path = gpgpusimOptions
    gpu.dump_kernel_stats = options.kernel_stats

    return gpu

def createOPU(options, gpu_mem_range, system):
    # DEPRECATED: Set a default GPU DRAM clock to be passed to the wrapper.
    # This must be eliminated when the wrapper can be removed.
    options.gpu_dram_clock = None

    gpgpusimOptions = parseGpgpusimConfig(options)
    pdb.set_trace()

    # The GPU's clock domain is a source for all of the components within the
    # GPU. By making it a SrcClkDomain, it can be directly referenced to change
    # the GPU clock frequency dynamically.
    gpu = OpuTop(warp_size = options.gpu_warp_size,
                  manage_gpu_memory = options.system_config == "opu_libgem5",
                  clk_domain = SrcClockDomain(clock = options.gpu_core_clock,
                                              voltage_domain = VoltageDomain()),
                  gpu_memory_range = gpu_mem_range)

    gpu.cores_wrapper = OpuSimComponentWrapper(clk_domain = gpu.clk_domain)

    warps_per_core = options.gpu_threads_per_core / options.gpu_warp_size
    gpu.shader_cores = [OpuCore(id = i, warp_contexts = warps_per_core)
                            for i in range(0, options.num_sc)]

    gpu.ce = OpuDma(driver_delay = 5000000,
                           buffering = options.ce_buffering
                          )

    # Create the zephyr os act as cp
    #gpu.cp = CommandProcessor(driver_delay = 5000,
    #                       buffering = options.cp_buffering,
    #                       start_tick = 20000,
    #                       file_name=options.cp_firmware
    #                      )
    #assert(options.cp_firmware != None)

    # The default setting for atoms_per_cache_subline is 3, consistent with
    # the Fermi microarchitecture. If the user wishes to set it differently,
    # note it and set the value for each shader core below.
    atoms_per_cache_subline = None
    if options.gpu_atoms_per_subline is not None:
        atoms_per_cache_subline = options.gpu_atoms_per_subline
    else:
        # If the user doesn't specify gpu_atoms_per_subline explicitly, then
        # use the number defined for the gpu_core_config
        if options.gpu_core_config == 'Maxwell':
            atoms_per_cache_subline = 32

    for sc in gpu.shader_cores:
        sc.lsq = OpuLSQ()
        sc.lsq.data_tlb.entries = options.gpu_tlb_entries
        sc.lsq.forward_flush = (buildEnv['PROTOCOL'] == 'VI_hammer_fusion' \
                                and options.flush_kernel_end)
        sc.lsq.warp_size = options.gpu_warp_size
        sc.lsq.cache_line_size = options.cacheline_size
        if atoms_per_cache_subline is not None:
            sc.lsq.atoms_per_subline = atoms_per_cache_subline
        if options.gpu_threads_per_core % options.gpu_warp_size:
            fatal("gpu_warp_size must divide gpu_threads_per_core evenly.")
        sc.lsq.warp_contexts = warps_per_core
        if options.gpu_core_config == 'Opu':
            # FIXME, temp copy from Maxwell
            # Maxwell latency for zero-load independent memory instructions is
            # 8-10 cycles quicker than Fermi, and tag access appears shorter
            sc.lsq.l1_tag_cycles = 1
            sc.lsq.latency = 6




    # This is a stop-gap solution until we implement a better way to register device memory
    if options.access_host_pagetable:
        gpu.access_host_pagetable = True
        for sc in gpu.shader_cores:
            sc.itb.access_host_pagetable = True
            sc.lsq.data_tlb.access_host_pagetable = True
        gpu.ce.device_dtb.access_host_pagetable = True
        gpu.ce.host_dtb.access_host_pagetable = True
        #gpu.cp.device_dtb.access_host_pagetable = True
        #gpu.cp.host_dtb.access_host_pagetable = True

    gpu.shared_mem_delay = options.shMemDelay
    gpu.config_path = gpgpusimOptions
    gpu.dump_kernel_stats = options.kernel_stats

    return gpu

def connectGPUPorts(gpu, ruby, options, system):
    num_cpus = options.num_cpus
    for i,sc in enumerate(gpu.shader_cores):
        sc.inst_port = ruby._cpu_ports[num_cpus+i].slave
        for j in range(0, options.gpu_warp_size):
            sc.lsq_port[j] = sc.lsq.lane_port[j]
        sc.lsq.cache_port = ruby._cpu_ports[num_cpus+i].slave
        sc.lsq_ctrl_port = sc.lsq.control_port

    #ipdb.set_trace()

    # The total number of sequencers is equal to the number of CPU cores, plus
    # the number of GPU cores plus any pagewalk caches and the copy engine
    # caches. Currently, for unified address space architectures, there is one
    # pagewalk cache and one copy engine cache (2 total), and the pagewalk cache
    # is indexed first. For split address space architectures, there are 2 copy
    # engine caches, and the host-side cache is indexed before the device-side.

    # for ppu two seq is for copy engine and cp
    assert(len(ruby._cpu_ports) == num_cpus + options.num_sc + 2)

    # Initialize the MMU, connecting it to either the pagewalk cache port for
    # unified address space, or the copy engine's host-side sequencer port for
    # split address space architectures.
    # FIXME for PPU how setup ce.host-side sequencer
    # below I just set to ce.device-side sequencer, it is not correct_
    gpu.shader_mmu.setUpPagewalkers(32,
                    ruby._cpu_ports[num_cpus+options.num_sc+1].slave,
                    options.gpu_tlb_bypass_l1)

    if options.system_config == "opu_libgem5":
        # NOTE: In opu_libgem5 address space architectures, the MMU is not used
        # copy engine host-side TLB access is faked to return vaddr as paddr

        # If inappropriately used, crash to inform MMU config problems to user:
        if options.access_host_pagetable:
            fatal('Cannot access host pagetable from the GPU or the copy ' \
                  'engine\'s GPU-side port\n in split address space. Use ' \
                  'only one of --split or --access-host-pagetable')

        # Tie copy engine ports to appropriate sequencers
        gpu.ce.host_port = system.hdp.data_port
        gpu.ce.device_port = ruby._cpu_ports[num_cpus+options.num_sc+1].slave
        gpu.ce.device_dtb.access_host_pagetable = False
        gpu.ce.host_dtb.access_host_pagetable = False
    #elif options.ppu:
    #    # Tie copy engine ports to appropriate sequencers
    #    # the host-side is connect to Membus , and device-side is connect to caches
    #    #gpu.ce.host_port = ruby._cpu_ports[num_cpus+options.num_sc].slave
    #    #gpu.ce.host_port = membus.slave
    #    gpu.ce.host_port = ruby._cpu_ports[num_cpus+options.num_sc+1].slave
    #    gpu.ce.device_port = ruby._cpu_ports[num_cpus+options.num_sc+1].slave

    #    # Tie cp command processor ports to appropriate sequencers

    #    #gpu.cp.pio_port = ruby._cpu_ports[num_cpus+options.num_sc].master

    #    #gpu.cp_membus = IOXBar()
    #    #gpu.cp.port = gpu.cp_membus.slave

    #    #gpu.cp_membus.master = membus.slave
    #    #gpu.cp_membus.master = ruby._cpu_ports[num_cpus+options.num_sc+1].slave
    #    #gpu.cp.device_dtb.access_host_pagetable = False
    else:
        # With a unified address space, tie both copy engine ports to the same
        # copy engine controller. NOTE: The copy engine is often unused in the
        # unified address space
        gpu.ce.host_port = \
            ruby._cpu_ports[num_cpus+options.num_sc].slave
        gpu.ce.device_port = \
            ruby._cpu_ports[num_cpus+options.num_sc+1].slave

def connectOPUPorts(gpu, ruby, options, system):
    num_cpus = options.num_cpus
    for i,sc in enumerate(gpu.shader_cores):
        sc.inst_port = ruby._cpu_ports[num_cpus+i].slave
        for j in range(0, options.gpu_warp_size):
            sc.lsq_port[j] = sc.lsq.lane_port[j]
        sc.lsq.cache_port = ruby._cpu_ports[num_cpus+i].slave
        sc.lsq_ctrl_port = sc.lsq.control_port

    #ipdb.set_trace()

    # The total number of sequencers is equal to the number of CPU cores, plus
    # the number of GPU cores plus any pagewalk caches and the copy engine
    # caches. Currently, for unified address space architectures, there is one
    # pagewalk cache and one copy engine cache (2 total), and the pagewalk cache
    # is indexed first. For split address space architectures, there are 2 copy
    # engine caches, and the host-side cache is indexed before the device-side.

    # for ppu two seq is for copy engine and cp
    assert(len(ruby._cpu_ports) == num_cpus + options.num_sc + 2)

    # Initialize the MMU, connecting it to either the pagewalk cache port for
    # unified address space, or the copy engine's host-side sequencer port for
    # split address space architectures.
    # FIXME for PPU how setup ce.host-side sequencer
    # below I just set to ce.device-side sequencer, it is not correct_
    gpu.opu_mmu.setUpPagewalkers(32,
                    ruby._cpu_ports[num_cpus+options.num_sc+1].slave,
                    options.gpu_tlb_bypass_l1)

    if options.system_config == "opu_libgem5":
        # NOTE: In opu_libgem5 address space architectures, the MMU is not used
        # copy engine host-side TLB access is faked to return vaddr as paddr

        # If inappropriately used, crash to inform MMU config problems to user:
        if options.access_host_pagetable:
            fatal('Cannot access host pagetable from the GPU or the copy ' \
                  'engine\'s GPU-side port\n in split address space. Use ' \
                  'only one of --split or --access-host-pagetable')

        # Tie copy engine ports to appropriate sequencers
        gpu.ce.host_port = system.hdp.data_port
        gpu.ce.device_port = ruby._cpu_ports[num_cpus+options.num_sc+1].slave
        gpu.ce.device_dtb.access_host_pagetable = False
        gpu.ce.host_dtb.access_host_pagetable = False
    #elif options.ppu:
    #    # Tie copy engine ports to appropriate sequencers
    #    # the host-side is connect to Membus , and device-side is connect to caches
    #    #gpu.ce.host_port = ruby._cpu_ports[num_cpus+options.num_sc].slave
    #    #gpu.ce.host_port = membus.slave
    #    gpu.ce.host_port = ruby._cpu_ports[num_cpus+options.num_sc+1].slave
    #    gpu.ce.device_port = ruby._cpu_ports[num_cpus+options.num_sc+1].slave

    #    # Tie cp command processor ports to appropriate sequencers

    #    #gpu.cp.pio_port = ruby._cpu_ports[num_cpus+options.num_sc].master

    #    #gpu.cp_membus = IOXBar()
    #    #gpu.cp.port = gpu.cp_membus.slave

    #    #gpu.cp_membus.master = membus.slave
    #    #gpu.cp_membus.master = ruby._cpu_ports[num_cpus+options.num_sc+1].slave
    #    #gpu.cp.device_dtb.access_host_pagetable = False
    else:
        # With a unified address space, tie both copy engine ports to the same
        # copy engine controller. NOTE: The copy engine is often unused in the
        # unified address space
        gpu.ce.host_port = \
            ruby._cpu_ports[num_cpus+options.num_sc].slave
        gpu.ce.device_port = \
            ruby._cpu_ports[num_cpus+options.num_sc+1].slave
