# Copyright (c) 2006-2008 The Regents of The University of Michigan
# Copyright (c) 2012-2015 Mark D. Hill and David A. Wood
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
import argparse
import os
import sys
from os.path import join as joinpath

import m5
from m5.defines import buildEnv
from m5.objects import *
from m5.objects.Platform import Platform
#from m5.util import addToPath, fatal
from m5.util import *

addToPath('../../../configs')
#addToPath('../../../configs/common')
#addToPath('../../../configs/ruby')
#addToPath('../../../configs/topologies')
addToPath('gpu_protocol')

import GPUConfig
import GPUMemConfig
#import Options
#import Ruby
#import Simulation
from common import Options
from ruby import Ruby
from common import Simulation
from common import CacheConfig
from common import CpuConfig
from common import ObjectList
from common import MemConfig
from common.FileSystemConfig import config_filesystem
from common.Caches import *
from common.cpu2000 import *

#import ipdb

parser = argparse.ArgumentParser()
GPUConfig.addGPUOptions(parser)
GPUMemConfig.addMemCtrlOptions(parser)
Options.addCommonOptions(parser)
Options.addSEOptions(parser)

#
# Add the ruby specific and protocol specific options
#
Ruby.define_options(parser)

options = parser.parse_args()

options.ruby = True


#if args:
#    print("Error: script doesn't take any positional arguments")
#    sys.exit(1)

if buildEnv['TARGET_ISA'] not in ["x86", "arm"]:
    fatal("gem5-gpu SE doesn't currently work with non-x86 or non-ARM system!")

#
# CPU type configuration
#
if options.cpu_type != "timing" and options.cpu_type != "TimingSimpleCPU" \
    and options.cpu_type != "detailed" and options.cpu_type != "DerivO3CPU":
    print("Warning: gem5-gpu only known to work with timing and detailed CPUs: Proceed at your own risk!")
(CPUClass, test_mem_mode, FutureClass) = Simulation.setCPUClass(options)

# If fast-forwarding, set the fast-forward CPU and mem mode for
# timing rather than atomic
if options.fast_forward:
    assert(CPUClass == AtomicSimpleCPU)
    assert(test_mem_mode == "atomic")
    CPUClass, test_mem_mode = Simulation.getCPUClass("TimingSimpleCPU")

#ipdb.set_trace()
#
# Memory space configuration
#
(cpu_mem_range, gpu_mem_range, total_mem_range) = GPUConfig.configureMemorySpaces(options)

#
# Setup benchmark to be run
#
process = Process()
process.executable = options.cmd
process.cmd = [options.cmd] + options.options.split()

if options.env:
    with open(options.env, 'r') as f:
        process.env = [line.rstrip() for line in f]


if options.input != "":
    process.input = options.input
if options.output != "":
    process.output = options.output
if options.errout != "":
    process.errout = options.errout

# Hard code the cache block width to 128B for now
# TODO: Remove this if/when block size can be different than 128B
if options.cacheline_size != 128:
    print("Warning: Only block size currently supported is 128B. Defaulting to 128.")
    options.cacheline_size = 128

cp_process = Process(pid=1000)
cp_process.executable = options.cp_firmware
cp_process.cmd = options.cp_firmware

multiprocesses = [process, cp_process]
options.num_cpus = 2
############################
# Instantiate system
#
# add multip-thread to support clone syscall
#   clone is need for system() in user app
multi_thread = False
num_threads = 1
system = System(cpu = [CPUClass(cpu_id = i,
                                workload = multiprocesses[i],
                                numThreads = num_threads
                               )
                       for i in range(0, options.num_cpus)],
                multi_thread = multi_thread,
                mem_mode = test_mem_mode,
                mem_ranges = [total_mem_range],
                cache_line_size = options.cacheline_size)

if ObjectList.is_kvm_cpu(CPUClass) or ObjectList.is_kvm_cpu(FutureClass):
    if buildEnv['TARGET_ISA'] == 'x86':
        system.kvm_vm = KvmVM()
        for process in multiprocesses:
            process.useArchPT = True
            process.kvmInSE = True
    else:
        fatal("KvmCPU can only be used in SE mode with x86")

# Create a top-level voltage domain
system.voltage_domain = VoltageDomain(voltage = options.sys_voltage)

