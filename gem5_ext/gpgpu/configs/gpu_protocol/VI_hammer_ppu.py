# Copyright (c) 2006-2007 The Regents of The University of Michigan
# Copyright (c) 2009 Advanced Micro Devices, Inc.
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
# Authors: Brad Beckmann

import math
import m5
import VI_hammer
from m5.objects import *
from m5.defines import buildEnv
from topologies.Cluster import Cluster
from common import MemConfig
from common import ObjectList
import ipdb

class L1Cache(RubyCache): pass
class L2Cache(RubyCache): pass
class ProbeFilter(RubyCache): pass

def create_system(options, full_system, system, dma_devices, bootmem, ruby_system):

    #if not buildEnv['GPGPU_SIM']:
    #    m5.util.panic("This script requires GPGPU-Sim integration to be built.")

    # Run the protocol script to setup CPU cluster, directory and DMA
    (all_sequencers, dir_cntrls, dma_cntrls, cpu_cluster) = \
                                        VI_hammer.create_system(options,
                                                                full_system,
                                                                system,
                                                                dma_devices,
                                                                bootmem,
                                                                ruby_system)

    # If we're going to split the directories/memory controllers
    if options.num_dev_dirs > 0:
        cpu_cntrl_count = len(cpu_cluster)
    else:
        cpu_cntrl_count = len(cpu_cluster) + len(dir_cntrls)

    #ipdb.set_trace()


    ####################################################################
    # Build GPU cluster
    #
    # Empirically, Fermi per-core bandwidth peaks at roughly 23GB/s
    # (32B/cycle @ 772MHz). Use ~16B per Ruby cycle to match this. Maxwell
    # per-core bandwidth peaks at 40GB/s (42B/cycle @ 1029MHz). Use ~24B per
    # Ruby cycle to match this.
    if options.gpu_core_config == 'Ppu':
        # FIXME on Volta bw
        l1_cluster_bw = 24
    else:
        m5.util.fatal("Unknown GPU core config: %s" % options.gpu_core_config)

    gpu_cluster = Cluster(intBW = l1_cluster_bw, extBW = l1_cluster_bw)
    gpu_cluster.disableConnectToParent()

    l2_bits = int(math.log(options.num_l2caches, 2))
    block_size_bits = int(math.log(options.cacheline_size, 2))
    # This represents the L1 to L2 interconnect latency
    # NOTES! 1) This latency is in Ruby (cache) cycles, not SM cycles
    #        2) Since the cluster interconnect doesn't model multihop latencies,
    #           model these latencies with the controller latency variables. If
    #           the interconnect model is changed, latencies will need to be
    #           adjusted for reasonable total memory access delay.
    per_hop_interconnect_latency = 45 # ~15 GPU cycles
    num_dance_hall_hops = int(math.log(options.num_sc, 2))
    if num_dance_hall_hops == 0:
        num_dance_hall_hops = 1
    l1_to_l2_noc_latency = per_hop_interconnect_latency * num_dance_hall_hops

    #
    # Caches for GPU cores
    #
    for i in xrange(options.num_sc):
        #
        # First create the Ruby objects associated with the GPU cores
        #
        cache = L1Cache(size = options.sc_l1_size,
                            assoc = options.sc_l1_assoc,
                            replacement_policy = TreePLRURP(), # LRUReplacementPolicy(),
                            start_index_bit = block_size_bits,
                            dataArrayBanks = 4,
                            tagArrayBanks = 4,
                            dataAccessLatency = 4,
                            tagAccessLatency = 4,
                            resourceStalls = False)

        l1_cntrl = GPUL1Cache_Controller(version = i,
                                  cache = cache,
                                  l2_select_num_bits = l2_bits,
                                  num_l2 = options.num_l2caches,
                                  transitions_per_cycle = options.ports,
                                  issue_latency = l1_to_l2_noc_latency,
                                  number_of_TBEs = options.gpu_l1_buf_depth,
                                  ruby_system = ruby_system)

        gpu_seq = RubySequencer(version = options.num_cpus + i,
                            icache = cache,
                            dcache = cache,
                            max_outstanding_requests = options.gpu_l1_buf_depth,
                            ruby_system = ruby_system,
                            deadlock_threshold = 2000000)
                            # connect_to_io = False)

        l1_cntrl.sequencer = gpu_seq

        exec("ruby_system.l1_cntrl_sp%02d = l1_cntrl" % i)

        #
        # Add controllers and sequencers to the appropriate lists
        #
        all_sequencers.append(gpu_seq)
        gpu_cluster.add(l1_cntrl)

        # Connect the controller to the network
        l1_cntrl.requestFromL1Cache = MessageBuffer(ordered = True)
        l1_cntrl.requestFromL1Cache.master = ruby_system.network.slave
        l1_cntrl.responseToL1Cache = MessageBuffer(ordered = True)
        l1_cntrl.responseToL1Cache.slave = ruby_system.network.master

        l1_cntrl.mandatoryQueue = MessageBuffer()

    l2_index_start = block_size_bits + l2_bits
    # Use L2 cache and interconnect latencies to calculate protocol latencies
    # NOTES! 1) These latencies are in Ruby (cache) cycles, not SM cycles
    #        2) Since the cluster interconnect doesn't model multihop latencies,
    #           model these latencies with the controller latency variables. If
    #           the interconnect model is changed, latencies will need to be
    #           adjusted for reasonable total memory access delay.
    l2_cache_access_latency = 30 # ~10 GPU cycles
    l2_to_l1_noc_latency = per_hop_interconnect_latency * num_dance_hall_hops
    l2_to_mem_noc_latency = 125 # ~40 GPU cycles
    # Empirically, Fermi per-L2 bank bandwidth peaks at roughly 66GB/s
    # (92B/cycle @ 772MHz). Use ~34B per Ruby cycle to match this. Maxwell
    # per-L2 bank bandwidth peaks at 123GB/s (128B/cycle @ 1029MHz). Use ~64B
    # per Ruby cycle to match this.
    if options.gpu_core_config == 'Ppu':
        # FIXME
        l2_cluster_bw = 68
    else:
        m5.util.fatal("Unknown GPU core config: %s" % options.gpu_core_config)

    l2_clusters = []
    for i in xrange(options.num_l2caches):
        #
        # First create the Ruby objects associated with this cpu
        #
        l2_cache = L2Cache(size = options.sc_l2_size,
                           assoc = options.sc_l2_assoc,
                           start_index_bit = l2_index_start,
                           replacement_policy = TreePLRURP(), # LRUReplacementPolicy(),
                           dataArrayBanks = 4,
                           tagArrayBanks = 4,
                           dataAccessLatency = 4,
                           tagAccessLatency = 4,
                           resourceStalls = options.gpu_l2_resource_stalls)

        l2_cntrl = GPUL2Cache_Controller(version = i,
                                L2cache = l2_cache,
                                transitions_per_cycle = options.ports,
                                l2_response_latency = l2_cache_access_latency +
                                                      l2_to_l1_noc_latency,
                                l2_request_latency = l2_to_mem_noc_latency,
                                cache_response_latency = l2_cache_access_latency,
                                ruby_system = ruby_system)

        exec("ruby_system.l2_cntrl%d = l2_cntrl" % i)
        l2_cluster = Cluster(intBW = l2_cluster_bw, extBW = l2_cluster_bw)
        l2_cluster.add(l2_cntrl)
        gpu_cluster.add(l2_cluster)
        l2_clusters.append(l2_cluster)

        # Connect the controller to the network
        l2_cntrl.responseToL1Cache = MessageBuffer(ordered = True)
        l2_cntrl.responseToL1Cache.master = ruby_system.network.slave
        l2_cntrl.requestFromCache = MessageBuffer()
        l2_cntrl.requestFromCache.master = ruby_system.network.slave
        l2_cntrl.responseFromCache = MessageBuffer()
        l2_cntrl.responseFromCache.master = ruby_system.network.slave
        l2_cntrl.unblockFromCache = MessageBuffer()
        l2_cntrl.unblockFromCache.master = ruby_system.network.slave

        l2_cntrl.requestFromL1Cache = MessageBuffer(ordered = True)
        l2_cntrl.requestFromL1Cache.slave = ruby_system.network.master
        l2_cntrl.forwardToCache = MessageBuffer()
        l2_cntrl.forwardToCache.slave = ruby_system.network.master
        l2_cntrl.responseToCache = MessageBuffer()
        l2_cntrl.responseToCache.slave = ruby_system.network.master

        l2_cntrl.triggerQueue = MessageBuffer()

    ############################################################################
    # dev dir controller
    gpu_phys_mem_size = system.gpu.gpu_memory_range.size()

    #ipdb.set_trace()
    if options.num_dev_dirs > 0:
    #if False:
        mem_module_size = gpu_phys_mem_size / options.num_dev_dirs

        #
        # determine size and index bits for probe filter
        # By default, the probe filter size is configured to be twice the
        # size of the L2 cache.
        #
        pf_size = MemorySize(options.sc_l2_size)
        pf_size.value = pf_size.value * 2
        dir_bits = int(math.log(options.num_dev_dirs, 2))
        pf_bits = int(math.log(pf_size.value, 2))
        if options.dev_numa_high_bit:
            if options.pf_on or options.dir_on:
                # if numa high bit explicitly set, make sure it does not overlap
                # with the probe filter index
                assert(options.numa_high_bit - dir_bits > pf_bits)

            # set the probe filter start bit to just above the block offset
            pf_start_bit = block_size_bits
        else:
            if dir_bits > 0:
                pf_start_bit = dir_bits + block_size_bits - 1
            else:
                pf_start_bit = block_size_bits

        dev_dir_cntrls = []
        dev_mem_ctrls = []
        num_cpu_dirs = len(dir_cntrls)
        for i in xrange(options.num_dev_dirs):
            #
            # Create the Ruby objects associated with the directory controller
            #

            dir_version = i + num_cpu_dirs

            dir_size = MemorySize('0B')
            dir_size.value = mem_module_size

            pf = ProbeFilter(size = pf_size, assoc = 4,
                             start_index_bit = pf_start_bit)

            dev_dir_cntrl = Directory_Controller(version = dir_version,
                                 directory = RubyDirectoryMemory(
                                            version = dir_version,
                                            size = dir_size,
                                            numa_high_bit = options.numa_high_bit,
                                            device_directory = True),
                                 probeFilter = pf,
                                 #probe_filter_enabled = options.pf_on,
                                 #full_bit_dir_enabled = options.dir_on,
                                 #transitions_per_cycle = options.ports,
                                 ruby_system = ruby_system)

            if options.recycle_latency:
                dev_dir_cntrl.recycle_latency = options.recycle_latency

            exec("ruby_system.dev_dir_cntrl%d = dev_dir_cntrl" % i)
            dev_dir_cntrls.append(dev_dir_cntrl)

            # Connect the directory controller to the network
            dev_dir_cntrl.forwardFromDir = MessageBuffer()
            dev_dir_cntrl.forwardFromDir.master = ruby_system.network.slave
            dev_dir_cntrl.responseFromDir = MessageBuffer()
            dev_dir_cntrl.responseFromDir.master = ruby_system.network.slave
            dev_dir_cntrl.dmaResponseFromDir = MessageBuffer(ordered = True)
            dev_dir_cntrl.dmaResponseFromDir.master = ruby_system.network.slave

            dev_dir_cntrl.unblockToDir = MessageBuffer()
            dev_dir_cntrl.unblockToDir.slave = ruby_system.network.master
            dev_dir_cntrl.responseToDir = MessageBuffer()
            dev_dir_cntrl.responseToDir.slave = ruby_system.network.master
            dev_dir_cntrl.requestToDir = MessageBuffer()
            dev_dir_cntrl.requestToDir.slave = ruby_system.network.master
            dev_dir_cntrl.dmaRequestToDir = MessageBuffer(ordered = True)
            dev_dir_cntrl.dmaRequestToDir.slave = ruby_system.network.master

            dev_dir_cntrl.triggerQueue = MessageBuffer(ordered = True)
            dev_dir_cntrl.requestToMemory = MessageBuffer()
            dev_dir_cntrl.responseFromMemory = MessageBuffer()

            #mem_type = 'SimpleMemory'
            #mem_channels = 1
            #options.mem_type = mem_type
            #options.mem_channels = mem_channels
            #MemConfig.config_mem(options, ppsystem)

            #dev_mem_ctrl = DDR3_1600_8x8()
            #dev_mem_ctrl.range = system.gpu.gpu_mem_range
            #dev_mem_ctrl.port = dev_dir_cntrl.memory
            #dev_mem_ctrls.append(dev_mem_ctrl)

            mem_type = ObjectList.mem_list.get(options.mem_type)
            dev_mem_ctrl = MemConfig.create_mem_ctrl(
                #MemConfig(options.mem_type), system.gpu.gpu_memory_range,
                mem_type, system.gpu.gpu_memory_range,
                i, options.num_dev_dirs, int(math.log(options.num_dev_dirs, 2)),
                options.cacheline_size)
            dev_mem_ctrl.port = dev_dir_cntrl.memory
            dev_mem_ctrls.append(dev_mem_ctrl)

        system.dev_mem_ctrls = dev_mem_ctrls
    else:
        # Since there are no device directories, use CPU directories
        # Fix up the memory sizes of the CPU directories
        num_dirs = len(dir_cntrls)
        add_gpu_mem = gpu_phys_mem_size / num_dirs
        for cntrl in dir_cntrls:
            #new_size = cntrl.directory.size.value + add_gpu_mem
            new_size = cntrl.directory.addr_ranges[0].size + add_gpu_mem
            cntrl.directory.size.value = new_size

    ############################################################################
    # Pagewalk cache
    # NOTE: We use a CPU L1 cache controller here. This is to facilatate MMU
    #       cache coherence (as the GPU L1 caches are incoherent without flushes
    #       The L2 cache is small, and should have minimal affect on the
    #       performance (see Section 6.2 of Power et al. HPCA 2014).
    pwd_cache = L1Cache(size = options.pwc_size,
                            assoc = 16, # 64 is fully associative @ 8kB
                            replacement_policy = TreePLRURP(), # LRUReplacementPolicy(),
                            start_index_bit = block_size_bits,
                            resourceStalls = False)
    # Small cache since CPU L1 requires I and D
    pwi_cache = L1Cache(size = "512B",
                            assoc = 2,
                            replacement_policy = TreePLRURP(), # LRUReplacementPolicy(),
                            start_index_bit = block_size_bits,
                            resourceStalls = False)

    # Small cache since CPU L1 controller requires L2
    l2_cache = L2Cache(size = "512B",
                           assoc = 2,
                           start_index_bit = block_size_bits,
                           resourceStalls = False)

    l1_pw_cntrl = L1Cache_Controller(version = options.num_cpus,
                                  L1Icache = pwi_cache,
                                  L1Dcache = pwd_cache,
                                  L2cache = l2_cache,
                                  send_evictions = False,
                                  transitions_per_cycle = options.ports,
                                  issue_latency = l1_to_l2_noc_latency,
                                  cache_response_latency = 1,
                                  l2_cache_hit_latency = 1,
                                  number_of_TBEs = options.gpu_l1_buf_depth,
                                  ruby_system = ruby_system)

    cpu_seq = RubySequencer(version = options.num_cpus + options.num_sc,
                            icache = pwd_cache, # Never get data from pwi_cache
                            dcache = pwd_cache,
                            # TODO the latency value is setting in controller
                            # dcache_hit_latency = 8,
                            # icache_hit_latency = 8,
                            max_outstanding_requests = options.gpu_l1_buf_depth,
                            ruby_system = ruby_system,
                            deadlock_threshold = 2000000)
                            #connect_to_io = False)

    l1_pw_cntrl.sequencer = cpu_seq


    ruby_system.l1_pw_cntrl = l1_pw_cntrl
    all_sequencers.append(cpu_seq)

    gpu_cluster.add(l1_pw_cntrl)

    # Connect the L1 controller and the network
    # Connect the buffers from the controller to network
    l1_pw_cntrl.requestFromCache = MessageBuffer()
    l1_pw_cntrl.requestFromCache.master = ruby_system.network.slave
    l1_pw_cntrl.responseFromCache = MessageBuffer()
    l1_pw_cntrl.responseFromCache.master = ruby_system.network.slave
    l1_pw_cntrl.unblockFromCache = MessageBuffer()
    l1_pw_cntrl.unblockFromCache.master = ruby_system.network.slave

    # Connect the buffers from the network to the controller
    l1_pw_cntrl.forwardToCache = MessageBuffer()
    l1_pw_cntrl.forwardToCache.slave = ruby_system.network.master
    l1_pw_cntrl.responseToCache = MessageBuffer()
    l1_pw_cntrl.responseToCache.slave = ruby_system.network.master

    l1_pw_cntrl.mandatoryQueue = MessageBuffer()
    l1_pw_cntrl.triggerQueue = MessageBuffer()


    ############################################################################
    # copy engine
    #
    # Create controller for the copy engine to connect to in GPU cluster
    # Cache is unused by controller
    #
    cache = L1Cache(size = "4096B", assoc = 2)

    # Setting options.ce_buffering = 0 indicates that the CE can use infinite
    # buffering, but we need to specify a finite number of outstandng accesses
    # that the CE is allowed to issue. Just set it to some large number greater
    # than normal memory access latencies to ensure that the sequencer could
    # service one access per cycle.
    max_out_reqs = options.ce_buffering
    if max_out_reqs == 0:
        max_out_reqs = 1024

    gpu_ce_seq = RubySequencer(version = options.num_cpus + options.num_sc+1,
                               icache = cache,
                               dcache = cache,
                               max_outstanding_requests = max_out_reqs,
                               support_inst_reqs = False,
                               ruby_system = ruby_system)
                               #connect_to_io = False)

    gpu_ce_cntrl = GPUCopyDMA_Controller(version = 0,
                                  sequencer = gpu_ce_seq,
                                  transitions_per_cycle = options.ports,
                                  number_of_TBEs = max_out_reqs,
                                  ruby_system = ruby_system)

    gpu_ce_cntrl.responseFromDir = MessageBuffer(ordered = True)
    gpu_ce_cntrl.responseFromDir.slave = ruby_system.network.master
    gpu_ce_cntrl.reqToDirectory = MessageBuffer(ordered = True)
    gpu_ce_cntrl.reqToDirectory.master = ruby_system.network.slave

    gpu_ce_cntrl.mandatoryQueue = MessageBuffer()

    ruby_system.ce_cntrl = gpu_ce_cntrl

    all_sequencers.append(gpu_ce_seq)

    # To limit the copy engine's bandwidth, we add it to a limited bandwidth
    # cluster. Approximate settings are as follows (assuming 2GHz Ruby clock):
    #   PCIe v1.x x16 effective bandwidth ~= 4GB/s: intBW = 3, extBW = 3
    #   PCIe v2.x x16 effective bandwidth ~= 8GB/s: intBW = 5, extBW = 5
    #   PCIe v3.x x16 effective bandwidth ~= 16GB/s: intBW = 10, extBW = 10
    #   PCIe v4.x x16 effective bandwidth ~= 32GB/s: intBW = 21, extBW = 21
    # NOTE: Bandwidth may bottleneck at other parts of the memory hierarchy,
    # so bandwidth considerations should be made in other parts of the memory
    # hierarchy also.
    gpu_ce_cluster = Cluster(intBW = 10, extBW = 10)
    gpu_ce_cluster.add(gpu_ce_cntrl)

    complete_cluster = Cluster(intBW = 32, extBW = 32)
    complete_cluster.add(gpu_ce_cluster)
    complete_cluster.add(cpu_cluster)
    complete_cluster.add(gpu_cluster)

    for cntrl in dir_cntrls:
        complete_cluster.add(cntrl)

    for cntrl in dma_cntrls:
        complete_cluster.add(cntrl)

    for cluster in l2_clusters:
        complete_cluster.add(cluster)

    return (all_sequencers, dir_cntrls, complete_cluster)
