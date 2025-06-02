# Copyright (c) 2015, University of Kaiserslautern
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
# TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
# OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Authors: Matthias Jung


import m5
from m5.objects import *

# This configuration shows a simple setup of a TrafficGen (CPU) and an
# external TLM port for SystemC co-simulation
#
# Base System Architecture:
# +-------------+  +-----+    ^
# | System Port |  | CPU |    |
# +-------+-----+  +--+--+    |
#         |           |       | gem5 World
#         |      +----+       | (see this file)
#         |      |            |
# +-------v------v-------+    |
# |        Membus1       |    v
# +----------------+-----+    External Port (see sc_slave_port.*)
#       |            |          ^
#   +---v---+    +---v---+      | TLM World
#   |  TLM  |    |  TLM  |      | (see sc_target.*)
#   +-------+    +-------+      v
##       |            |          ^
#   +---v---+    +---v---+      | TLM World
#   |  TLM  |    |  TLM  |      | (see sc_initiator.*)
#   +-------+    +-------+      v
#       |            |          |
# +----------------v-----+    |
# |        Membus2       |    v
# +----------------+-----+    External Port (see sc_slave_port.*)
#            |                | gem5 World
#        +---v----+           |
#        | Memory |           |
#        +--------+           v
#
# Create a system with a Crossbar and a TrafficGenerator as CPU:
system = System()
system.membus1 = IOXBar(width = 16)
system.membus2 = IOXBar(width = 16)

# This must be instanciated, even if not needed
system.physmem = SimpleMemory(range = AddrRange('512MB'))

#system.cpu = TrafficGen(config_file = "conf/tgen.cfg")
system.cpu = MemTest(max_loads = 1e5, progress_interval = 16,
                     progress_check = 1e4,
                    size = 0x20000000,
                    block_addrmask = 1,
                    base_addr1 = 0,
                    base_addr2 = 0,
                    uncache_addr = 0,
                    percent_functional = 0,
                    start_tick = 20000,
                    interval = 16
                    )

system.clk_domain = SrcClockDomain(clock = '1.5GHz',
    voltage_domain = VoltageDomain(voltage = '1V'))

# Create a external TLM port:
system.tlm_slave_0 = ExternalSlave()
system.tlm_slave_0.addr_ranges = [AddrRange('256MB')]

# sim_controll.cc will pre-register tlm_master&tlm_slave portHandler
# so port_type is either tlm_master or tlm_slave

# tlm_salve is register as to SCSlavePortHandler in ExternalSlave class (external_slave.cc)
# sim_controll.cc also hook up event queue, load ini file, and instantiate all object
system.tlm_slave_0.port_type = "tlm_slave"

# when bindPort, the ExternalSlave object will bind their port
# by using SCSlavePortHandler's getExternalPort, the port_data is pass as argument
# SClavePortHandler create new SClavePort by use port_data as name
# and call sim_control to register port_data into slavePorts via registerSlavePort
#    std::map<const std::string, SCSlavePort*> slavePorts;
#    std::map<const std::string, SCMasterPort*> masterPorts;
system.tlm_slave_0.port_data = "slave_transactor_0"

# in main.c it need to create a Gem5SlaveTransactor, it use same port name as port_data
# Gem5SlaveTransactor is sc_module it will call sim_control to get SlavePort and bind
# the port 's transactor it itself
#Gem5SlaveTransactor::before_end_of_elaboration()
#{
#    auto* port = sim_control->getSlavePort(portName);
#    port->bindToTransactor(this);  # it register transactor's socket to SCSlavePort::nb_transport_bw
#}
#
# in main.c this transcator's socket is bind to real TLM socket
#       the real slave TLM refer to examples/slave_port/sc_target.cc
#       the TLM socket will register b/nb_transofort_fw/bw method define in sc_target.cc


# Create a external TLM master port:
system.tlm_master_0 = ExternalMaster()

system.tlm_master_0.port_type = "tlm_master"
system.tlm_master_0.port_data = "master_transactor_0"


# pair 1
system.tlm_slave_1 = ExternalSlave()
system.tlm_slave_1.addr_ranges = [AddrRange(start=Addr(0x10000000), size='256MB')]
system.tlm_slave_1.port_type = "tlm_slave"
system.tlm_slave_1.port_data = "slave_transactor_1"

system.tlm_master_1 = ExternalMaster()
system.tlm_master_1.port_type = "tlm_master"
system.tlm_master_1.port_data = "master_transactor_1"


# Route the connections:
system.cpu.port = system.membus1.slave
system.system_port = system.membus1.slave
system.membus1.master = system.tlm_slave_0.port
system.membus1.master = system.tlm_slave_1.port

system.tlm_master_0.port = system.membus2.slave
system.tlm_master_1.port = system.membus2.slave

system.physmem.port = system.membus2.master

system.memchecker = MemChecker()

# -----------------------
# run simulation
# -----------------------

# Start the simulation:
root = Root(full_system = False, system = system)
root.system.mem_mode = 'timing'

m5.instantiate()
#m5.simulate() #Simulation time specified later on commandline
exit_event = m5.simulate(1000000000)
if exit_event.getCause() != "simulate() limit reached":
    exit(1)
