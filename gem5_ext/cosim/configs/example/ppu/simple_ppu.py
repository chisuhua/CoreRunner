# Copyright (c) 2018 TU Dresden
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
# Authors: Robert Scheffel

'''
System definition.
'''

from m5.objects import *
#from minor_custom_fu import PpuMinorCustomFUPool
import pdb

#use_tlm = True
use_tlm = False

class PPUFUPool(PpuMinorFUPool):
    funcUnits = [PpuMinorDefaultIntFU(), PpuMinorDefaultIntFU(),
                 PpuMinorDefaultIntMulFU(), PpuMinorDefaultIntDivFU(),
                 PpuMinorDefaultFloatSimdFU(), PpuMinorDefaultMemFU(),
                 PpuMinorDefaultMiscFU()]

class MemBus(SystemXBar):
    badaddr_responder = BadAddr(warn_access="warn")
    default = Self.badaddr_responder.pio

    frontend_latency = 0
    forward_latency = 0
    response_latency = 0
    snoop_response_latency = 0
    snoop_filter = SnoopFilter(lookup_latency=1)


class PPSystem(BareMetalPpuSystem):
    """
    pp system containing just CPU, bus and memory
    """

    def __init__(self, cpu_class, wfgdb, **kwargs):
        super(PPSystem, self).__init__(**kwargs)

        # create clock and voltage domain
        # set to 100MHz clock frequency (like real hw board)
        self.clk_domain = SrcClockDomain(clock='100MHz')
        self.clk_domain.voltage_domain = VoltageDomain()

        # set up our board
        self.board = PpuSystemBoard(
                    #timer_cpu=TimerCpu(pio_addr=0x4400bff8),
                    #uart = Uart8250(pio_addr=0x70013000, pio_size=0x1000)
                    )
        # appearently we need a dedicated interrupt controller
        self.intrctrl = IntrControl()

        # specify functional unit
        self.execFU = PPUFUPool()

        # create cpu and thread
        # the interruptcontroller needs to be created as well
        # tell the cpu if it shall wait until a gdb gets
        # connected remotely or not
        self.cpu = cpu_class()
        self.cpu.createThreads()
        self.cpu.createInterruptController()
        self.cpu.wait_for_remote_gdb = wfgdb

        # TODO:
        # better: modify existing isa
        # read only, init value
        # regs = [0x800, 0xcc0]
        #self.cpu.isa[0].cust_regs = regs

        # system memory bus
        self.membus = MemBus()

        #self.icache_bridge = Bridge()
        #self.dcache_bridge = Bridge()

        # Create a external TLM port:
        if use_tlm:
            self.tlm_slave_dcache = ExternalSlave()
            self.tlm_slave_dcache.addr_ranges = [AddrRange('4GB')]
            self.tlm_slave_dcache.port_type = "tlm_slave"
            self.tlm_slave_dcache.port_data = "slave_transactor_dcache"
            self.tlm_master_dcache = ExternalMaster()
            self.tlm_master_dcache.port_type = "tlm_master"
            self.tlm_master_dcache.port_data = "master_transactor_dcache"

            # Create a external TLM port:
            self.tlm_slave_icache = ExternalSlave()
            self.tlm_slave_icache.addr_ranges = [AddrRange('4GB')]
            #self.tlm_slave_icache.addr_ranges = [AddrRange(start=Addr(0x10000000), size='256MB')]
            self.tlm_slave_icache.port_type = "tlm_slave"
            self.tlm_slave_icache.port_data = "slave_transactor_icache"
            self.tlm_master_icache = ExternalMaster()
            self.tlm_master_icache.port_type = "tlm_master"
            self.tlm_master_icache.port_data = "master_transactor_icache"

        # create mem_range
        # for now start at 0x0
        # take 3GB as size as it is a bit lower than 0xffffffff bytes
        mem_start = Addr(0x00000000)
        mem_size = '1GB'
        self.mem_ranges = [AddrRange(start=mem_start, size=mem_size)]

        # define bit mode
        # self.rv32 = True

    def connect(self):
        # connect cache ports of cpu to membus
        # no caches -> connect directly to mem bus

        #self.cpu.connectAllPorts(self.membus)
        if use_tlm:
            self.cpu.dcache_port = self.tlm_slave_dcache.port
            self.cpu.icache_port = self.tlm_slave_icache.port
            self.tlm_master_dcache.port = self.membus.slave
            self.tlm_master_icache.port = self.membus.slave
        else:
            self.cpu.connectAllPorts(self.membus)
            #self.cpu.dcache_port = self.membus.slave
            #self.cpu.icache_port = self.membus.slave

        self.system_port = self.membus.slave

        self.board.attachIO(self.membus)
