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

parser = argparse.ArgumentParser(
    formatter_class=argparse.ArgumentDefaultsHelpFormatter
)

parser.add_argument("binary", default="tests/test-progs/hello/bin/riscv/linux/hello", nargs="?", type=str,
                    help="Path to the binary to execute.")
parser.add_argument(
    "-g", "--gdb", action="store_true", help="Use atomic (non-timing) mode"
)

parser.add_argument(
    "-c", "--cores", default=1, type=int, help="Use atomic (non-timing) mode"
)



args = parser.parse_args()

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

# 配置 CPU
processor = SimpleProcessor(
    cpu_type=CPUTypes.TIMING, isa=ISA.RISCV, num_cores=args.cores
)

for i,cpu in enumerate(processor.cores):
    cpu.core.isa[i].enable_rvv = False
    cpu.core.isa[i].riscv_type = "RV32"
    cpu.core.hard_id = i

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

if args.gdb:
    board.workload.wait_for_remote_gdb = True
else:
    board.workload.wait_for_remote_gdb = False

# 运行仿真
simulator = Simulator(board=board)
print("开始运行 gem5 仿真...")
simulator.run()
print(
    "Exiting @ tick {} because {}.".format(
        simulator.get_current_tick(), simulator.get_last_exit_event_cause()
    )
)
