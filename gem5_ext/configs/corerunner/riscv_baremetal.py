from m5.objects import *
from gem5.components.boards.riscv_board import RiscvBoard
from gem5.components.cachehierarchies.classic.no_cache import NoCache
from gem5.components.processors.simple_processor import SimpleProcessor
from gem5.components.processors.cpu_types import CPUTypes 
from gem5.isas import ISA
from gem5.components.memory import SingleChannelDDR3_1600
from gem5.simulate.simulator import Simulator
from gem5.resources.resource import BinaryResource
import argparse

parser = argparse.ArgumentParser(description='A simple system with 2-level cache.')
parser.add_argument("binary", default="tests/test-progs/hello/bin/riscv/linux/hello", nargs="?", type=str,
                    help="Path to the binary to execute.")

options = parser.parse_args()

thispath = os.path.dirname(os.path.realpath(__file__))
binary = os.path.join(
    thispath,
    "../../../",
    "build/quarks/kernel.elf",
)

# 配置内存
#memory = SimpleMemory(
#    size="64MB",
#    read_latency="1ns",
#    write_latency="1ns"
#)
#memory = SingleChannelDDR3_1600()

# 配置 CPU
processor = SimpleProcessor(
    cpu_type=CPUTypes.TIMING, isa=ISA.RISCV, num_cores=1
)

# 配置缓存（无缓存）
cache_hierarchy = NoCache()

# 构建 Board
#board = RiscvBoard(
#    clk_freq="2GHz",
#    processor=processor,
#    memory=memory,
#    cache_hierarchy=cache_hierarchy
#)

# 设置工作负载（启动地址 0x80000000）
#board.set_se_binary_workload(
#    binary_path=options.binary
#)
#board.workload = RiscvBareMetal()
#board.workload.bootloader=options.binary
#board._set_fullsystem(True)
#board._bootloader=options.binary
#board.set_kernel_disk_workload(
#    kernel=BinaryResource(local_path=options.binary),
#    disk_image=options.binary
#)

#range_start=0x80000000
range_start=0x00000000

system = System()
system.clk_domain = SrcClockDomain()
system.clk_domain.clock = '1GHz'
system.clk_domain.voltage_domain = VoltageDomain()
system.mem_mode = 'timing'
system.mem_ranges = [AddrRange(start=range_start, size='1GiB')]
system.membus = SystemXBar()

#system.cpu = SimpleProcessor(
#    cpu_type=CPUTypes.TIMING, isa=ISA.RISCV, num_cores=1
#)
system.cpu = RiscvTimingSimpleCPU()
system.cpu.createInterruptController()
system.cpu.icache_port = system.membus.cpu_side_ports
system.cpu.dcache_port = system.membus.cpu_side_ports

system.workload = RiscvBareMetal()
system.workload.bootloader = binary

system.ram = SimpleMemory(
        #image_file=binary,
        #range=AddrRange(0x80000000, size="8MiB"),
    )

system.ram.port = system.membus.mem_side_ports

system.cpu.createThreads()
system.system_port = system.membus.cpu_side_ports

root = Root(full_system = True, system = system)
m5.instantiate()
exit_event = m5.simulate()
print('Exiting @ tick {} because {}' .format(m5.curTick(), exit_event.getCause()))

