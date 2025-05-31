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
import os

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
memory = SingleChannelDDR3_1600()

#memory = SimpleMemory()
#memory = SimpleMemory(
#    range=AddrRange(0x80000000, size="8MiB"),
#)

# 配置 CPU
processor = SimpleProcessor(
    cpu_type=CPUTypes.TIMING, isa=ISA.RISCV, num_cores=1
)

processor.cores[0].core.isa[0].enable_rvv = False
processor.cores[0].core.isa[0].riscv_type = "RV32"

# 配置缓存（无缓存）
cache_hierarchy = NoCache()

# 构建 Board
board = RiscvBoard(
    clk_freq="2GHz",
    processor=processor,
    memory=memory,
    cache_hierarchy=cache_hierarchy
)

# 设置工作负载（启动地址 0x80000000）
board.set_se_binary_workload(
    binary=BinaryResource(local_path=binary)
)

#board.set_kernel_disk_workload(
#   kernel=BinaryResource(local_path=options.binary),
#   disk_image=options.binary
#
# 运行仿真
simulator = Simulator(board=board)
print("开始运行 gem5 仿真...")
simulator.run()
print(
    "Exiting @ tick {} because {}.".format(
        simulator.get_current_tick(), simulator.get_last_exit_event_cause()
    )
)
