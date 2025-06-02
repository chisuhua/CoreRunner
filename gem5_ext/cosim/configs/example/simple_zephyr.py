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
system.physmem = SimpleMemory( range = AddrRange(0x00000000, size='1GB')) # This must be instanciated, even if not needed

#system.cpu = TrafficGen(config_file = "conf/tgen.cfg")
system.zephyr = Zephyr(max_loads = 1e5, progress_interval = 16,
                    progress_check = 1e4,
                    size = 16,
                    block_addrmask = 1,
                    base_addr1 = 0,
                    base_addr2 = 0,
                    uncache_addr = 0,
                    percent_functional = 0,
                    start_tick = 20000,
                    interval = 16,
                    file_name="/work_source/github/sim/zephyrproject/zephyr/build_posix_gem5/zephyr/zephyr.exe"
                    )

system.clk_domain = SrcClockDomain(clock = '1.5GHz',
    voltage_domain = VoltageDomain(voltage = '1V'))

# Route the connections:
system.zephyr.port = system.membus.slave
system.system_port = system.membus.slave
system.membus.master = system.physmem.port

system.memchecker = MemChecker()

# -----------------------
# run simulation
# -----------------------

# Start the simulation:
root = Root(full_system = False, system = system)
root.system.mem_mode = 'timing'

m5.instantiate()
#m5.simulate() #Simulation time specified later on commandline
exit_event = m5.simulate(1000000000000)
#exit_event = m5.simulate()
print(exit_event.getCause())
if exit_event.getCause() != "simulate() limit reached":
    exit(1)
