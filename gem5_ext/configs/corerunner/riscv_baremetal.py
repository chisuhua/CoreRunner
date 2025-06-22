import m5
import os
import argparse
from m5.objects import *

# 解析命令行参数
parser = argparse.ArgumentParser(
    formatter_class=argparse.ArgumentDefaultsHelpFormatter
)
parser.add_argument("--cores", type=int, default=2, help="Number of CPUs")
parser.add_argument("-g", "--gdb", action="store_true", help="Wait for GDB connection")
args = parser.parse_args()

# 定义 ELF 文件路径
thispath = os.path.dirname(os.path.realpath(__file__))
binary = os.path.join(thispath, "../../../build/quarks/kernel.elf")

# 创建系统对象
system = System()
system.clk_domain = SrcClockDomain(clock='1GHz', voltage_domain=VoltageDomain())
system.mem_mode = 'timing'
system.mem_ranges = [AddrRange('512MB')]

# 创建互连总线
system.membus = SystemXBar()

# 创建多个 CPU Core
system.cpus = [RiscvTimingSimpleCPU() for _ in range(args.cores)]
process = Process()
process.cmd = [binary]


# 为每个 CPU 设置 hart_id、中断控制器、缓存端口
for i, cpu in enumerate(system.cpus):
    cpu.numThreads = 1
    cpu.createInterruptController()
    cpu.icache_port = system.membus.cpu_side_ports
    cpu.dcache_port = system.membus.cpu_side_ports

for i, cpu in enumerate(system.cpus):
    cpu.workload = process
    cpu.createThreads()
    cpu.isa[0].riscv_type = "RV32"
    cpu.isa[0].enable_rvv = False

# 添加内存控制器
system.ram = SimpleMemory()
system.ram.port = system.membus.mem_side_ports

# 系统级端口（用于加载程序等）
system.system_port = system.membus.cpu_side_ports

# 创建工作负载（共享同一个 ELF）
system.workload = SEWorkload.init_compatible(binary)
if args.gdb:
    system.workload.wait_for_remote_gdb = True
else:
    system.workload.wait_for_remote_gdb = False


# 根对象与实例化
root = Root(full_system=False, system=system)
m5.instantiate()

print("Beginning simulation!")
exit_event = m5.simulate()
print(f"Exiting @ tick {m5.curTick()} because {exit_event.getCause()}")
