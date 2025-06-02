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
# |        Membus        |    v
# +----------------+-----+    External Port (see sc_slave_port.*)
#                  |          ^
#              +---v---+      | TLM World
#              |  TLM  |      | (see sc_target.*)
#              +-------+      v
#

# Create a system with a Crossbar and a TrafficGenerator as CPU:
system = System()
system.membus = IOXBar(width = 16)
system.physmem = SimpleMemory() # This must be instanciated, even if not needed

# Create a config to be used by all the traffic generators
cfg_file_name = "tlm_slave.cfg"
cfg_file = open(cfg_file_name, 'w')

# Three states, with random, linear and idle behaviours. The random
# and linear states access memory in the range [0 : 16 Mbyte] with 8
# byte and 64 byte accesses respectively.
cfg_file.write("STATE 0 10000000 RANDOM 65 0 16777216 8 50000 150000 0\n")
cfg_file.write("STATE 1 10000000 LINEAR 65 0 16777216 64 50000 150000 0\n")
cfg_file.write("STATE 2 10000000 IDLE\n")
cfg_file.write("INIT 0\n")
cfg_file.write("TRANSITION 0 1 0.5\n")
cfg_file.write("TRANSITION 0 2 0.5\n")
cfg_file.write("TRANSITION 1 0 0.5\n")
cfg_file.write("TRANSITION 1 2 0.5\n")
cfg_file.write("TRANSITION 2 0 0.5\n")
cfg_file.write("TRANSITION 2 1 0.5\n")
cfg_file.close()

system.cpu = TrafficGen(config_file = cfg_file_name)

system.clk_domain = SrcClockDomain(clock = '1.5GHz',
    voltage_domain = VoltageDomain(voltage = '1V'))

# Create a external TLM port:
system.tlm = ExternalSlave()
system.tlm.addr_ranges = [AddrRange('512MB')]

# sim_controll.cc will pre-register tlm_master&tlm_slave portHandler
# tlm_salve is register as to SCSlavePortHandler in ExternalSlave class (external_slave.cc)
# sim_controll.cc also hook up event queue, load ini file, and instantiate all object
system.tlm.port_type = "tlm_slave"

# when bindPort, the ExternalSlave object will bind their port
# by using SCSlavePortHandler's getExternalPort, the port_data is pass as argument
# SClavePortHandler create new SClavePort by use port_data as name
# and call sim_control to register port_data into slavePorts via registerSlavePort
#    std::map<const std::string, SCSlavePort*> slavePorts;
#    std::map<const std::string, SCMasterPort*> masterPorts;
system.tlm.port_data = "transactor"

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

# Route the connections:
system.cpu.port = system.membus.slave
system.system_port = system.membus.slave
system.membus.master = system.tlm.port

# Start the simulation:
root = Root(full_system = False, system = system)
root.system.mem_mode = 'timing'
m5.instantiate()
m5.simulate() #Simulation time specified later on commandline
