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
from m5.objects import *
from m5.defines import buildEnv
# from Cluster import Cluster
from topologies.Cluster import Cluster
from ruby.Ruby import create_topology, create_directories
from ruby.Ruby import send_evicts

class L1Cache(RubyCache): pass
class L2Cache(RubyCache): pass
class ProbeFilter(RubyCache): pass

def define_options(parser):
    parser.add_option("--allow-atomic-migration", action="store_true",
          help="allow migratory sharing for atomic only accessed blocks")
    parser.add_option("--pf-on", action="store_true",
          help="Hammer: enable Probe Filter")
    parser.add_option("--dir-on", action="store_true",
          help="Hammer: enable Full-bit Directory")

def create_system(options, full_system, system, dma_ports, bootmem, ruby_system):

    if 'VI_hammer' not in buildEnv['PROTOCOL']:
        panic("This script requires the VI_hammer protocol to be built.")

    options.access_backing_store = True

    cpu_sequencers = []
    #
    # The ruby network creation expects the list of nodes in the system to be
    # consistent with the NetDest list.  Therefore the l1 controller nodes must be
    # listed before the directory nodes and directory nodes before dma nodes, etc.
    #
    #l1_cntrl_nodes = []
    dir_cntrl_nodes = []
    dma_cntrl_nodes = []

    topology = Cluster(intBW = 68, extBW = 68)

    #
    # Must create the individual controllers before the network to ensure the
    # controller constructors are called before the network constructor
    #
    l2_bits_float = math.log(options.num_l2caches, 2)
    l2_bits = int(l2_bits_float)
    if l2_bits_float > l2_bits:
        l2_bits += 1
    block_size_bits = int(math.log(options.cacheline_size, 2))

    for i in xrange(options.num_cpus):
        #
        # First create the Ruby objects associated with this cpu
        #
        l1i_cache = L1Cache(size = options.l1i_size,
                            assoc = options.l1i_assoc,
                            start_index_bit = block_size_bits,
                            is_icache = True)
        l1d_cache = L1Cache(size = options.l1d_size,
                            assoc = options.l1d_assoc,
                            start_index_bit = block_size_bits)
        l2_cache = L2Cache(size = options.l2_size,
                           assoc = options.l2_assoc,
                           start_index_bit = block_size_bits)

        l1_cntrl = L1Cache_Controller(version = i,
                                      L1Icache = l1i_cache,
                                      L1Dcache = l1d_cache,
                                      L2cache = l2_cache,
                                      no_mig_atomic = not \
                                        options.allow_atomic_migration,
                                      send_evictions = send_evicts(options),
                                      transitions_per_cycle = options.ports,
                                      ruby_system = ruby_system)

        cpu_seq = RubySequencer(version = i,
                                icache = l1i_cache,
                                dcache = l1d_cache,
                                ruby_system = ruby_system)

        l1_cntrl.sequencer = cpu_seq

        if options.recycle_latency:
            l1_cntrl.recycle_latency = options.recycle_latency

        exec("ruby_system.l1_cntrl%d = l1_cntrl" % i)
        #
        # Add controllers and sequencers to the appropriate lists
        #
        cpu_sequencers.append(cpu_seq)
        topology.add(l1_cntrl)

        # Connect the L1 controller and the network
        # Connect the buffers from the controller to network
        l1_cntrl.requestFromCache = MessageBuffer()
        l1_cntrl.requestFromCache.master = ruby_system.network.slave
        l1_cntrl.responseFromCache = MessageBuffer()
        l1_cntrl.responseFromCache.master = ruby_system.network.slave
        l1_cntrl.unblockFromCache = MessageBuffer()
        l1_cntrl.unblockFromCache.master = ruby_system.network.slave

        # Connect the buffers from the network to the controller
        l1_cntrl.forwardToCache = MessageBuffer()
        l1_cntrl.forwardToCache.slave = ruby_system.network.master
        l1_cntrl.responseToCache = MessageBuffer()
        l1_cntrl.responseToCache.slave = ruby_system.network.master

        l1_cntrl.mandatoryQueue = MessageBuffer()
        l1_cntrl.triggerQueue = MessageBuffer()

    cpu_mem_range = AddrRange(options.total_mem_size)
    mem_module_size = cpu_mem_range.size() / options.num_dirs

    #
    # determine size and index bits for probe filter
    # By default, the probe filter size is configured to be twice the
    # size of the L2 cache.
    #
    pf_size = MemorySize(options.l2_size)
    pf_size.value = pf_size.value * 2
    dir_bits = int(math.log(options.num_dirs, 2))
    pf_bits = int(math.log(pf_size.value, 2))
    if options.numa_high_bit:
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


    #mem_dir_cntrl_nodes, rom_dir_cntrl_node = create_directories(
    #    options, bootmem, ruby_system, system)

    #dir_cntrl_nodes = mem_dir_cntrl_nodes[:]
    #if rom_dir_cntrl_node is not None:
    #    dir_cntrl_nodes.append(rom_dir_cntrl_node)

    #for dir_cntrl in dir_cntrl_nodes:
    for i in xrange(options.num_dirs):
        #
        # Create the Ruby objects associated with the directory controller
        #

        dir_size = MemorySize('0B')
        dir_size.value = mem_module_size

        pf = ProbeFilter(size = pf_size, assoc = 4,
                         start_index_bit = pf_start_bit)

        dir_cntrl = Directory_Controller()
        dir_cntrl.version = i
        # TODO fix option
        dir_cntrl.directory = RubyDirectoryMemory()
        dir_cntrl.ruby_system = ruby_system
        dir_cntrl.probeFilter = pf

        #dir_cntrl = Directory_Controller(version = i,
        #                                 directory = \
        #                                 RubyDirectoryMemory( \
        #                                            version = i,
        #                                            size = dir_size,
        #                                            numa_high_bit = \
        #                                            options.numa_high_bit),
        #                                 probeFilter = pf,
        #                                 probe_filter_enabled = options.pf_on,
        #                                 full_bit_dir_enabled = options.dir_on,
        #                                 transitions_per_cycle = options.ports,
        #                                 ruby_system = ruby_system)

        if options.recycle_latency:
            dir_cntrl.recycle_latency = options.recycle_latency

        exec("ruby_system.dir_cntrl%d = dir_cntrl" % i)
        dir_cntrl_nodes.append(dir_cntrl)

        # Connect the directory controller to the network
        dir_cntrl.forwardFromDir = MessageBuffer()
        dir_cntrl.forwardFromDir.master = ruby_system.network.slave
        dir_cntrl.responseFromDir = MessageBuffer()
        dir_cntrl.responseFromDir.master = ruby_system.network.slave
        dir_cntrl.dmaResponseFromDir = MessageBuffer(ordered = True)
        dir_cntrl.dmaResponseFromDir.master = ruby_system.network.slave

        dir_cntrl.unblockToDir = MessageBuffer()
        dir_cntrl.unblockToDir.slave = ruby_system.network.master
        dir_cntrl.responseToDir = MessageBuffer()
        dir_cntrl.responseToDir.slave = ruby_system.network.master
        dir_cntrl.requestToDir = MessageBuffer()
        dir_cntrl.requestToDir.slave = ruby_system.network.master
        dir_cntrl.dmaRequestToDir = MessageBuffer(ordered = True)
        dir_cntrl.dmaRequestToDir.slave = ruby_system.network.master

        dir_cntrl.triggerQueue = MessageBuffer(ordered = True)
        dir_cntrl.requestToMemory = MessageBuffer()
        dir_cntrl.responseFromMemory = MessageBuffer()

    for i, dma_port in enumerate(dma_ports):
        #
        # Create the Ruby objects associated with the dma controller
        #
        dma_seq = DMASequencer(version = i,
                               ruby_system = ruby_system)

        dma_cntrl = DMA_Controller(version = i,
                                   dma_sequencer = dma_seq,
                                   transitions_per_cycle = options.ports,
                                   ruby_system = ruby_system)

        exec("ruby_system.dma_cntrl%d = dma_cntrl" % i)
        exec("ruby_system.dma_cntrl%d.dma_sequencer.slave = dma_port" % i)
        dma_cntrl_nodes.append(dma_cntrl)

        if options.recycle_latency:
            dma_cntrl.recycle_latency = options.recycle_latency

        # Connect the dma controller to the network
        dma_cntrl.responseFromDir = MessageBuffer(ordered = True)
        dma_cntrl.responseFromDir.slave = ruby_system.network.master
        dma_cntrl.requestToDir = MessageBuffer()
        dma_cntrl.requestToDir.master = ruby_system.network.slave

        dma_cntrl.mandatoryQueue = MessageBuffer()

    # Create the io controller and the sequencer
    if full_system:
        io_seq = DMASequencer(version=len(dma_ports), ruby_system=ruby_system)
        ruby_system._io_port = io_seq
        io_controller = DMA_Controller(version = len(dma_ports),
                                       dma_sequencer = io_seq,
                                       transitions_per_cycle = options.ports,
                                       ruby_system = ruby_system)
        ruby_system.io_controller = io_controller

        # Connect the dma controller to the network
        io_controller.responseFromDir = MessageBuffer(ordered = True)
        io_controller.responseFromDir.slave = ruby_system.network.master
        io_controller.requestToDir = MessageBuffer()
        io_controller.requestToDir.master = ruby_system.network.slave

        io_controller.mandatoryQueue = MessageBuffer()

        dma_cntrl_nodes.append(io_controller)

    # TODO fix me 5 or 10, why need to specifiy
    ruby_system.network.number_of_virtual_networks = 10

    return (cpu_sequencers, dir_cntrl_nodes, dma_cntrl_nodes, topology)