# Create a source clock for the system and set the clock period
system.clk_domain = SrcClockDomain(clock = options.sys_clock,
                                   voltage_domain = system.voltage_domain)

# Create a CPU voltage domain
system.cpu_voltage_domain = VoltageDomain()

# Create a separate clock domain for the CPUs
system.cpu_clk_domain = SrcClockDomain(clock = options.cpu_clock,
                                       voltage_domain =
                                       system.cpu_voltage_domain)

Simulation.setWorkCountOptions(system, options)

############################
# Create the GPU
#
pdb.set_trace()
if options.system_config == 'cpu_only':
    pass
elif options.system_config == 'gpgpu':
    system.gpu = GPUConfig.createGPU(options, gpu_mem_range, system)
else:
    system.gpu = GPUConfig.createOPU(options, gpu_mem_range, system)

#
# Setup Ruby
#
system.ruby_clk_domain = SrcClockDomain(clock = options.ruby_clock,
                                        voltage_domain = system.voltage_domain)

Ruby.create_system(options, False, system)

if options.system_config != 'cpu_only':
    system.gpu.ruby = system.ruby
system.ruby.clk_domain = system.ruby_clk_domain
system.ruby.block_size_bytes = 128

#ipdb.set_trace()
#if options.split or options.ppu:
#    if options.access_backing_store:
#        #
#        # Reset Ruby's phys_mem to add the device memory range
#        #
#        system.ruby.phys_mem = SimpleMemory(range=total_mem_range,
#                                            in_addr_map=False)



###############################################
# Connect CPU ports
#
# Create a memory bus, a system crossbar, in this case
# IOXbar is non-coherent , it run little faster in gem5
#system.membus = SystemXBar()
#system.membus = IOXBar()
class MemBus(SystemXBar):
    badaddr_responder = BadAddr()
    default = Self.badaddr_responder.pio

def connectX86ClassicSystem(x86_sys, numCPUs):
    x86_sys.membus = MemBus()
    # North Bridge
    x86_sys.iobus = IOXBar()
    x86_sys.pc.atttachIO(x86_sys.iobus)

# for fs
# system.pc = Pc()
# connectX86ClassicSystem(system, numCPUs)
# systgem.pci_host = GenericPciHost()
# system.pci_host.pio = system.membus.mem_side_ports
# system.pci_device = OpuPciDevice()
# system.pci_device.host = system.pci_host
# system.pci_device.pio = system.membus.mem_side_ports
# system.pci_device.dma = system.membus.slave

system.hdp = HostDataPath()


for (i, cpu) in enumerate(system.cpu):
#    cpu.wait_for_remote_gdb = options.wait_for_gdb
    ruby_port = system.ruby._cpu_ports[i]

    cpu.clk_domain = system.cpu_clk_domain
    cpu.createThreads()
    cpu.createInterruptController()

    # Connect the cpu's cache ports to Ruby
    ruby_port.connectCpuPorts(cpu)

    #if buildEnv['TARGET_ISA'] == "x86":
    #    cpu.interrupts[0].pio = ruby_port.master
    #    cpu.interrupts[0].int_master = ruby_port.slave
    #    cpu.interrupts[0].int_slave = ruby_port.master
    ## Tie the cpu port s to correct ruby system ports
    #cpu.icache_port = system.ruby._cpu_ports[i].slave
    #cpu.dcache_port = system.ruby._cpu_ports[i].slave
    #if buildEnv['TARGET_ISA'] == "x86":
    #    cpu.itb.walker.port = system.ruby._cpu_ports[i].slave
    #    cpu.dtb.walker.port = system.ruby._cpu_ports[i].slave
    #else:
    #    fatal("Not sure how to connect TLB walker ports in non-x86 system!")

##########################
# Connect GPU ports
#
if options.system_config == 'cpu_only':
    pass
elif options.system_config == 'gpgpu':
    GPUConfig.connectGPUPorts(system.gpu, system.ruby, options, system)
else:
    GPUConfig.connectOPUPorts(system.gpu, system.ruby, options, system)

if options.mem_type == "RubyMemoryControl":
    GPUMemConfig.setMemoryControlOptions(system, options)

system.workload = SEWorkload.init_compatible(process.executable)
#
# Finalize setup and run
#

root = Root(full_system = False, system = system)

# allow gdb connect
#m5.disableAllListeners()

Simulation.run(options, root, system, FutureClass)
