import m5
import os
from m5.objects import  *

thispath = os.path.dirname(os.path.realpath(__file__))
binary = os.path.join(
    thispath,
    "../../../",
    "build/quarks/kernel.elf",
)

system = System()


system.clk_domain = SrcClockDomain()
system.clk_domain.clock = '1GHz'
system.clk_domain.voltage_domain = VoltageDomain()


system.mem_mode = 'timing'
system.mem_ranges = [AddrRange('512MB')]


system.membus = SystemXBar()

system.cpu = RiscvTimingSimpleCPU()
system.cpu.createInterruptController()
system.cpu.icache_port = system.membus.cpu_side_ports
system.cpu.dcache_port = system.membus.cpu_side_ports

system.system_port = system.membus.cpu_side_ports

system.ram = SimpleMemory(
    )
system.ram.port = system.membus.mem_side_ports


#processor.cores[0].core.isa[0].riscv_type = "RV32"

system.workload = SEWorkload.init_compatible(binary)

process = Process()
process.cmd = [binary]

system.cpu.workload = process
system.cpu.createThreads()

system.cpu.isa[0].riscv_type = "RV32"
system.cpu.isa[0].enable_rvv = False

root = Root(full_system = False, system = system)
m5.instantiate()

print("Beginning simulation")
exit_event = m5.simulate()

print('Exiting @ tick {} because {}' .format(m5.curTick(), exit_event.getCause()))
