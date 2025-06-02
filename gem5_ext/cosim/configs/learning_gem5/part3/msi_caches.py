# -*- coding: utf-8 -*-
# Copyright (c) 2017 Jason Power
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
# Authors: Jason Power

""" This file creates a set of Ruby caches, the Ruby network, and a simple
point-to-point topology.
See Part 3 in the Learning gem5 book:
http://gem5.org/documentation/learning_gem5/part3/MSIintro

IMPORTANT: If you modify this file, it's likely that the Learning gem5 book
           also needs to be updated. For now, email Jason <jason@lowepower.com>

"""

from __future__ import print_function
from __future__ import absolute_import

import math

from m5.defines import buildEnv
from m5.util import fatal, panic

from m5.objects import *
import pdb

class MyCacheSystem(RubySystem):

    def __init__(self):
        pdb.set_trace()
        # if buildEnv['PROTOCOL'] != 'MSI':
        if buildEnv['PROTOCOL'] != 'MSI' and buildEnv['PROTOCOL'] != 'VI_hammer':
            fatal("This system assumes MSI from learning gem5!")

        super(MyCacheSystem, self).__init__()

    def setup(self, system, cpus, mem_ctrls):
        """Set up the Ruby cache subsystem. Note: This can't be done in the
           constructor because many of these items require a pointer to the
           ruby system (self). This causes infinite recursion in initialize()
           if we do this in the __init__.
        """
        pdb.set_trace()
        # Ruby's global network.
        self.network = MyNetwork(self)

        # MSI uses 3 virtual networks. One for requests (lowest priority), one
        # for responses (highest priority), and one for "forwards" or
        # cache-to-cache requests. See *.sm files for details.
        self.number_of_virtual_networks = 3
        self.network.number_of_virtual_networks = 3

        pdb.set_trace()
        # There is a single global list of all of the controllers to make it
        # easier to connect everything to the global network. This can be
        # customized depending on the topology/network requirements.
        # Create one controller for each L1 cache (and the cache mem obj.)
        # Create a single directory controller (Really the memory cntrl)
        self.controllers = \
            [L1Cache(system, self, cpu) for cpu in cpus] + \
            [DirController(self, system.mem_ranges, mem_ctrls)]

        # Create one sequencer per CPU. In many systems this is more
        # complicated since you have to create sequencers for DMA controllers
        # and other controllers, too.
        self.sequencers = [RubySequencer(version = i,
                                # I/D cache is combined and grab from ctrl
                                icache = self.controllers[i].cacheMemory,
                                dcache = self.controllers[i].cacheMemory,
                                clk_domain = self.controllers[i].clk_domain,
                                ) for i in range(len(cpus))]

        # We know that we put the controllers in an order such that the first
        # N of them are the L1 caches which need a sequencer pointer
        for i,c in enumerate(self.controllers[0:len(self.sequencers)]):
            c.sequencer = self.sequencers[i]

        self.num_of_sequencers = len(self.sequencers)

        # Create the network and connect the controllers.
        # NOTE: This is quite different if using Garnet!
        self.network.connectControllers(self.controllers)
        self.network.setup_buffers()

        # Set up a proxy port for the system_port. Used for load binaries and
        # other functional-only things.
        self.sys_port_proxy = RubyPortProxy()
        system.system_port = self.sys_port_proxy.slave

        # Connect the cpu's cache, interrupt, and TLB ports to Ruby
        for i,cpu in enumerate(cpus):
            cpu.icache_port = self.sequencers[i].slave
            cpu.dcache_port = self.sequencers[i].slave
            isa = buildEnv['TARGET_ISA']
            if isa == 'x86':
                cpu.interrupts[0].pio = self.sequencers[i].master
                cpu.interrupts[0].int_master = self.sequencers[i].slave
                cpu.interrupts[0].int_slave = self.sequencers[i].master
            if isa == 'x86' or isa == 'arm':
                cpu.itb.walker.port = self.sequencers[i].slave
                cpu.dtb.walker.port = self.sequencers[i].slave


class L1Cache(L1Cache_Controller):

    _version = 0
    @classmethod
    def versionCount(cls):
        cls._version += 1 # Use count for this particular type
        return cls._version - 1

    def __init__(self, system, ruby_system, cpu):
        """CPUs are needed to grab the clock domain and system is needed for
           the cache block size.
        """
        pdb.set_trace()
        super(L1Cache, self).__init__()

        self.version = self.versionCount()
        # This is the cache memory object that stores the cache data and tags
        self.cacheMemory = RubyCache(size = '16kB',
                               assoc = 8,
                               start_index_bit = self.getBlockSizeBits(system))
        self.clk_domain = cpu.clk_domain
        self.send_evictions = self.sendEvicts(cpu)
        self.ruby_system = ruby_system
        self.connectQueues(ruby_system)

    def getBlockSizeBits(self, system):
        bits = int(math.log(system.cache_line_size, 2))
        if 2**bits != system.cache_line_size.value:
            panic("Cache line size not a power of 2!")
        return bits

    def sendEvicts(self, cpu):
        """True if the CPU model or ISA requires sending evictions from caches
           to the CPU. Two scenarios warrant forwarding evictions to the CPU:
           1. The O3 model must keep the LSQ coherent with the caches
           2. The x86 mwait instruction is built on top of coherence
           3. The local exclusive monitor in ARM systems
        """
        if type(cpu) is DerivO3CPU or \
           buildEnv['TARGET_ISA'] in ('x86', 'arm'):
            return True
        return False

    def connectQueues(self, ruby_system):
        """Connect all of the queues for this controller.
        """
        # mandatoryQueue is a special variable. It is used by the sequencer to
        # send RubyRequests from the CPU (or other processor). It isn't
        # explicitly connected to anything.
        self.mandatoryQueue = MessageBuffer()



        # All message buffers must be created and connected to the general
        # Ruby network. In this case, "slave/master" don't mean the same thing
        # as normal gem5 ports. If a MessageBuffer is a "to" buffer (i.e., out)
        # then you use the "master", otherwise, the slave.
        self.requestToDir = MessageBuffer(ordered = True)
        self.responseToDirOrSibling = MessageBuffer(ordered = True)
        self.forwardFromDir = MessageBuffer(ordered = True)
        self.responseFromDirOrSibling = MessageBuffer(ordered = True)

        self.tlm_slave_ = [ExternalSlave() for i in range(2)]
        self.tlm_master_ = [ExternalMaster() for i in range(2)]

        for i in range(2):
            self.tlm_slave_[i].addr_ranges = [AddrRange('256MB')]
            self.tlm_slave_[i].port_type = "tlm_slave"
            self.tlm_slave_[i].port_data = "slave_transactor_{}_{}".format(self.version, i)
            self.tlm_master_[i].port_type = "tlm_master"
            self.tlm_master_[i].port_data = "master_transactor_{}_{}".format(self.version, i)

        # connect requestToDir
        #self.requestToDir.master = ruby_system.network.slave
        self.requestToDir.master = self.tlm_slave_[0].port
        self.tlm_master_[0].port = ruby_system.network.slave
        #ruby_system.network.slave = self.tlm_master_[0].port

        # connect requestToDirOrSibling
        #self.responseToDirOrSibling.master = ruby_system.network.slave
        self.responseToDirOrSibling.master = self.tlm_slave_[1].port
        self.tlm_master_[1].port = ruby_system.network.slave

        # connect forwardFromDir
        self.forwardFromDir.slave = ruby_system.network.master
        #ruby_system.network.master = self.tlm_slave[2].port
        #self.tlm_master[2].port = self.forwardFromDir.slave

        # connect responseFromDirOrSibling
        self.responseFromDirOrSibling.slave = ruby_system.network.master
        #ruby_system.network.master = self.tlm_slave[3].port
        #self.tlm_master[3].port = self.responseFromDirOrSibling.slave


class DirController(Directory_Controller):

    _version = 0
    @classmethod
    def versionCount(cls):
        cls._version += 1 # Use count for this particular type
        return cls._version - 1

    def __init__(self, ruby_system, ranges, mem_ctrls):
        """ranges are the memory ranges assigned to this controller.
        """
        if len(mem_ctrls) > 1:
            panic("This cache system can only be connected to one mem ctrl")
        super(DirController, self).__init__()
        self.version = self.versionCount()
        self.addr_ranges = ranges
        self.ruby_system = ruby_system
        self.directory = RubyDirectoryMemory()
        # Connect this directory to the memory side.
        self.memory = mem_ctrls[0].port
        self.connectQueues(ruby_system)

    def connectQueues(self, ruby_system):
        self.requestFromCache = MessageBuffer(ordered = True)
        self.requestFromCache.slave = ruby_system.network.master
        self.responseFromCache = MessageBuffer(ordered = True)
        self.responseFromCache.slave = ruby_system.network.master

        self.responseToCache = MessageBuffer(ordered = True)
        self.responseToCache.master = ruby_system.network.slave
        self.forwardToCache = MessageBuffer(ordered = True)
        self.forwardToCache.master = ruby_system.network.slave

        # This is another special message buffer. It is used to send replies
        # from memory back to the controller. Any messages received on the
        # memory port (see self.memory above) will be directed to this
        # message buffer.
        self.responseFromMemory = MessageBuffer()

class MyNetwork(SimpleNetwork):
    """A simple point-to-point network. This doesn't not use garnet.
    """

    def __init__(self, ruby_system):
        super(MyNetwork, self).__init__()
        self.netifs = []
        self.ruby_system = ruby_system

    def connectControllers(self, controllers):
        """Connect all of the controllers to routers and connec the routers
           together in a point-to-point network.
        """
        # Create one router/switch per controller in the system
        self.routers = [Switch(router_id = i) for i in range(len(controllers))]

        # Make a link from each controller to the router. The link goes
        # externally to the network.
        self.ext_links = [SimpleExtLink(link_id=i, ext_node=c,
                                        int_node=self.routers[i])
                          for i, c in enumerate(controllers)]

        # Make an "internal" link (internal to the network) between every pair
        # of routers.
        link_count = 0
        self.int_links = []
        for ri in self.routers:
            for rj in self.routers:
                if ri == rj: continue # Don't connect a router to itself!
                link_count += 1
                self.int_links.append(SimpleIntLink(link_id = link_count,
                                                    src_node = ri,
                                                    dst_node = rj))
